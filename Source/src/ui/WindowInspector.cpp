#include "core/hepch.h"
#include "WindowInspector.h"

#include "modules/ModuleEditor.h"
#include "modules/ModuleScriptingSystem.h"
#include "scripting/Script.h"

Hachiko::WindowInspector::WindowInspector() 
    : Window("Inspector", true) 
{
}

Hachiko::WindowInspector::~WindowInspector() = default;

void Hachiko::WindowInspector::Update()
{
    ImGui::SetNextWindowDockID(App->editor->dock_right_id, ImGuiCond_FirstUseEver);
    if (ImGui::Begin(StringUtils::Concat(ICON_FA_EYE, " ", name).c_str(), &active))
    {
        DrawGameObject(App->editor->GetSelectedGameObject());
    }
    ImGui::End();
}

void Hachiko::WindowInspector::DrawGameObject(GameObject* game_object) const
{
    if (game_object == nullptr)
    {
        return;
    }

    std::string add_script_modal_name = "Add Script to " + 
        game_object->GetName();

    char game_object_name[50];
    strcpy_s(game_object_name, 50, game_object->GetName().c_str());
    const ImGuiInputTextFlags name_input_flags = ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue;
    if (ImGui::InputText("###", game_object_name, 50, name_input_flags))
    {
        game_object->name = game_object_name;
    }

    ImGui::SameLine();
    bool activate = game_object->IsActive();
    if (ImGui::Checkbox("Active", &activate))
    {
        game_object->SetActive(activate);
    }

    std::vector<Component*> game_object_components = game_object->GetComponents();
    for (auto it = game_object_components.begin(); it != game_object_components.end(); ++it)
    {
        (*it)->DrawGui();
    }

    ImGui::Separator();
    const float x = (ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(StringUtils::Concat(ICON_FA_PLUS, " ", "Add component").c_str()).x - ImGui::GetStyle().FramePadding.x * 2) * 0.5f;
    ImGui::SetCursorPosX(x);
    if (ImGui::Button(StringUtils::Concat(ICON_FA_PLUS, " ", "Add component").c_str()))
    {
        ImGui::OpenPopup("AddComponentPopup");
    }

    static bool add_script = false;

    if (ImGui::BeginPopup("AddComponentPopup"))
    {
        if (ImGui::MenuItem("Camera"))
        {
            game_object->CreateComponent(Component::Type::CAMERA);
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::MenuItem("Point Light"))
        {
            game_object->CreateComponent(Component::Type::POINTLIGHT);
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::MenuItem("Spot Light"))
        {
            game_object->CreateComponent(Component::Type::SPOTLIGHT);
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::MenuItem("Dir Light"))
        {
            game_object->CreateComponent(Component::Type::DIRLIGHT);
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::MenuItem("Script"))
        {
            ImGui::CloseCurrentPopup();
            add_script = true;
        }
           

        // TODO: Group these UI stuff in a parent UI MenuItem and select
        // those like that.

        if (ImGui::MenuItem("Canvas"))
        {
            game_object->CreateComponent(Component::Type::TRANSFORM_2D);
            game_object->CreateComponent(Component::Type::CANVAS);
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::MenuItem("Button"))
        {                
            game_object->CreateComponent(Component::Type::TRANSFORM_2D);
            game_object->CreateComponent(Component::Type::CANVAS_RENDERER);
            game_object->CreateComponent(Component::Type::BUTTON);
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::MenuItem("Image"))
        {
            game_object->CreateComponent(Component::Type::TRANSFORM_2D);
            game_object->CreateComponent(Component::Type::CANVAS_RENDERER);
            game_object->CreateComponent(Component::Type::IMAGE);
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::MenuItem("Progress Bar"))
        {
            game_object->CreateComponent(Component::Type::PROGRESS_BAR);
            game_object->CreateComponent(Component::Type::TRANSFORM_2D);
            GameObject* background = new GameObject(game_object, "Bar Background");
            background->CreateComponent(Component::Type::TRANSFORM_2D);
            background->CreateComponent(Component::Type::CANVAS_RENDERER);
            background->CreateComponent(Component::Type::IMAGE);
            GameObject* fill = new GameObject(game_object, "Bar Fill");
            fill->CreateComponent(Component::Type::TRANSFORM_2D);
            fill->CreateComponent(Component::Type::CANVAS_RENDERER);
            fill->CreateComponent(Component::Type::IMAGE);
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::MenuItem("Text Label"))
        {
            game_object->CreateComponent(Component::Type::TRANSFORM_2D);
            game_object->CreateComponent(Component::Type::CANVAS_RENDERER);
            game_object->CreateComponent(Component::Type::TEXT);
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    // Open Add Script Modal.

    if (add_script)
    {
        ImGui::OpenPopup(add_script_modal_name.c_str());
    }

    ImGui::SetNextWindowSize(ImVec2(300, 100));
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, 
        ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal(add_script_modal_name.c_str(), 
        NULL, ImGuiWindowFlags_NoCollapse))
    {
        char script_name_buffer[MAX_PATH] = "Script Name\0";

        ImGui::SetItemDefaultFocus();
        if (ImGui::InputText("Script Name", script_name_buffer, 
            MAX_PATH, ImGuiInputTextFlags_EnterReturnsTrue))
        {
            std::string script_name = script_name_buffer;
            Scripting::Script* script = App->scripting_system->
                InstantiateScript(script_name, game_object);

            if (script != nullptr)
            {
                game_object->AddComponent(script);
            }
               
            ImGui::CloseCurrentPopup();
            add_script = false;
        }

        if (ImGui::Button("Close", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
            add_script = false;
        }

        ImGui::EndPopup();
    }
}
