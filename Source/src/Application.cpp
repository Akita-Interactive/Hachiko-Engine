#include "core/hepch.h"

#include "Application.h"

#include "modules/ModuleWindow.h"
#include "modules/ModuleRender.h"
#include "modules/ModuleInput.h"
#include "modules/ModuleEditor.h"
#include "modules/ModuleCamera.h"
#include "modules/ModuleTexture.h"
#include "modules/ModuleProgram.h"
#include "modules/ModuleSceneManager.h"
#include "modules/ModuleDebugDraw.h"
#include "modules/ModuleEvent.h"
#include "modules/ModuleScriptingSystem.h"
#include "modules/ModuleResources.h"
#include "modules/ModuleUserInterface.h"
#include "modules/ModuleDebugMode.h"
#include "modules/ModuleAudio.h"
#include "modules/ModuleNavigation.h"

#include "core/preferences/PreferenceManager.h"


Hachiko::Application::Application()
{
    modules.push_back(window = new ModuleWindow());
    modules.push_back(input = new ModuleInput());
    modules.push_back(scripting_system = new ModuleScriptingSystem());
    modules.push_back(texture = new ModuleTexture());
    modules.push_back(renderer = new ModuleRender());
    modules.push_back(camera = new ModuleCamera());
    modules.push_back(resources = new ModuleResources());
    modules.push_back(audio = new ModuleAudio());
    modules.push_back(navigation = new ModuleNavigation());
    modules.push_back(program = new ModuleProgram());
    modules.push_back(scene_manager = new ModuleSceneManager()); 
    modules.push_back(debug_draw = new ModuleDebugDraw());
    modules.push_back(editor = new ModuleEditor());
    modules.push_back(event = new ModuleEvent());
    modules.push_back(ui = new ModuleUserInterface()); 
    modules.push_back(debug_mode = new ModuleDebugMode());
    preferences = new PreferenceManager(SETTINGS_FILE_PATH);

}

Hachiko::Application::~Application()
{
    for (auto it = modules.begin(); it != modules.end(); ++it)
    {
        delete *it;
    }

    delete preferences;
}

bool Hachiko::Application::Init()
{
    bool ret = true;

    ret &= preferences->Init();

    file_system.Init();

    // Execute Init:
    for (auto it = modules.begin(); it != modules.end() && ret; ++it)
    {
        ret = (*it)->Init();
    }

    delta = 0;
    EngineTimer::Start();

    // GameTimer is triggered when the scene is loaded on PLAY_BUILD or when
    // Play button on engine was hit. Therefore it won't be started here.

    // Execute Start:
    for (auto it = modules.begin(); it != modules.end() && ret; ++it)
    {
        ret = (*it)->Start();
    }

    return ReturnStatusWithQuit(ret);
}

UpdateStatus Hachiko::Application::Update()
{
    OPTICK_EVENT();
    delta = EngineTimer::Update();
    GameTimer::Update();

    auto ret = UpdateStatus::UPDATE_CONTINUE;

    for (auto it = modules.begin(); it != modules.end() && ret == UpdateStatus::UPDATE_CONTINUE; ++it)
    {
        ret = (*it)->PreUpdate(static_cast<float>(delta));
    }

    for (auto it = modules.begin(); it != modules.end() && ret == UpdateStatus::UPDATE_CONTINUE; ++it)
    {
        ret = (*it)->Update(static_cast<float>(delta));
    }

    for (auto it = modules.begin(); it != modules.end() && ret == UpdateStatus::UPDATE_CONTINUE; ++it)
    {
        ret = (*it)->PostUpdate(static_cast<float>(delta));
    }

    return ReturnStatusWithQuit(ret);
}

void Hachiko::Application::MarkAsQuitting(const bool value)
{
    should_quit = value;
}

bool Hachiko::Application::CleanUp()
{
    bool ret = true;

    for (auto it = modules.rbegin(); it != modules.rend() && ret; ++it)
    {
        ret = (*it)->CleanUp();
    }

    preferences->SaveConfigurationFile();

    return ret;
}

void Hachiko::Application::RequestBrowser(const char* url)
{
    ShellExecuteA(nullptr, "open", url, nullptr, nullptr, SW_SHOWNORMAL);
}

UpdateStatus Hachiko::Application::ReturnStatusWithQuit(UpdateStatus status) const
{
    return should_quit ? UpdateStatus::UPDATE_STOP : status;
}

bool Hachiko::Application::ReturnStatusWithQuit(bool status) const
{
    return !should_quit && status;
}