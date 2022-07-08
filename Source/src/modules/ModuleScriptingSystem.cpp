#include "core/hepch.h"
#include "ModuleScriptingSystem.h"
#include "utils/WindowsFileUtil.h"
#include "core/GameObject.h"
#include "scripting/Script.h"
#include "modules/ModuleSceneManager.h"
#include "modules/ModuleEvent.h"
#include "Application.h"

constexpr const wchar_t* SCRIPTING_DLL_NAME = L"Gameplay";

Hachiko::ModuleScriptingSystem::ModuleScriptingSystem() 
    : _times_reloaded(0) 
    , _loaded_dll(NULL) 
    , _current_dll_timestamp("") 
    , _current_dll_name(L"")
    , _available_script_count(0)
    , _script_names({})
    , _dll_change_check_timer(0.0f)
    , _dll_change_check_frequency_in_secs(0.0f)
    , _script_factory(nullptr)
    , _scripts_paused(false)
    , _in_play_mode(false)
    , _waiting_for_scene_load(true)
{
}

bool Hachiko::ModuleScriptingSystem::Init()
{
    HE_LOG("INITIALIZING MODULE: SCRIPTING SYSTEM");

    _dll_change_check_frequency_in_secs = 2.0f;
    _dll_change_check_timer = 0.0f;
    _in_play_mode = false;
    _waiting_for_scene_load = true;

    bool loading_successful = LoadFirstTime();

    if (!loading_successful)
    {
        // NOTE: If this happens to be true, i.e someone does not have Gameplay
        // built before running the engine, the scene will load without scripts
        // and therefore, after a new dll is built and the Hachiko detects it,
        // since there are no scripts on the scene, this module will not try to
        // re-create the scripts. To solve this issue we may have a auxiliary
        // save file that has gameobject uids and scripts that are attached to
        // them, and the serialized fields of those scripts. When we try to 
        // load the scripts for the first time and there is no Gameplay.dll,
        // We save all the scripts on the scene to that file, and hold it until
        // we have a built Gameplay.dll. If we don't someone may save the scene
        // without the scripts and lose all the attached scripts. On another 
        // note though, with the new flow of scripting layer building, this 
        // issue won't happen as we will always compile the "Gameplay" when we
        // first run the engine.
        // TODO: Handle this.

        HE_LOG("[IMPORTANT] There are no Gameplay.dll detected by Hachiko " 
            "Continuing without scripting functionality.");
    }

    // Subscribe to events that may effect script updating behaviour:
    SubscribeToEvents();

    #ifdef PLAY_BUILD
    _in_play_mode = true;
    if (!_waiting_for_scene_load)
    {
        AwakeAllScriptsOnCurrentScene();
    }
    #endif

    return true;
}

bool Hachiko::ModuleScriptingSystem::CleanUp()
{
    if (_loaded_dll != NULL)
    {
        FreeDll(_loaded_dll, _times_reloaded);
    }

    return true;
}

UpdateStatus Hachiko::ModuleScriptingSystem::PreUpdate(const float delta)
{
    HotReload(delta);

    return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus Hachiko::ModuleScriptingSystem::Update(const float delta)
{
    return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus Hachiko::ModuleScriptingSystem::PostUpdate(const float delta)
{
    return UpdateStatus::UPDATE_CONTINUE;
}

Hachiko::Scripting::Script* Hachiko::ModuleScriptingSystem::InstantiateScript(
    const std::string& script_name, GameObject* owner_game_object) const
{
    if (_script_factory == nullptr)
    {
        return nullptr;
    }

    Scripting::Script* script = 
        _script_factory(owner_game_object, script_name);

    if (!_waiting_for_scene_load && _in_play_mode && script != nullptr 
        && !_scripts_paused)
    {
        script->Awake();

        if (owner_game_object->IsActive())
        {
            script->Start();
        }
    }

    return script;
}

bool Hachiko::ModuleScriptingSystem::HotReload(const float delta) 
{
    if (!ShouldCheckForChanges(delta))
    {
        return false;
    }

    if (!IsDllVersionChanged())
    {
        return false;
    }

    // Pause script updates:
    _scripts_paused = true;

    HMODULE new_dll = NULL;
    LoadDll(&new_dll);

    if (new_dll == NULL)
    {
        if (_loaded_dll != NULL)
        {
            HE_LOG("New loaded dll is NULL. Hot reloading is cancelled.");

            FreeDll(new_dll, _times_reloaded);
            _times_reloaded--;
        }
        else
        {
            HE_LOG("This code should never run.");
        }
        
        return false;
    }

    HE_LOG("Loaded Gameplay Scripting dll (%u).", _times_reloaded);

    Scripting::ScriptFactory new_factory = 
        reinterpret_cast<Scripting::ScriptFactory>(
            GetProcAddress(new_dll, "InstantiateScript"));

    LoadScriptNamesAndCount();

    // Get all the scripts inside the current scene:
    std::vector<Component*>& scripts = App->scene_manager->GetRoot()->
        GetComponentsInDescendants(Component::Type::SCRIPT);

    std::vector<Scripting::Script*> new_scripts;
    new_scripts.reserve(scripts.size());
    
    std::unordered_map<std::string, Scripting::SerializedField> serialization;    

    for (Component* component : scripts)
    {
        // Get current as script:
        Scripting::Script* script = static_cast<Scripting::Script*>(component);
        GameObject* game_object = script->GetGameObject();

        HE_LOG("Reloading %s of %s with id %llu", script->GetName().c_str(),
            game_object->name.c_str(), game_object->GetID());
        
        // Serialize script into the map:
        script->SerializeTo(serialization);
        HE_LOG("\tSerialized old version.");

        // Create new version of the script of same type:
        Scripting::Script* new_script = new_factory(game_object, 
            script->GetName().c_str());
        HE_LOG("\tCreated the new version.");

        // Remove the old script from it's owner:
        game_object->ForceRemoveComponent(component);
        
        HE_LOG("\tRemoved the old version from components list.");

        if (new_script != nullptr)
        {
            // Deserialize old script to new script:
            new_script->DeserializeFrom(serialization);
            HE_LOG("\tDeserialized the old version to the new version.");

            // Add script to the game_object:
            game_object->AddComponent(new_script);
            HE_LOG("\tAdded the new version to components list.");

            new_scripts.push_back(new_script);
        }
        else
        {
            HE_LOG("\tScript '%s' could not be found in new dll.", 
                script->GetName().c_str());
        }

        // Delete the old script:
        delete script;
        script = nullptr;
        HE_LOG("\tDeleted the old script.");

        // Clear the serialization map:
        serialization.clear();
    }

    FreeDll(_loaded_dll, _times_reloaded - 1);

    _loaded_dll = new_dll;
    
    _script_factory = new_factory;

    // Unpause script updates:
    _scripts_paused = false;

    if (ShouldExecuteScripts())
    {
        for (Scripting::Script* script : new_scripts)
        {
            script->Awake();
        }
    }

    return true;
}

bool Hachiko::ModuleScriptingSystem::ShouldExecuteScripts() const
{
    return !_scripts_paused && _in_play_mode && !_waiting_for_scene_load;
}

void Hachiko::ModuleScriptingSystem::StopExecutingScripts()
{
    _scripts_paused = true;
}

const std::vector<std::string>& Hachiko::ModuleScriptingSystem::GetLoadedScriptNames() const
{
    return _script_names;
}

size_t Hachiko::ModuleScriptingSystem::GetLoadedScriptCount() const
{
    return _available_script_count;
}

void Hachiko::ModuleScriptingSystem::SubscribeToEvents() 
{
    std::function on_mode_changed = [&](Event& evt) 
    { 
        auto& event_data = evt.GetEventData<GameStateEventPayload>();
        _waiting_for_scene_load = 
            !App->scene_manager->GetActiveScene()->IsLoaded();

        switch (event_data.GetState())
        {
            case GameStateEventPayload::State::STOPPED:
            case GameStateEventPayload::State::PAUSED:
            {
                _in_play_mode = false;
            }
            break;

            case GameStateEventPayload::State::STARTED:
            {
                _in_play_mode = true;
                
                if (!_waiting_for_scene_load)
                {
                    AwakeAllScriptsOnCurrentScene();
                }
            }
            break;

            case GameStateEventPayload::State::RESUMED:
            {
                _in_play_mode = true;
            }
            break;
        }
    };

    std::function on_scene_loaded = [&](Event& evt) 
    { 
        auto& event_data = evt.GetEventData<SceneLoadEventPayload>();

        _waiting_for_scene_load = 
            event_data.GetState() == SceneLoadEventPayload::State::NOT_LOADED;
        
        if (!_waiting_for_scene_load)
        {
            ExecuteOnLoadForAllScripts();

            if (_in_play_mode)
            {
                AwakeAllScriptsOnCurrentScene();
            }
        }
    };

    App->event->Subscribe(Event::Type::GAME_STATE, on_mode_changed);
    App->event->Subscribe(Event::Type::SCENE_LOADED, on_scene_loaded);
}

bool Hachiko::ModuleScriptingSystem::LoadFirstTime()
{
     // Pause script updates:
    _scripts_paused = true;

    _loaded_dll = NULL;
    LoadDll(&_loaded_dll);

    if (_loaded_dll == NULL)
    {
        _times_reloaded--;
        return false;
    }

    // Build the name of the new file that's gonna hold the copy of new dll:
    std::wstring current_dll_path = App->file_system.GetWorkingDirectoryW();
    current_dll_path += L"\\";
    current_dll_path += SCRIPTING_DLL_NAME;
    current_dll_path += L".dll";

    GetFileLastWriteTimestamp(current_dll_path, _current_dll_timestamp);

    HE_LOG("Loaded Gameplay Scripting dll (%u).", _times_reloaded);

    _script_factory = reinterpret_cast<Scripting::ScriptFactory>(
            GetProcAddress(_loaded_dll, "InstantiateScript"));

    LoadScriptNamesAndCount();

    // Unpause script updates:
    _scripts_paused = false;
}

bool Hachiko::ModuleScriptingSystem::ShouldCheckForChanges(const float delta)
{
    if (_dll_change_check_timer >= _dll_change_check_frequency_in_secs)
    {
        _dll_change_check_timer = 0.0f;
        
        return true;
    }

    _dll_change_check_timer += delta;

    return false;
}

void Hachiko::ModuleScriptingSystem::LoadDll(HMODULE* dll)
{
    // Increment this to hold an instance count of the original dll so we 
    // delete the previous one when we load the new one:
    _times_reloaded++;

    // Open the dll we will copy from:
    std::ifstream source;
    
    // NOTE: The following todo will be fixed by new flow of the scripting
    // hot reloading implementation. Please ignore this ugly solution for now.
    // I couldn't solve this problem in a beautiful manner, for now I'm doing 
    // this to do other stuff. TODO: Find a robust solution to this.
    // Wait while the file is just created:
    int source_buffer_size = 0;
    bool source_open_failed = false;

    std::wstring main_dll_name = SCRIPTING_DLL_NAME;
    main_dll_name = main_dll_name + L".dll";

    while (source_buffer_size <= 0)
    {
        // Wait for 100ms to be less busy-waiting:
        Sleep(100);

        source.close();
        source.open(main_dll_name, std::ios::binary);

        if (source.fail())
        {
            source_open_failed = true;
            break;
        }

        source_buffer_size = source.rdbuf()->pubseekoff(0, source.end, 
            source.in);
    }

    if (!source_open_failed)
    {
        // Go back to initial position:
        source.rdbuf()->pubseekpos(0, source.in);

        // Build the name of the new file that's gonna hold the copy of new 
        // dll:
        std::wstringstream wss;
        wss << L"_" << _times_reloaded << L".dll";
        std::wstring current_dll_name = SCRIPTING_DLL_NAME + wss.str();

        // Open the dll we will use:
        std::ofstream destination(current_dll_name, std::ios::binary);

        // Copy the original dll to the one we will use:
        destination << source.rdbuf();

        // Close streams:
        source.close();
        destination.close();

        // Load the copied library:
        *dll = LoadLibraryW(current_dll_name.c_str());
        _current_dll_name = current_dll_name;
    }
}

void Hachiko::ModuleScriptingSystem::FreeDll(HMODULE dll, 
    unsigned int load_index) 
{
    // Form it's name using the same formula LoadDll uses:
    std::wstringstream wss;
    wss << L"_" << load_index << L".dll";
    std::wstring previous_dll_name = SCRIPTING_DLL_NAME + wss.str();

    // Free the dll, and delete it:
    FreeLibrary(dll);
    _wremove(previous_dll_name.c_str());
}

void Hachiko::ModuleScriptingSystem::DeleteAllScriptsOnCurrentScene() const 
{
    // Get all the scripts inside the current scene:
    std::vector<Component*>& scripts = App->scene_manager->GetRoot()->
        GetComponentsInDescendants(Component::Type::SCRIPT);

    // We are deleting the scripts here because, scripts belong to dll and must
    // be deleted/freed before freeing the dll:
    for (Component* script : scripts)
    {
        script->GetGameObject()->ForceRemoveComponent(script);
        
        delete script;
        
        script = nullptr;
    }

    scripts.clear();
}

void Hachiko::ModuleScriptingSystem::AwakeAllScriptsOnCurrentScene() const 
{
    // Get all the scripts inside the current scene:
    std::vector<Component*>& scripts = App->scene_manager->GetRoot()->
        GetComponentsInDescendants(Component::Type::SCRIPT);

    for (Component* script : scripts)
    {
        static_cast<Scripting::Script*>(script)->Awake();
    }

    for (Component* script : scripts)
    {
        if (script->GetGameObject()->IsActive())
        {
            static_cast<Scripting::Script*>(script)->Start();
        }
    }

    scripts.clear();
}

void Hachiko::ModuleScriptingSystem::ExecuteOnLoadForAllScripts() const 
{
    // Get all the scripts inside the current scene:
    std::vector<Component*>& scripts = App->scene_manager->GetRoot()->GetComponentsInDescendants(Component::Type::SCRIPT);

    for (Component* script : scripts)
    {
        static_cast<Scripting::Script*>(script)->OnLoad();
    }

    scripts.clear();
}

void Hachiko::ModuleScriptingSystem::LoadScriptNamesAndCount() 
{
    if (_loaded_dll == NULL)
    {
        return;
    }
       
    // Get the count of script classes that are available to be created:
    _available_script_count = *reinterpret_cast<size_t*>(
        GetProcAddress(_loaded_dll, "SCRIPT_COUNT"));

    // Reorganize the script names vector:
    _script_names.clear();
    _script_names.resize(_available_script_count);

    // Get the names of script classes that are available to be created:
    const char** script_names_dll = reinterpret_cast<const char**>(
        GetProcAddress(_loaded_dll, "SCRIPT_NAMES"));

    for (size_t i = 0; i < _available_script_count; ++i)
    {
        _script_names[i] = script_names_dll[i];
    }
}

bool Hachiko::ModuleScriptingSystem::IsDllVersionChanged()
{
    std::string new_timestamp = "";

    std::wstring current_dll_path = App->file_system.GetWorkingDirectoryW();
    current_dll_path += L"\\";
    current_dll_path += SCRIPTING_DLL_NAME;
    current_dll_path += L".dll";

    bool new_timestamp_loaded = GetFileLastWriteTimestamp(current_dll_path, 
        new_timestamp);

    if (!new_timestamp_loaded)
    {
        return false;
    }

    bool timestamp_changed = new_timestamp != _current_dll_timestamp;

    _current_dll_timestamp = new_timestamp;

    return timestamp_changed;
}