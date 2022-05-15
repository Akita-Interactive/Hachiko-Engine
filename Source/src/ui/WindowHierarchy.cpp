#include "core/hepch.h"
#include "WindowHierarchy.h"

#include "modules/ModuleSceneManager.h"
#include "modules/ModuleInput.h"
#include "modules/ModuleEditor.h"

Hachiko::WindowHierarchy::WindowHierarchy() :
    Window("Hierarchy", true) {}

Hachiko::WindowHierarchy::~WindowHierarchy() = default;

void Hachiko::WindowHierarchy::Update()
{
    ImGui::SetNextWindowDockID(App->editor->dock_left_id, ImGuiCond_FirstUseEver);
    if (!ImGui::Begin((std::string(ICON_FA_SITEMAP " ") + name).c_str(), &active, ImGuiWindowFlags_NoNavInputs))
    {
        ImGui::End();
        return;
    }
    DrawHierarchyTree(App->scene_manager->GetRoot());
    ImGui::End();
}

void Hachiko::WindowHierarchy::CleanUp()
{
    dragged_object = nullptr;
    delete dragged_object;
}

void Hachiko::WindowHierarchy::DrawHierarchyTree(const GameObject* game_object)
{
    for (const auto go : game_object->children)
    {
       RecursiveDraw(go);
    }

    if (!App->input->IsMouseButtonPressed(SDL_BUTTON_LEFT))
    {
        dragged_object = nullptr;
    }
}

bool Hachiko::WindowHierarchy::RecursiveDraw(GameObject* game_object)
{
    bool stop = false;

    const char* str = strstr(game_object->name.c_str(), AUXILIAR_NODE);
    if (str != nullptr)
    {
        for (GameObject* go : game_object->children)
        {
            if ((stop = RecursiveDraw(go)) == true)
            {
                break;
            }
        }
    }
    else
    {
        stop = DrawGameObject(game_object, stop);
    }

    return stop;
}

bool Hachiko::WindowHierarchy::DrawGameObject(GameObject* game_object, bool stop)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

    if (game_object->children.empty())
    {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }

    bool is_selected = game_object == App->editor->GetSelectedGameObject();
    if (is_selected)
    {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    ImVec4 node_color = ImGui::GetStyle().Colors[ImGuiCol_Text];
    if (!game_object->IsActive())
    {
        node_color = ImVec4(1.f, 0.f, 0.f, 1.f);
    }

    ImGui::PushStyleColor(ImGuiCol_Text, node_color);

    const bool node_open = ImGui::TreeNodeEx(game_object, flags, game_object->name.c_str());

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && dragged_object != nullptr)
    {
        App->editor->SetSelectedGO(game_object);
    }
    
    DragAndDrop(game_object);

    if (dragged_object == nullptr && ImGui::IsItemHovered() && !ImGui::IsItemToggledOpen())
    {
        if (ImGui::IsMouseClicked(0))
        {
            App->editor->SetSelectedGO(game_object);
        }

        if (ImGui::IsMouseClicked(1))
        {
            ImGui::OpenPopup(game_object->GetName().c_str());
        }
        
        if (ImGui::IsMouseClicked(0) && is_selected)
        {
            App->editor->SetSelectedGO(nullptr);
        }
    }

    if (node_open && !game_object->children.empty() && !stop)
    {
        for (const auto go : game_object->children)
        {
            RecursiveDraw(go);
        }
    }

    // TODO: Make robust to repeted game object names
    if (ImGui::BeginPopup(game_object->GetName().c_str()))
    {
        // Alternativs: ImGui::Selectable, ImGuiHelper::ValueSelection
        // TODO: Open options to create/destroy new object or move up down in the list of children
        if (ImGui::MenuItem("Add Game Object"))
        {
            GameObject* selectedGameObject = App->editor->GetSelectedGameObject();
            if (selectedGameObject == game_object)
            {
                App->editor->SetSelectedGO(nullptr);
            }

            App->scene_manager->GetActiveScene()->CreateNewGameObject(game_object, "New Game Object");
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::MenuItem("Delete Game Object"))
        {
            RELEASE(game_object);
            dragged_object = nullptr;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (node_open)
    {
        ImGui::TreePop();
    }

    ImGui::PopStyleColor();

    return stop;
}

bool Hachiko::WindowHierarchy::DragAndDrop(GameObject* game_object)
{
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
    {
        ImGui::SetDragDropPayload("GameObject", &game_object, sizeof(GameObject*));
        ImGui::Text("%s", game_object->name.c_str());
        ImGui::EndDragDropSource();

        dragged_object = game_object;

        return true;
    }

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObject"))
        {
            IM_ASSERT(payload->DataSize == sizeof(std::intptr_t*));
            GameObject* payload_n = *(GameObject**)payload->Data;
            GameObject* parent_check = game_object->parent;
            while (parent_check != nullptr)
            {
                if (parent_check == payload_n)
                {
                    HE_LOG("Trying to move parent to its child! Operation aborted.");
                    return false;
                }
                parent_check = parent_check->parent;
            }
            payload_n->SetNewParent(game_object);
        }
        ImGui::EndDragDropTarget();
    }

    return false;
}