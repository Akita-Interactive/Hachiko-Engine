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

#include <iostream>
#include <iomanip>
#include <ctime>

bool Hachiko::ModuleSceneManager::Init()
{
    HE_LOG("INITIALIZING MODULE: SCENE MANAGER");

    preferences = App->preferences->GetResourcesPreference();

    // If uid is not found it will load an empty scene
    LoadScene(preferences->GetSceneUID());

    std::function handleSceneSwapping = [&](Event& evt) {
        auto scene = evt.GetEventData<EditorHistoryEntryRestore>().GetScene();
        ChangeMainScene(scene);
    };
    App->event->Subscribe(Event::Type::RESTORE_EDITOR_HISTORY_ENTRY, handleSceneSwapping);

#ifdef PLAY_BUILD
    main_scene->Start();
#else
    EditorPreferences* editor_prefs = App->preferences->GetEditorPreference();
    scene_autosave = editor_prefs->GetAutosave();
#endif
    return true;
}

void Hachiko::ModuleSceneManager::AttemptScenePause()
{
    if (!GameTimer::paused)
    {
        Event game_state(Event::Type::GAME_STATE);
        game_state.SetEventData<GameStateEventPayload>(GameStateEventPayload::State::PAUSED);
        App->event->Publish(game_state);

        GameTimer::Pause();
    }
}

void Hachiko::ModuleSceneManager::AttemptScenePlay()
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

    if (!GameTimer::running)
    {
        RefreshSceneResource();

        Event game_state(Event::Type::GAME_STATE);
        game_state.SetEventData<GameStateEventPayload>(GameStateEventPayload::State::STARTED);
        App->event->Publish(game_state);
        main_scene->Start();

        GameTimer::Start();
    }
    else if (GameTimer::paused)
    {
        Event game_state(Event::Type::GAME_STATE);
        game_state.SetEventData<GameStateEventPayload>(GameStateEventPayload::State::RESUMED);
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

UpdateStatus Hachiko::ModuleSceneManager::Update(const float delta)
{
    main_scene->Update();
    return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus Hachiko::ModuleSceneManager::PostUpdate(float delta)
{
    if (scene_reload_requested)
    {
        // Use the scene resource refreshed when play is pressed to restore og state
        // Basically scene serialized in memory
        // Keeps the currently defined navmesh since the one saved in library is from the last time we saved
        scene_reload_requested = false;
        constexpr bool keep_navmesh = true;
        LoadScene(scene_resource, keep_navmesh);
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
        for (const GameObject* go : to_remove)
        {
            if (App->editor->GetSelectedGameObject() == go)
            {
                App->editor->SetSelectedGO(nullptr);
            }
            delete go;
        }
        to_remove.clear();
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
    // If it was a temporary scene it will set id to 0 which will generate a new temporary scene on load
    preferences->SetSceneUID(scene_resource->GetID());
    preferences->SetSceneName(scene_resource->name.c_str());

    SetSceneResource(nullptr);

    RELEASE(main_scene);
    // Release because not owned by RM

    return true;
}

void Hachiko::ModuleSceneManager::RemoveGameObject(GameObject* go)
{
    if (go)
    {
        if (!go->scene_owner)
        {
            delete go;
            return;
        }
        to_remove.push_back(go);
    }
}

void Hachiko::ModuleSceneManager::CreateEmptyScene(const char* name)
{
    LoadScene(nullptr);
    if (name)
    {
        main_scene->name = name;
    }
    else
    {
        // Default name with timestamp for new empt unnamed scenes
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        std::ostringstream oss;
        oss << "unnamed_scene_" << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
        main_scene->name = oss.str().c_str();
    }

    // Since the empty scene is already loaded, it's loaded flag must be set to
    // true, for systems that need a "loaded" scene to function such as
    // ModuleScriptingSystem which needs both engine to be in play mode and
    // current scene to be flagged as "loaded":
    main_scene->loaded = true;

    // Create a scene resource not related to an asset to reload the scene as any other
    SceneImporter scene_importer;
    SetSceneResource(scene_importer.CreateSceneResource(main_scene));
}

void Hachiko::ModuleSceneManager::StopScene()
{
    Event game_state(Event::Type::GAME_STATE);
    game_state.SetEventData<GameStateEventPayload>(GameStateEventPayload::State::STOPPED);
    App->event->Publish(game_state);

    App->camera->SetRenderingCamera(App->camera->GetEditorCamera());
    main_scene->SetCullingCamera(App->camera->GetEditorCamera());
    main_scene->Stop();
    GameTimer::Stop();
}

void Hachiko::ModuleSceneManager::LoadScene(UID new_scene_id)
{
    ResourceScene* scene_resource = static_cast<ResourceScene*>(App->resources->GetResource(Resource::Type::SCENE, new_scene_id));
    if (scene_resource)
    {
        LoadScene(scene_resource);
    }
    else
    {
        CreateEmptyScene();
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

Hachiko::GameObject* Hachiko::ModuleSceneManager::Raycast(const float3& origin, const float3& destination)
{
    return main_scene->Raycast(origin, destination);
}

Hachiko::GameObject* Hachiko::ModuleSceneManager::BoundingRaycast(const float3& origin, const float3& destination)
{
    return main_scene->Raycast(origin, destination);
}

void Hachiko::ModuleSceneManager::ChangeSceneById(UID new_scene_id, bool stop_scene)
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
    char scene_name[50];
    strcpy_s(scene_name, 50, main_scene->GetName());
    const ImGuiInputTextFlags name_input_flags = ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue;
    if (ImGui::InputText("###", scene_name, 50, name_input_flags))
    {
        main_scene->SetName(scene_name);
    }
    ImGui::Checkbox("Autosave Scene", &scene_autosave);
    App->navigation->DrawOptionsGui();

    // Skybox
    ImGui::Separator();
    main_scene->GetSkybox()->DrawImGui();
}

void Hachiko::ModuleSceneManager::LoadScene(ResourceScene* new_resource, bool keep_navmesh)
{
    SetSceneResource(new_resource);

    Scene* new_scene = new Scene();
    if (scene_resource)
    {
        if (!keep_navmesh)
        {
            App->navigation->SetNavmesh(scene_resource->scene_data[NAVMESH_ID].as<UID>());
        }
        new_scene->Load(scene_resource->scene_data);
    }
    else
    {
        if (!keep_navmesh)
        {
            App->navigation->SetNavmesh(nullptr);
        }
        // This removes current navmesh
        GameObject* camera_go = new_scene->CreateNewGameObject(new_scene->GetRoot(), "Main Camera");
        camera_go->CreateComponent(Component::Type::CAMERA);
    }

    ChangeMainScene(new_scene);
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

    // TODO: If we make empty scenes have a game object with a camera component
    // attached by default, add the following lines to CreateEmptyScene as well
#ifdef PLAY_BUILD
    main_scene->Start();
    ComponentCamera* main_camera = main_scene->GetMainCamera();
    App->camera->SetRenderingCamera(main_camera);
    main_scene->SetCullingCamera(main_camera);
#else
    if (IsScenePlaying())
    {
        App->camera->SetRenderingCamera(main_scene->GetMainCamera());
        main_scene->SetCullingCamera(main_scene->GetMainCamera());
    }
#endif // PLAY_MODE
}

void Hachiko::ModuleSceneManager::SetSceneResource(ResourceScene* new_scene_resource)
{
    if (scene_resource != new_scene_resource)
    {
        // Only delete if it exists, its not the same resource and its not loaded by resource manager (id 0)
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

void Hachiko::ModuleSceneManager::RefreshSceneResource()
{
    SceneImporter scene_importer;
    scene_importer.RefreshSceneResource(scene_resource, main_scene);
}
