#include "core/hepch.h"
#include "WindowConfiguration.h"

#include "modules/ModuleEditor.h"
#include "modules/ModuleProgram.h"
#include "modules/ModuleCamera.h"
#include "modules/ModuleWindow.h"
#include "modules/ModuleSceneManager.h"
#include "modules/ModuleAudio.h"
#include "batching/BatchManager.h"

#include "components/ComponentCamera.h"
#include "core/preferences/src/EditorPreferences.h"

Hachiko::WindowConfiguration::WindowConfiguration() :
    Window("Configuration", true)
{
}

void Hachiko::WindowConfiguration::Update()
{
    ImGui::SetNextWindowDockID(App->editor->dock_right_id, ImGuiCond_FirstUseEver);
    if (!ImGui::Begin((std::string(ICON_FA_GEAR " ") + name).c_str(), &active, ImGuiWindowFlags_NoNavInputs))
    {
        ImGui::End();
        return;
    }
    if (ImGui::CollapsingHeader("Scene"))
    {
        App->scene_manager->OptionsMenu();
        ImGui::Separator();
        App->renderer->OptionsMenu();
        ImGui::Separator();
        ImGui::Text("Shader Options");
        App->program->OptionsMenu();
    }
    
    if (ImGui::CollapsingHeader("Audio"))
    {
        App->audio->OptionsMenu();
    }
    
    //It is already a collapsing header
    App->camera->GetRenderingCamera()->DrawGui();

    if (ImGui::CollapsingHeader("Engine"))
    {
        ImGui::Checkbox("Undo/Redo", &App->editor->undo_redo_active);
        App->window->OptionsMenu();
        App->renderer->PerformanceMenu();
    }

    if (ImGui::CollapsingHeader("Batching"))
    {
        App->scene_manager->GetActiveScene()->GetBatchManager()->DebugMenu();
    }
    ImGui::End();
}
