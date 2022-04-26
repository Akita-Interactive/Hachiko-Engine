#include "core/hepch.h"
#include "WindowConfiguration.h"

#include "modules/ModuleEditor.h"
#include "modules/ModuleProgram.h"
#include "modules/ModuleCamera.h"
#include "modules/ModuleWindow.h"
#include "modules/ModuleSceneManager.h"

#include "components/ComponentCamera.h"
#include "core/preferences/src/EditorPreferences.h"

Hachiko::WindowConfiguration::WindowConfiguration() :
    Window("Configuration", true)
{
}

void Hachiko::WindowConfiguration::Update()
{
    ImGui::SetNextWindowDockID(App->editor->dock_right_id, ImGuiCond_FirstUseEver);
    if (ImGui::Begin((std::string(ICON_FA_COG " ") + name).c_str(), &active))
    {
        if (ImGui::CollapsingHeader("Scene"))
        {
            App->renderer->OptionsMenu();
            ImGui::Separator();
            App->scene_manager->OptionsMenu();
            ImGui::Separator();
            ImGui::Text("Shader Options");
            App->program->OptionsMenu();
        }

        //It is already a collapsing header
        App->camera->GetMainCamera()->DrawGui();

        if (ImGui::CollapsingHeader("Engine"))
        {
            App->window->OptionsMenu();
            App->renderer->PerformanceMenu();
        }
    }
    ImGui::End();
}
