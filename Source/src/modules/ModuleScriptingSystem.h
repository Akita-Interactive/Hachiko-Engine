#pragma once
#include "Module.h"

namespace Hachiko
{
class GameObject;

namespace Scripting
{
class Script;
typedef Script*(__cdecl* ScriptFactory)(GameObject*, const std::string&);
}

class ModuleScriptingSystem : public Module
{
public:
    ModuleScriptingSystem();
    ~ModuleScriptingSystem() override = default;

    bool Init() override;
    bool CleanUp() override;

    UpdateStatus PreUpdate(const float delta) override;
    UpdateStatus Update(const float delta) override;
    UpdateStatus PostUpdate(const float delta) override;

    Scripting::Script* InstantiateScript(const std::string& script_name,
        GameObject* owner_game_object) const;

    bool ShouldExecuteScripts() const;
    void StopExecutingScripts();

private:
    void SubscribeToEvents();
    bool LoadFirstTime();
    bool HotReload(const float delta);
    bool ShouldCheckForChanges(const float delta);
    void LoadDll(HMODULE* dll);
    void FreeDll(HMODULE dll, unsigned int load_index);
    void DeleteAllScriptsOnCurrentScene() const; 
    void AwakeAllScriptsOnCurrentScene() const; 
    bool IsDllVersionChanged();

private:
    HMODULE _loaded_dll;
    Scripting::ScriptFactory _script_factory;
    unsigned int _times_reloaded;
    std::string _current_dll_timestamp;
    std::wstring _current_dll_name;
    
    // TODO: Implement a timer class that basically has these two values
    // and has a method Tick, that takes delta and decreases the timer and
    // returns true if the timer is finished.
    float _dll_change_check_frequency_in_secs;
    float _dll_change_check_timer;
    
    bool _scripts_paused;
    bool _in_play_mode;
    bool _waiting_for_scene_load;
};
} // namespace Hachiko