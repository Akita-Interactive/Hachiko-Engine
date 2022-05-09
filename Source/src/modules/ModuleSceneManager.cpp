#include "core/hepch.h"
#include "ModuleSceneManager.h"

#include "ModuleCamera.h"
#include "ModuleEvent.h"
#include "ModuleNavigation.h"

#include "core/preferences/src/ResourcesPreferences.h"
#include "core/preferences/src/EditorPreferences.h"

bool Hachiko::ModuleSceneManager::Init()
{
    serializer = new SceneSerializer();
    preferences = App->preferences->GetResourcesPreference();
    std::string scene_path = StringUtils::Concat(preferences->GetAssetsPath(Resource::AssetType::SCENE), preferences->GetSceneName());

    if (std::filesystem::exists(scene_path))
    {
        LoadScene(scene_path.c_str());
    }
    else
    {
        CreateEmptyScene();
    }
    

    std::function handleSceneSwapping = [&](Event& evt)
    {
        auto scene = evt.GetEventData<EditorHistoryEntryRestore>().GetScene();
        SwapScene(scene);
    };
    App->event->Subscribe(Event::Type::RESTORE_EDITOR_HISTORY_ENTRY, handleSceneSwapping);

#ifdef PLAY_BUILD
    main_scene->Start();
#endif

    EditorPreferences* editor_prefs = App->preferences->GetEditorPreference();
    scene_autosave = editor_prefs->GetAutosave();
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
        Event game_state(Event::Type::GAME_STATE);
        game_state.SetEventData<GameStateEventPayload>(GameStateEventPayload::State::STARTED);
        App->event->Publish(game_state);

        SaveScene(StringUtils::Concat(preferences->GetLibraryPath(Resource::Type::SCENE), SCENE_TEMP_NAME, SCENE_EXTENSION).c_str());

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
        Event game_state(Event::Type::GAME_STATE);
        game_state.SetEventData<GameStateEventPayload>(GameStateEventPayload::State::STOPPED);
        App->event->Publish(game_state);

        main_scene->SetCullingCamera(App->camera->GetEditorCamera());
        App->camera->SetRenderingCamera(App->camera->GetEditorCamera());

        GameTimer::Stop();

        LoadScene(StringUtils::Concat(preferences->GetLibraryPath(Resource::Type::SCENE), SCENE_TEMP_NAME, SCENE_EXTENSION).c_str());
    }
}

bool Hachiko::ModuleSceneManager::IsScenePlaying()
{
    return GameTimer::running && !GameTimer::paused;
}

UpdateStatus Hachiko::ModuleSceneManager::Update(const float delta)
{
    if (scene_ready_to_load)
    {
        scene_ready_to_load = false;
        LoadScene(scene_to_load.c_str());
        currentScenePath = scene_to_load;
    }

    main_scene->Update();
    return UpdateStatus::UPDATE_CONTINUE;
}

bool Hachiko::ModuleSceneManager::CleanUp()
{
    if (scene_autosave)
    {
        SaveScene();
    }

    EditorPreferences* editor_prefs = App->preferences->GetEditorPreference();
    editor_prefs->SetAutosave(scene_autosave);
    preferences->SetSceneName(main_scene->GetName());

    // TODO: Remove temp_scene.scene from disk
    RELEASE(main_scene);
    RELEASE(serializer);
    return true;
}

void Hachiko::ModuleSceneManager::CreateEmptyScene()
{
    Event scene_load(Event::Type::SCENE_LOADED);

    scene_load.SetEventData<SceneLoadEventPayload>(
        SceneLoadEventPayload::State::NOT_LOADED);
    App->event->Publish(scene_load);

    delete main_scene;
    main_scene = new Scene();
    // Since the empty scene is already loaded, it's loaded flag must be set to
    // true, for systems that need a "loaded" scene to function such as
    // ModuleScriptingSystem which needs both engine to be in play mode and 
    // current scene to be flagged as "loaded":
    main_scene->loaded = true;

    scene_load.SetEventData<SceneLoadEventPayload>(
        SceneLoadEventPayload::State::LOADED);
    App->event->Publish(scene_load);

    currentScenePath = "";
}

void Hachiko::ModuleSceneManager::LoadScene(const char* file_path)
{
    Scene* scene = serializer->Load(file_path);
    currentScenePath = file_path;
    SwapScene(scene);
}

void Hachiko::ModuleSceneManager::SaveScene()
{
    if (IsScenePlaying())
    {
        LoadScene(StringUtils::Concat(preferences->GetLibraryPath(Resource::Type::SCENE), SCENE_TEMP_NAME, SCENE_EXTENSION).c_str());
    }

    serializer->Save(main_scene);
}

void Hachiko::ModuleSceneManager::SaveScene(const char* file_path)
{
    serializer->Save(main_scene, file_path);
}

Hachiko::GameObject* Hachiko::ModuleSceneManager::Raycast(const float3& origin, const float3& destination)
{
    return main_scene->Raycast(origin, destination);
}

void Hachiko::ModuleSceneManager::SwitchTo(const char* file_path)
{
    scene_ready_to_load = true;
    scene_to_load = file_path;
    currentScenePath = file_path;
}

void Hachiko::ModuleSceneManager::ReloadScene()
{
    if (std::filesystem::exists(currentScenePath))
    {
        LoadScene(currentScenePath.c_str());
    }
    else
    {
        CreateEmptyScene();
    }
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
}

void Hachiko::ModuleSceneManager::SwapScene(Scene* scene)
{
    Event scene_load(Event::Type::SCENE_LOADED);

    scene_load.SetEventData<SceneLoadEventPayload>(SceneLoadEventPayload::State::NOT_LOADED);
    App->event->Publish(scene_load);

    delete main_scene;
    main_scene = scene;

    scene_load.SetEventData<SceneLoadEventPayload>(SceneLoadEventPayload::State::LOADED);
    App->event->Publish(scene_load);

    // TODO: If we make empty scenes have a game object with a camera component
    // attached by default, add the following lines to CreateEmptyScene as well
#ifdef PLAY_BUILD
    main_scene->Start();
    App->camera->SetRenderingCamera(main_scene->GetMainCamera());
    main_scene->SetCullingCamera(main_scene->GetMainCamera());
#else
    if (IsScenePlaying())
    {
        App->camera->SetRenderingCamera(main_scene->GetMainCamera());
        main_scene->SetCullingCamera(main_scene->GetMainCamera());
    }
#endif // PLAY_MODE
}
