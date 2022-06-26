#include "core/hepch.h"
#include "WindowStateMachine.h"

#include "imgui_node_editor.h"
#include "modules/ModuleEditor.h"

#include "components/ComponentAnimation.h"

#define DEFAULT_BLEND 300


Hachiko::WindowStateMachine::WindowStateMachine() : Window("State Machine editor", false)
{
    context = ax::NodeEditor::CreateEditor();
}

Hachiko::WindowStateMachine::~WindowStateMachine()
{
    ax::NodeEditor::DestroyEditor(context);
}

void Hachiko::WindowStateMachine::Update()
{
    ImGui::SetNextWindowSize(ImVec2(400.0f, 200.0f), ImGuiCond_FirstUseEver);

    ResourceStateMachine* r_state_machine = nullptr;
    GameObject* selected_gameobject = App->editor->GetSelectedGameObject();
    if (selected_gameobject != nullptr)
    {
        ComponentAnimation* component_animation = selected_gameobject->GetComponent<ComponentAnimation>();
        r_state_machine = (component_animation != nullptr) ? component_animation->GetStateMachine() : nullptr;
    }
    
    std::string window_name
        = StringUtils::Concat(ICON_FA_BEZIER_CURVE, " Animator", (r_state_machine != nullptr) ? " " + r_state_machine->state_m_name : "", " - Left Alt + H for help###WindowStateMachine");
    if (!ImGui::Begin(window_name.c_str(), &active))
    {
        ImGui::End();
        return;
    }

    ax::NodeEditor::SetCurrentEditor(context);
    ax::NodeEditor::Begin("State Machine Editor"); // TODO: Revise why this causes memory leaks
    
    if (r_state_machine != nullptr)
    {
        DrawNodes(r_state_machine);
        DrawTransitions(r_state_machine);
        CreateTransitions(r_state_machine);

        ax::NodeEditor::Suspend();
        ShowContextMenus();
        ShowNodeMenu(r_state_machine);
        ShowLinkMenu(r_state_machine);
        ShowAddNodeMenu(r_state_machine);
        ax::NodeEditor::Resume();
   
        ShowHelp();

        if (ImGui::IsKeyDown(SDL_SCANCODE_ESCAPE))
        {
            editTrigger = false;
            editIT = false;
        }
    }

    ax::NodeEditor::End();
    ax::NodeEditor::SetCurrentEditor(nullptr);

    ImGui::End();
}

void Hachiko::WindowStateMachine::DrawNodes(ResourceStateMachine* r_state_machine)
{
    for (int i = 0; i < r_state_machine->nodes.size(); ++i)
    {
        ax::NodeEditor::PushStyleColor(ax::NodeEditor::StyleColor_PinRect, ImColor(60, 180, 255, 150));
        ax::NodeEditor::PushStyleColor(ax::NodeEditor::StyleColor_PinRectBorder, ImColor(60, 180, 255, 150));

        ax::NodeEditor::BeginNode(i * 3 + 1);
        ImGui::Indent(1.0);
        ImGui::TextColored(ImVec4(255, 255, 0, 255), r_state_machine->GetNodeName(i).c_str());

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f * ImGui::GetStyle().Alpha);

        ImVec2 size = ax::NodeEditor::GetNodeSize(i * 3 + 1);

        ImGui::PopStyleVar();

        ImGui::Dummy(ImVec2(96.0, 8.0));
        ImGui::BulletText("Clip: %s", r_state_machine->GetNodeClip(i).c_str());
        if (i == r_state_machine->GetDefaultNode())
        {
            ImGui::BulletText("Default");
        }

        ImGui::Dummy(ImVec2(96.0, 8.0));

        ImGui::Dummy(ImVec2(64.0, 8.0));

        // In Pin
        ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PinArrowSize, 8.0f);
        ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PinArrowWidth, 8.0f);
        ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PinRadius, 10.0f);
        ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_TargetDirection, ImVec2(0.0f, 0.0f));
        ax::NodeEditor::BeginPin(i * 3 + 2, ax::NodeEditor::PinKind::Input);
        ImGui::Text("In");
        ax::NodeEditor::EndPin();
        ax::NodeEditor::PopStyleVar(4);

        // Out Pin
        ImGui::SameLine(size.x - 40);
        ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PinArrowSize, 0.0f);
        ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PinArrowWidth, 0.0f);
        ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_TargetDirection, ImVec2(0.0f, 0.0f));
        ax::NodeEditor::BeginPin(i * 3 + 3, ax::NodeEditor::PinKind::Output);
        ImGui::Text("Out");

        ax::NodeEditor::EndPin();

        ax::NodeEditor::EndNode();

        ax::NodeEditor::PopStyleVar(3);
        ax::NodeEditor::PopStyleColor(2);
    }
}

void Hachiko::WindowStateMachine::DrawTransitions(ResourceStateMachine* r_state_machine)
{
    ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_LinkStrength, 4.0f);
    for (const Hachiko::ResourceStateMachine::Transition& transition : r_state_machine->transitions)
    {
        int sourceID = r_state_machine->FindNode(transition.source);
        int targetID = r_state_machine->FindNode(transition.target);

        int linkID = sourceID * 100 + targetID;
        ax::NodeEditor::Link(linkID, sourceID * 3 + 3, targetID * 3 + 2);
    }
    ax::NodeEditor::PopStyleVar(1);
}

void Hachiko::WindowStateMachine::CreateTransitions(ResourceStateMachine* r_state_machine)
{
    if (ax::NodeEditor::BeginCreate(ImColor(255, 255, 255), 2.0f))
    {
        auto showLabel = [](const char* label, ImColor color) {
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
            auto size = ImGui::CalcTextSize(label);

            auto padding = ImGui::GetStyle().FramePadding;
            auto spacing = ImGui::GetStyle().ItemSpacing;

            ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

            auto rectMin = ImGui::GetCursorScreenPos() - padding;
            auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

            auto drawList = ImGui::GetWindowDrawList();
            drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
            ImGui::TextUnformatted(label);
        };

        ax::NodeEditor::PinId startPinId = 0, endPinId = 0;
        if (ax::NodeEditor::QueryNewLink(&startPinId, &endPinId))
        {
            if (startPinId && endPinId)
            {
                bool startIsInput = int(startPinId.Get() - 1) % 3 == 1;
                bool endIsInput = int(endPinId.Get() - 1) % 3 == 1;
                int startNode = int(startPinId.Get() - 1) / 3;
                int endNode = int(endPinId.Get() - 1) / 3;

                if (endPinId == startPinId)
                {
                    ax::NodeEditor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                }
                else if (startIsInput == endIsInput)
                {
                    showLabel("x Incompatible Pins. Must be In->Out", ImColor(45, 32, 32, 180));
                    ax::NodeEditor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                }
                else if (startNode == endNode)
                {
                    showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
                    ax::NodeEditor::RejectNewItem(ImColor(255, 0, 0), 1.0f);
                }
                else
                {
                    showLabel("+ Create Link", ImColor(32, 45, 32, 180));
                    if (ax::NodeEditor::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
                    {
                        if (startIsInput)
                        {
                            r_state_machine->AddTransition(r_state_machine->GetNodeName(endNode).c_str(), r_state_machine->GetNodeName(startNode).c_str(), "", DEFAULT_BLEND);
                        }
                        else
                        {
                            r_state_machine->AddTransition(r_state_machine->GetNodeName(startNode).c_str(), r_state_machine->GetNodeName(endNode).c_str(), "", DEFAULT_BLEND);
                        }

                        r_state_machine->Save();
                    }
                }
            }
        }
    }
    ax::NodeEditor::EndCreate();
}

void Hachiko::WindowStateMachine::ShowContextMenus()
{
    auto openPopupPosition = ImGui::GetMousePos();

    ax::NodeEditor::NodeId contextNodeId = 0;
    ax::NodeEditor::PinId contextPinId = 0;
    ax::NodeEditor::LinkId contextLinkId = 0;

    if (ax::NodeEditor::ShowBackgroundContextMenu())
    {
        new_node_pos = ImGui::GetMousePos();
        ImGui::OpenPopup("Add Node Menu");
    }
    else if (ax::NodeEditor::ShowNodeContextMenu(&contextNodeId))
    {
        nodeId = int(contextNodeId.Get() - 1) / 3;
        ImGui::OpenPopup("Node Menu");
    }
    else if (ax::NodeEditor::ShowLinkContextMenu(&contextLinkId))
    {
        linkId = int(contextLinkId.Get());
        ImGui::OpenPopup("Link Menu");
    }
}

void Hachiko::WindowStateMachine::ShowAddNodeMenu(ResourceStateMachine* r_state_machine)
{
    if (ImGui::BeginPopup("Add Node Menu"))
    {
        ImGui::TextUnformatted("Create Node Menu");
        ImGui::Separator();

        if (ImGui::BeginMenu("New animation"))
        {
            for (unsigned int i = 0, count = r_state_machine->GetNumClips(); i < count; ++i)
            {
                if (ImGui::MenuItem(r_state_machine->GetClipName(i).c_str()))
                {
                    unsigned int node_idx = r_state_machine->GetNumNodes();
                    ax::NodeEditor::SetNodePosition(node_idx * 3 + 1, ax::NodeEditor::ScreenToCanvas(new_node_pos));
                    AddAnimationNode(r_state_machine, i);

                    unsigned int out_node = 0;
                    if (new_node_pin != ax::NodeEditor::PinId::Invalid)
                    {
                        unsigned int out_node = unsigned int(new_node_pin.Get() - 1) / 3;
                        r_state_machine->AddTransition(r_state_machine->GetNodeName(out_node), r_state_machine->GetNodeName(node_idx), "", DEFAULT_BLEND);
                        r_state_machine->Save();
                    }
                }
            }

            ImGui::EndMenu();
        }
        ImGui::EndPopup();
    }
}

void Hachiko::WindowStateMachine::ShowNodeMenu(ResourceStateMachine* r_state_machine)
{
    if (ImGui::BeginPopup("Node Menu"))
    {
        ImGui::TextUnformatted("Node Menu");
        ImGui::Separator();
        
        if (ImGui::BeginMenu("Edit clip"))
        {
            if (ImGui::MenuItem("Change clip"))
            {
                r_state_machine->EditNodeClip(r_state_machine->GetNodeName(nodeId).c_str(), "newClip");
                ImGui::CloseCurrentPopup();
            }

            if (ImGui::MenuItem("Change looping"))
            {
                r_state_machine->EditClipLoop(r_state_machine->GetNodeName(nodeId).c_str(), !r_state_machine->clips[r_state_machine->FindClip(r_state_machine->nodes[nodeId].clip.c_str())].loop);
                ImGui::CloseCurrentPopup();
            }
            
            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Delete node"))
        {
            ax::NodeEditor::DeleteNode(ax::NodeEditor::NodeId((nodeId + 1) * 3));
            r_state_machine->RemoveNode(r_state_machine->GetNodeName(nodeId).c_str());
            ImGui::CloseCurrentPopup();
        }
        
        r_state_machine->Save();
        ImGui::EndPopup();
    }
}

void Hachiko::WindowStateMachine::ShowLinkMenu(ResourceStateMachine* r_state_machine)
{
    if (ImGui::BeginPopup("Link Menu"))
    {
        ImGui::TextUnformatted("Transition Menu");
        ImGui::Separator();

        if (ImGui::BeginMenu("Edit transition"))
        {
            if (ImGui::MenuItem("Edit trigger"))
            {
                editTrigger = true;
            }

            if (ImGui::MenuItem("Edit interpolation time"))
            {
                editIT = true;
            }

            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Delete"))
        {
            ax::NodeEditor::DeleteLink(ax::NodeEditor::LinkId((linkId + 1) + 2));
            r_state_machine->RemoveTransitionWithTarget(r_state_machine->nodes[linkId / 100].name, r_state_machine->nodes[linkId % 100].name);
            r_state_machine->Save();
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    if (editTrigger)
    {
        ImGui::OpenPopup("editTrigger");
        if (ImGui::BeginPopup("editTrigger"))
        {
            std::string sourceName = r_state_machine->nodes[linkId / 100].name;
            std::string targetName = r_state_machine->nodes[linkId % 100].name;
            const char* trigger = r_state_machine->transitions[r_state_machine->FindTransitionWithTarget(sourceName, targetName)].trigger.c_str();

            static char newTrigger[128] = "";
            snprintf(newTrigger, 128, trigger ? trigger : "");
            const ImGuiInputTextFlags editTrigger_input_flags = ImGuiInputTextFlags_EnterReturnsTrue;

            if (ImGui::InputText(" Edit trigger", newTrigger, IM_ARRAYSIZE(newTrigger), editTrigger_input_flags))
            {
                editTrigger = false;
                r_state_machine->EditTransitionTrigger(sourceName, targetName, newTrigger);
                r_state_machine->Save();
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

    }

    if (editIT)
    {
        ImGui::OpenPopup("editIT");
        if (ImGui::BeginPopup("editIT"))
        {
            std::string sourceName = r_state_machine->nodes[linkId / 100].name;
            std::string targetName = r_state_machine->nodes[linkId % 100].name;
            unsigned int it = r_state_machine->transitions[r_state_machine->FindTransitionWithTarget(sourceName, targetName)].blend;

            static char newIT[128] = "";
            snprintf(newIT, 128, std::to_string(it).c_str());
            const ImGuiInputTextFlags editIT_input_flags = ImGuiInputTextFlags_EnterReturnsTrue;

            if (ImGui::InputText(" Edit interpolation time", newIT, IM_ARRAYSIZE(newIT), editIT_input_flags))
            {
                editIT = false;
                r_state_machine->EditTransitionInterpolationTime(r_state_machine->nodes[linkId / 100].name, r_state_machine->nodes[linkId % 100].name, std::stoi(newIT));
                r_state_machine->Save();
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

}

void Hachiko::WindowStateMachine::ShowHelp() 
{
    if (ImGui::IsKeyDown(SDL_SCANCODE_LALT) && ImGui::IsKeyPressed(SDL_SCANCODE_H, false))
    {
        ImGui::OpenPopup("Help");
    }

    if (ImGui::BeginPopup("Help"))
    {
        ImGui::Text("Controls:");
        ImGui::BulletText("Mouse left-click while moving the mouse to move around the editor.");
        ImGui::Separator();
        ImGui::BulletText("Mouse left-clicking in the background to add a node.");
        ImGui::Separator();
        ImGui::BulletText("Mouse left-clicking hovering a node to open Node Menu.");
        ImGui::Separator();
        ImGui::BulletText("Mouse right-clicking hovering a pin to start a link");
        ImGui::Text("   and release it in another pin to create it.");
        ImGui::Separator();
        ImGui::BulletText("Mouse left-clicking hovering a link to open Transition Menu.");
        ImGui::Separator();
        ImGui::BulletText("ESC to close a menu.");
        ImGui::Separator();
        ImGui::Text("Node legend:           ______________________");
        //ImGui::Text("                      |       Node name      |");
        ImGui::Text("                      |      ");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(255, 255, 0, 255),"Node name");
        ImGui::SameLine();
        ImGui::Text("     |");
        ImGui::Text("                      | Clip name | looping? |");
        ImGui::Text("                      | In               Out |");

        ImGui::EndPopup();
    }
}

void Hachiko::WindowStateMachine::AddAnimationNode(ResourceStateMachine* r_state_machine, unsigned int index)
{
    std::string name = r_state_machine->GetClipName(index);
    std::string clip = r_state_machine->GetClipName(index);

    unsigned int node_idx = r_state_machine->FindNode(name);

    unsigned int counter = 0;
    while (node_idx < r_state_machine->GetNumNodes())
    {
        char tmp[128];
        snprintf(tmp, 127, "%s_%d", name.c_str(), ++counter);
        name = std::string(tmp);
        node_idx = r_state_machine->FindNode(name);
    }

    r_state_machine->AddNode(name, clip);
    r_state_machine->Save();
}
