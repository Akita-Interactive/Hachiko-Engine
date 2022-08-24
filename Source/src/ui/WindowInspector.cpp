#include "core/hepch.h"
#include "WindowInspector.h"

#include "modules/ModuleEditor.h"
#include "modules/ModuleEvent.h"
#include "modules/ModuleScriptingSystem.h"
#include "scripting/Script.h"

Hachiko::WindowInspector::WindowInspector() :
    Window("Inspector", true)
{
}

Hachiko::WindowInspector::~WindowInspector() = default;

void Hachiko::WindowInspector::Update()
{
    ImGui::SetNextWindowDockID(App->editor->dock_right_id, ImGuiCond_FirstUseEver);
    if (!ImGui::Begin(StringUtils::Concat(ICON_FA_EYE, " ", name).c_str(), &active, ImGuiWindowFlags_NoNavInputs))
    {
        ImGui::End();
        return;
    }
    DrawGameObject(App->editor->GetSelectedGameObject());
    ImGui::End();
}

void Hachiko::WindowInspector::DrawGameObject(GameObject* game_object) const
{
    if (game_object == nullptr)
    {
        return;
    }

    std::string add_script_modal_name = "Add Script to " + game_object->GetName();

    char game_object_name[50];
    strcpy_s(game_object_name, 50, game_object->GetName().c_str());
    const ImGuiInputTextFlags name_input_flags = ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue;
    if (ImGui::InputText("###", game_object_name, 50, name_input_flags))
    {
        game_object->name = game_object_name;
        App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
    }

    ImGui::SameLine();
    bool activate = game_object->IsActive();
    if (ImGui::Checkbox("Active", &activate))
    {
        game_object->SetActive(activate);
        App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
    }
    
    std::vector<Component*> game_object_components = game_object->GetComponents();
    for (auto it = game_object_components.begin(); it != game_object_components.end(); ++it)
    {
        (*it)->DrawGui();
    }

    ImGui::Separator();
    if (ImGui::Button(StringUtils::Concat(ICON_FA_PLUS, " ", "Add component").c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
    {
        ImGui::OpenPopup("AddComponentPopup");
    }

    static bool add_script = false;

    const auto current_pos = ImGui::GetCurrentWindow()->DC.CursorPos;
    float pivot_y = 0.0f;
    float adjust_y = 0.0f;
    constexpr float default_popup_size_y = 250.0f;
    if (ImGui::GetCurrentWindow()->Size.y - current_pos.y < default_popup_size_y)
    {
        pivot_y = 1.0f;
        adjust_y = ImGui::GetCurrentWindow()->DC.PrevLineSize.y + ImGui::GetStyle().WindowPadding.y;
    }
    ImGui::SetNextWindowSize(ImVec2(ImGui::GetContentRegionAvail().x, default_popup_size_y));
    ImGui::SetNextWindowPos(ImVec2(current_pos.x, current_pos.y - adjust_y), 0, ImVec2(0, pivot_y));
    if (ImGui::BeginPopup("AddComponentPopup"))
    {
        if (ImGui::MenuItem("Camera"))
        {
            game_object->CreateComponent(Component::Type::CAMERA);
            ImGui::CloseCurrentPopup();
            App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
        }

        if (ImGui::MenuItem("Point light"))
        {
            game_object->CreateComponent(Component::Type::POINTLIGHT);
            ImGui::CloseCurrentPopup();
            App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
        }

        if (ImGui::MenuItem("Spotlight"))
        {
            game_object->CreateComponent(Component::Type::SPOTLIGHT);
            ImGui::CloseCurrentPopup();
            App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
        }

        if (ImGui::MenuItem("Directional Light"))
        {
            game_object->CreateComponent(Component::Type::DIRLIGHT);
            ImGui::CloseCurrentPopup();
            App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
        }

        if (ImGui::MenuItem("Script"))
        {
            ImGui::CloseCurrentPopup();
            add_script = true;
        }

        if (ImGui::MenuItem("Animation"))
        {
            game_object->CreateComponent(Component::Type::ANIMATION);
            ImGui::CloseCurrentPopup();
            App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
        }

        if (ImGui::MenuItem("Obstacle"))
        {
            game_object->CreateComponent(Component::Type::OBSTACLE);
            ImGui::CloseCurrentPopup();
            App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
        }

        if (ImGui::MenuItem("Agent"))
        {
            game_object->CreateComponent(Component::Type::AGENT);
            ImGui::CloseCurrentPopup();
            App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
        }

        // TODO: Group these UI stuff in a parent UI MenuItem and select
        // those like that.

        if (ImGui::MenuItem("Canvas"))
        {
            game_object->CreateComponent(Component::Type::TRANSFORM_2D);
            game_object->CreateComponent(Component::Type::CANVAS);
            ImGui::CloseCurrentPopup();
            App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
        }

        if (ImGui::MenuItem("Button"))
        {
            game_object->CreateComponent(Component::Type::TRANSFORM_2D);
            game_object->CreateComponent(Component::Type::CANVAS_RENDERER);
            game_object->CreateComponent(Component::Type::BUTTON);
            ImGui::CloseCurrentPopup();
            App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
        }

        if (ImGui::MenuItem("Image"))
        {
            game_object->CreateComponent(Component::Type::TRANSFORM_2D);
            game_object->CreateComponent(Component::Type::CANVAS_RENDERER);
            game_object->CreateComponent(Component::Type::IMAGE);
            ImGui::CloseCurrentPopup();
            App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
        }

        if (ImGui::MenuItem("Progress bar"))
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
            App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
        }

        if (ImGui::MenuItem("Text label"))
        {
            game_object->CreateComponent(Component::Type::TRANSFORM_2D);
            game_object->CreateComponent(Component::Type::CANVAS_RENDERER);
            game_object->CreateComponent(Component::Type::TEXT);
            ImGui::CloseCurrentPopup();
            App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
        }

        if (ImGui::MenuItem("Audio listener"))
        {
            game_object->CreateComponent(Component::Type::AUDIO_LISTENER);
            ImGui::CloseCurrentPopup();
            App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
        }

        if (ImGui::MenuItem("Audio source"))
        {
            game_object->CreateComponent(Component::Type::AUDIO_SOURCE);
            ImGui::CloseCurrentPopup();
            App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
        }

        if (ImGui::MenuItem("Particle system"))
        {
            game_object->CreateComponent(Component::Type::PARTICLE_SYSTEM);
            ImGui::CloseCurrentPopup();
            App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
        }

        if (ImGui::MenuItem("Billboard"))
        {
            game_object->CreateComponent(Component::Type::BILLBOARD);
            ImGui::CloseCurrentPopup();
            App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
        }

        if (ImGui::MenuItem("Video"))
        {
            game_object->CreateComponent(Component::Type::VIDEO);
            ImGui::CloseCurrentPopup();
            App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
        }

        ImGui::EndPopup();
    }

    // Open Add Script Modal.
    if (add_script)
    {
        ImGui::OpenPopup(add_script_modal_name.c_str());
    }

    ImGui::SetNextWindowSize(ImVec2(ImGui::GetContentRegionAvail().x, default_popup_size_y));
    ImGui::SetNextWindowPos(ImVec2(current_pos.x, current_pos.y - adjust_y), 0, ImVec2(0, pivot_y));
    if (ImGui::BeginPopup(add_script_modal_name.c_str()))
    {
        std::string selected_name = "";
        add_script = false;

        for (const std::string& name : App->scripting_system->GetLoadedScriptNames())
        {
            if (ImGui::MenuItem(name.c_str()))
            {
                selected_name = name;   
                ImGui::CloseCurrentPopup();
            }
        }

        if (selected_name != "")
        {
            Scripting::Script* script = App->scripting_system->InstantiateScript(
                selected_name, game_object);

            if (script != nullptr)
            {
                game_object->AddComponent(script);
                App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
            }
        }

        ImGui::EndPopup();
    }
}
