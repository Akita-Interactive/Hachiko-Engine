#include "core/hepch.h"
#include "ModuleSceneManager.h"

#include "ModuleCamera.h"
#include "ModuleEvent.h"

#include "core/preferences/src/ResourcesPreferences.h"
#include "core/preferences/src/EditorPreferences.h"

bool Hachiko::ModuleSceneManager::Init()
{ 
    serializer = new SceneSerializer();
    preferences = App->preferences->GetResourcesPreference();
    std::string scene_path = StringUtils::Concat(preferences->GetAssetsPath(Resource::Type::SCENE), preferences->GetSceneName());
    
    if (std::filesystem::exists(scene_path))
    {
        LoadScene(scene_path.c_str());
    }
    else
    {
        CreateEmptyScene();
    }

#ifdef PLAY_BUILD
    App->camera->ReturnPlayerCamera();
    main_scene->Start();
#endif

    EditorPreferences* pref = App->preferences->GetEditorPreference();
    scene_autosave = pref->GetAutosave();
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

    EditorPreferences* pref = App->preferences->GetEditorPreference();
    pref->SetAutosave(scene_autosave);
    
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

    scene_load.SetEventData<SceneLoadEventPayload>(
        SceneLoadEventPayload::State::LOADED);
    App->event->Publish(scene_load);
}

void Hachiko::ModuleSceneManager::LoadScene(const char* file_path)
{
    Event scene_load(Event::Type::SCENE_LOADED);

    scene_load.SetEventData<SceneLoadEventPayload>(
        SceneLoadEventPayload::State::NOT_LOADED);
    App->event->Publish(scene_load);

    delete main_scene;
    main_scene = serializer->Load(file_path);

    scene_load.SetEventData<SceneLoadEventPayload>(SceneLoadEventPayload::State::LOADED);
    App->event->Publish(scene_load);
    
#ifdef PLAY_BUILD
    App->camera->ReturnPlayerCamera();
    main_scene->Start();
#endif
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
    serializer->Save(main_scene, file_path); // TODO: Take into account temporal scenes
}

Hachiko::GameObject* Hachiko::ModuleSceneManager::Raycast(const float3& origin, const float3& destination)
{
    return main_scene->Raycast(origin, destination);
}

void Hachiko::ModuleSceneManager::SwitchTo(const char* file_path)
{
    scene_ready_to_load = true;
    scene_to_load = file_path;
}

void Hachiko::ModuleSceneManager::OptionsMenu()
{
    ImGui::Checkbox("Autosave Scene", &scene_autosave);
}
