#include "core/hepch.h"
#include "WindowConfiguration.h"

#include "modules/ModuleEditor.h"
#include "modules/ModuleCamera.h"
#include "modules/ModuleWindow.h"
#include "modules/ModuleSceneManager.h"
#include "modules/ModuleAudio.h"
#include "modules/ModuleNavigation.h"
#include "batching/BatchManager.h"

#include "components/ComponentCamera.h"

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
    }
    if (ImGui::CollapsingHeader("Navigation"))
    {
        App->navigation->DrawOptionsGui();
    }
    if (ImGui::CollapsingHeader("Render"))
    {
        App->renderer->OptionsMenu();
    }
    
    if (ImGui::CollapsingHeader("Audio"))
    {
        App->audio->OptionsMenu();
    }
    
    //It is already a collapsing header
    App->camera->GetRenderingCamera()->DrawGui();

    if (ImGui::CollapsingHeader("Engine"))
    {
        Widgets::Checkbox("Undo/Redo", &App->editor->undo_redo_active);
        App->window->OptionsMenu();
        App->renderer->PerformanceMenu();
        App->renderer->LoadingScreenOptions();
    }

    if (ImGui::CollapsingHeader("Batching"))
    {
        App->scene_manager->GetActiveScene()->GetBatchManager()->DebugMenu();
    }
    ImGui::End();
}
