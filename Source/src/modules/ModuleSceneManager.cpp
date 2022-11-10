#include "core/hepch.h"
#include "ModuleSceneManager.h"

#include "ModuleCamera.h"
#include "ModuleEvent.h"
#include "ModuleNavigation.h"

#include "resources/ResourceScene.h"
#include "ModuleResources.h"
#include "ModuleEditor.h"

#include "core/preferences/src/ResourcesPreferences.h"
#include "core/preferences/src/EditorPreferences.h"
#include "importers/SceneImporter.h"

#include <iomanip>
#include <ctime>
#include <thread>

bool Hachiko::ModuleSceneManager::Init()
{
    HE_LOG("INITIALIZING MODULE: SCENE MANAGER");

    preferences = App->preferences->GetResourcesPreference();

#ifdef PLAY_BUILD
    // If the engine is built in play mode, we want to force the game to start
    // after it loads the scene, on the first update:
    should_trigger_attempt_play = true;
#endif

    // If uid is not found this will load an empty scene:
    LoadScene(preferences->GetSceneUID(), false);

    std::function handle_scene_swapping = [&](Event& evt) {
        Scene* scene = 
            evt.GetEventData<EditorHistoryEntryRestore>().GetScene();

        ChangeMainScene(scene);
    };

    App->event->Subscribe(
        Event::Type::RESTORE_EDITOR_HISTORY_ENTRY, 
        handle_scene_swapping);

#ifndef PLAY_BUILD
    EditorPreferences* editor_prefs = App->preferences->GetEditorPreference();
    scene_autosave = editor_prefs->GetAutosave();
#endif
    return true;
}

void Hachiko::ModuleSceneManager::AttemptScenePause()
{
    if (GameTimer::paused)
    {
        return;
    }

    Event game_state(Event::Type::GAME_STATE);
    game_state.SetEventData<GameStateEventPayload>(GameStateEventPayload::State::PAUSED);
    App->event->Publish(game_state);

    GameTimer::Pause();
}

void Hachiko::ModuleSceneManager::AttemptScenePlay() const
{
    ComponentCamera* scene_camera = main_scene->GetMainCamera();

    if (scene_camera == nullptr)
    {
        HE_LOG("Current scene does not have a CameraComponent inside."
            " Therefore, cannot enter Play Mode.");
        return;
    }

    main_scene->SetCullingCamera(scene_camera);

    App->camera->SetRenderingCamera(scene_camera);

    if (!GameTimer::running || should_trigger_attempt_play)
    {
        RefreshSceneResource();

        main_scene->Start();

        Event game_state(Event::Type::GAME_STATE);
        game_state.SetEventData<GameStateEventPayload>(
            GameStateEventPayload::State::STARTED);

        App->event->Publish(game_state);

        GameTimer::Start();
    }
    else if (GameTimer::paused)
    {
        Event game_state(Event::Type::GAME_STATE);
        game_state.SetEventData<GameStateEventPayload>(
            GameStateEventPayload::State::RESUMED);

        App->event->Publish(game_state);

        GameTimer::Resume();
    }
}

void Hachiko::ModuleSceneManager::AttemptSceneStop()
{
    if (GameTimer::running)
    {
        StopScene();
        ReloadScene();
    }
}

bool Hachiko::ModuleSceneManager::IsScenePlaying()
{
    return GameTimer::running && !GameTimer::paused;
}

void Hachiko::ModuleSceneManager::RebuildBatches() const
{
    if (main_scene)
    {
        main_scene->RebuildBatches();
    }
}

void Hachiko::ModuleSceneManager::CheckSceneLoading() 
{
    if (loading_scene)
    {
        return;
    }

    HE_LOG("Loading the scene.");

    loading_scene_worker.join();

    App->resources->PostLoadSceneResources();
    tmp_loading_scene->GetSkybox()->Reload();

    PostLoadScene();

    App->FinishLoading();

    HE_LOG("Scene is loaded.");
}

UpdateStatus Hachiko::ModuleSceneManager::Update(const float delta)
{
    if (should_trigger_attempt_play)
    {
        AttemptScenePlay();
        should_trigger_attempt_play = false;
    }

    main_scene->Update();

    return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus Hachiko::ModuleSceneManager::PostUpdate(float delta)
{
    if (scene_reload_requested)
    {
        // Use the scene resource refreshed when play is pressed to restore og
        // state. Basically scene serialized in memory keeps the currently
        // defined navmesh since the one saved in library is from the last time
        // we saved:

        scene_reload_requested = false;

        constexpr bool keep_navigation_mesh = true;

        LoadScene(scene_resource->GetID(), keep_navigation_mesh);

        return UpdateStatus::UPDATE_CONTINUE;
    }

    if (scene_change_requested)
    {
        scene_change_requested = false;

        LoadScene(scene_to_load_id);

        return UpdateStatus::UPDATE_CONTINUE;
    }

    if (!to_remove.empty())
    {
        while (!to_remove.empty())
        {
            GameObject* go = to_remove[to_remove.size() - 1];

            if (App->editor->GetSelectedGameObject() == go)
            {
                App->editor->SetSelectedGO(nullptr);
            }

            delete go;
        }
    }

    return UpdateStatus::UPDATE_CONTINUE;
}

bool Hachiko::ModuleSceneManager::CleanUp()
{
    if (scene_autosave)
    {
        SaveScene();
    }

    if (IsScenePlaying())
    {
        main_scene->Stop();
    }

    EditorPreferences* editor_prefs = App->preferences->GetEditorPreference();
    editor_prefs->SetAutosave(scene_autosave);

#ifndef PLAY_BUILD
    // If such scene was a temporary one it will set id to 0 which will
    // generate a new temporary scene on load:
    preferences->SetSceneUID(scene_resource->GetID());
    preferences->SetSceneName(scene_resource->name.c_str());
#endif

    SetSceneResource(nullptr);

    // Release main_scene because it's not owned by the resource manager:
    RELEASE(main_scene);

    return true;
}

void Hachiko::ModuleSceneManager::RemoveGameObject(GameObject* game_object)
{
    if (!game_object)
    {
        return;
    }

    if (!game_object->scene_owner)
    {
        delete game_object;
        game_object = nullptr;
        return;
    }

    to_remove.push_back(game_object);
}

void Hachiko::ModuleSceneManager::RemovedGameObject(GameObject* go)
{
    for (auto it = std::find(to_remove.begin(), to_remove.end(), go); it != to_remove.end();)
    {
        to_remove.erase(it);
        it = std::find(to_remove.begin(), to_remove.end(), go);
    }
}

void Hachiko::ModuleSceneManager::LoadEmptyScene()
{
    tmp_loading_scene = new Scene();

    GameObject* camera_go = tmp_loading_scene->CreateNewGameObject(
        tmp_loading_scene->GetRoot(), // parent
        "Main Camera"); // name

    camera_go->CreateComponent(Component::Type::CAMERA);

    // Default name with timestamp for new empty untitled scenes
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << "untitled_scene_" << std::put_time(&tm, "%d-%m-%Y_%H-%M-%S");
    tmp_loading_scene->name = oss.str().c_str();

    // Create a scene resource not related to an asset to reload the scene as any other
    SceneImporter scene_importer;
    tmp_resource_scene = scene_importer.CreateSceneResource(tmp_loading_scene);

    App->navigation->SetNavmesh(nullptr);

    tmp_loading_scene->loaded = true;

    PostLoadScene();
}

void Hachiko::ModuleSceneManager::PostLoadScene()
{
    SetSceneResource(tmp_resource_scene);
    ChangeMainScene(tmp_loading_scene);
    
    tmp_resource_scene = nullptr;
    tmp_loading_scene = nullptr;
}

void Hachiko::ModuleSceneManager::StopScene() const
{
    Event game_state(Event::Type::GAME_STATE);
    game_state.SetEventData<GameStateEventPayload>(GameStateEventPayload::State::STOPPED);
    App->event->Publish(game_state);

    App->camera->SetRenderingCamera(App->camera->GetEditorCamera());
    main_scene->SetCullingCamera(App->camera->GetEditorCamera());
    main_scene->Stop();
    GameTimer::Stop();
}

void Hachiko::ModuleSceneManager::LoadScene(
    UID new_scene_id, 
    bool keep_navigation_mesh)
{
    // Stop the scene if it was playing to avoid scripts calling Start:
    const bool was_scene_playing = IsScenePlaying();

    if (was_scene_playing)
    {
        StopScene();
    }
    
    // If the scene was playing previously, continue playing. If should trigger
    // attempt play was marked true in any other functions, let it stay as true.
    should_trigger_attempt_play = 
        was_scene_playing || should_trigger_attempt_play;

    const bool does_resource_exist = 
        App->resources->DoesResourceExist(Resource::Type::SCENE, new_scene_id);
    const bool is_scene_id_valid = new_scene_id != 0;

    if (is_scene_id_valid && does_resource_exist)
    {        
        HE_LOG("START LOADING");

        App->StartLoading();

        loading_scene = true;

        loading_scene_worker = std::thread(
            &ModuleSceneManager::ThreadLoadScene, // worker function
            this, // implicit this at the start of ThreadLoadScene method
            new_scene_id, // scene_id parameter of ThreadLoadScene method
            keep_navigation_mesh); // keep_navmesh parameter of ThreadLoadScene
                                   // method
    }
    else
    {
        // Load an empty scene
        LoadEmptyScene();
    }
}

void Hachiko::ModuleSceneManager::SaveScene(const char* save_name)
{
    if (IsScenePlaying())
    {
        AttemptSceneStop();
    }

    RefreshSceneResource();

    if (save_name)
    {
        main_scene->name = save_name;
    }
    SceneImporter scene_importer;
    UID saved_scene_uid = scene_importer.CreateSceneAsset(main_scene);
}

Hachiko::GameObject* Hachiko::ModuleSceneManager::RayCast(
    const float3& origin, 
    const float3& destination, 
    float3* closest_hit, 
    GameObject* parent_filter, 
    bool active_only) const
{
    return main_scene->RayCast(
        origin, 
        destination, 
        closest_hit, 
        parent_filter, 
        active_only);
}

Hachiko::GameObject* Hachiko::ModuleSceneManager::BoundingRayCast(
    const float3& origin,
    const float3& destination, 
    GameObject* parent_filter, 
    bool active_only) const
{
    return main_scene->RayCast(origin, destination, nullptr, parent_filter, active_only);
}

void Hachiko::ModuleSceneManager::ChangeSceneById(
    UID new_scene_id, 
    bool stop_scene)
{
    scene_change_requested = true;
    scene_to_load_id = new_scene_id;

    if (stop_scene)
    {
        StopScene();
    }
}

void Hachiko::ModuleSceneManager::ReloadScene()
{
    // Set flag to reload current scene
    scene_reload_requested = true;
}

void Hachiko::ModuleSceneManager::OptionsMenu()
{
    std::string scene_name =main_scene->GetName();

    constexpr ImGuiInputTextFlags name_input_flags = 
        ImGuiInputTextFlags_AutoSelectAll | 
        ImGuiInputTextFlags_EnterReturnsTrue;

    if (Widgets::Input("Scene name", scene_name, name_input_flags))
    {
        main_scene->SetName(scene_name.c_str());
    }

    Widgets::Checkbox("Autosave scene", &scene_autosave);
    ImGui::Separator();
    main_scene->AmbientLightOptionsMenu();
    ImGui::Separator();
    main_scene->FogOptionsMenu();
    ImGui::Separator();
    main_scene->SkyboxOptionsMenu();
}

void Hachiko::ModuleSceneManager::ThreadLoadScene(UID scene_id, bool keep_navmesh)
{
    // Load the scene
    tmp_resource_scene = static_cast<ResourceScene*>(
        App->resources->GetResource(Resource::Type::SCENE, scene_id));

    if (!keep_navmesh)
    {
        App->navigation->SetNavmesh(tmp_resource_scene->scene_data[NAVMESH_ID].as<UID>());
    }

    App->resources->LoadSceneResources(tmp_resource_scene->scene_data);

    tmp_loading_scene = new Scene();
    tmp_loading_scene->Load(tmp_resource_scene->scene_data);

    loading_scene = false;
}

void Hachiko::ModuleSceneManager::ChangeMainScene(Scene* new_scene)
{
    // Loads scene if a pointer is passed, nullpointer creates empty scene
    Event scene_load(Event::Type::SCENE_LOADED);
    scene_load.SetEventData<SceneLoadEventPayload>(SceneLoadEventPayload::State::NOT_LOADED);
    App->event->Publish(scene_load);

    // We are already releasing scene so we need to update the to remove_vector
    to_remove.clear();
    RELEASE(main_scene);
    main_scene = new_scene;

    scene_load.SetEventData<SceneLoadEventPayload>(SceneLoadEventPayload::State::LOADED);
    App->event->Publish(scene_load);
}

void Hachiko::ModuleSceneManager::SetSceneResource(
    ResourceScene* new_scene_resource)
{
    if (scene_resource != new_scene_resource)
    {
        // Only delete if it exists, its not the same resource and its not
        // loaded by resource manager (id 0)
        if (scene_resource && scene_resource->GetID() == 0)
        {
            delete scene_resource;
        }
        else
        {
            App->resources->ReleaseResource(scene_resource);
        }
    }
    scene_resource = new_scene_resource;
}

void Hachiko::ModuleSceneManager::RefreshSceneResource() const
{
    SceneImporter scene_importer;
    scene_importer.RefreshSceneResource(scene_resource, main_scene);
}
