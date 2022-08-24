#include "core/hepch.h"
#include "WindowStateMachine.h"

#include "imgui_node_editor.h"
#include "modules/ModuleEditor.h"

#define DEFAULT_BLEND 300


Hachiko::WindowStateMachine::WindowStateMachine() : Window("State Machine editor - Press Left Alt + H for help", false)
{}

Hachiko::WindowStateMachine::WindowStateMachine(std::string name) : Window(std::string(name + std::string(" State Machine editor - Press Left Alt + H for help")).c_str(), false)
{
    context = ed::CreateEditor();
}

Hachiko::WindowStateMachine::~WindowStateMachine()
{
    r_state_machine = nullptr;
    ed::DestroyEditor(context);
}

void Hachiko::WindowStateMachine::Update()
{
    
    ImGui::SetNextWindowSize(ImVec2(400.0f, 200.0f), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin((std::string(ICON_FA_BEZIER_CURVE " ") + r_state_machine->state_m_name.c_str() + " - Left Alt + H for help").c_str(), &active))
    {
        ImGui::End();
        return;
    }

    ed::SetCurrentEditor(context);
    ed::Begin("State Machine Editor", ImVec2(0.0, 0.0f)); // TODO: Revise why this causes memory leaks
    
    DrawNodes();
    DrawTransitions();
    CreateTransitions();

    ed::Suspend();
    ShowContextMenus();
    ShowNodeMenu();
    ShowLinkMenu();
    ShowAddNodeMenu();
    ed::Resume();
   
    ShowHelp();

    if (ImGui::IsKeyDown(SDL_SCANCODE_ESCAPE))
    {
        editTrigger = false;
        editIT = false;
    }

    ed::End();
    ed::SetCurrentEditor(nullptr);

    ImGui::End();
}

void Hachiko::WindowStateMachine::DrawNodes()
{
    for (int i = 0; i < r_state_machine->nodes.size(); ++i)
    {
        ed::PushStyleColor(ed::StyleColor_PinRect, ImColor(60, 180, 255, 150));
        ed::PushStyleColor(ed::StyleColor_PinRectBorder, ImColor(60, 180, 255, 150));

        ed::BeginNode(i * 3 + 1);
        ImGui::Indent(1.0);
        ImGui::TextColored(ImVec4(255, 255, 0, 255), r_state_machine->GetNodeName(i).c_str());

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f * ImGui::GetStyle().Alpha);

        ImVec2 size = ed::GetNodeSize(i * 3 + 1);

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
        ed::PushStyleVar(ed::StyleVar_PinArrowSize, 8.0f);
        ed::PushStyleVar(ed::StyleVar_PinArrowWidth, 8.0f);
        ed::PushStyleVar(ed::StyleVar_PinRadius, 10.0f);
        ed::PushStyleVar(ed::StyleVar_TargetDirection, ImVec2(0.0f, 0.0f));
        ed::BeginPin(i * 3 + 2, ed::PinKind::Input);
        ImGui::Text("In");
        ed::EndPin();
        ed::PopStyleVar(4);

        // Out Pin
        ImGui::SameLine(size.x - 40);
        ed::PushStyleVar(ed::StyleVar_PinArrowSize, 0.0f);
        ed::PushStyleVar(ed::StyleVar_PinArrowWidth, 0.0f);
        ed::PushStyleVar(ed::StyleVar_TargetDirection, ImVec2(0.0f, 0.0f));
        ed::BeginPin(i * 3 + 3, ed::PinKind::Output);
        ImGui::Text("Out");

        ed::EndPin();

        ed::EndNode();

        ed::PopStyleVar(3);
        ed::PopStyleColor(2);
    }
}

void Hachiko::WindowStateMachine::DrawTransitions()
{
    ed::PushStyleVar(ed::StyleVar_LinkStrength, 4.0f);
    for (const Hachiko::ResourceStateMachine::Transition& transition : r_state_machine->transitions)
    {
        int sourceID = r_state_machine->FindNode(transition.source);
        int targetID = r_state_machine->FindNode(transition.target);

        int linkID = sourceID * 100 + targetID;
        ed::Link(linkID, sourceID * 3 + 3, targetID * 3 + 2);
    }
    ed::PopStyleVar(1);
}

void Hachiko::WindowStateMachine::CreateTransitions() 
{
    if (ed::BeginCreate(ImColor(255, 255, 255), 2.0f))
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

        ed::PinId startPinId = 0, endPinId = 0;
        if (ed::QueryNewLink(&startPinId, &endPinId))
        {
            if (startPinId && endPinId)
            {
                bool startIsInput = int(startPinId.Get() - 1) % 3 == 1;
                bool endIsInput = int(endPinId.Get() - 1) % 3 == 1;
                int startNode = int(startPinId.Get() - 1) / 3;
                int endNode = int(endPinId.Get() - 1) / 3;

                if (endPinId == startPinId)
                {
                    ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                }
                else if (startIsInput == endIsInput)
                {
                    showLabel("x Incompatible Pins. Must be In->Out", ImColor(45, 32, 32, 180));
                    ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                }
                else if (startNode == endNode)
                {
                    showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
                    ed::RejectNewItem(ImColor(255, 0, 0), 1.0f);
                }
                else
                {
                    showLabel("+ Create Link", ImColor(32, 45, 32, 180));
                    if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
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
    ed::EndCreate();
}

void Hachiko::WindowStateMachine::ShowContextMenus()
{
    auto openPopupPosition = ImGui::GetMousePos();

    ed::NodeId contextNodeId = 0;
    ed::PinId contextPinId = 0;
    ed::LinkId contextLinkId = 0;

    if (ed::ShowBackgroundContextMenu())
    {
        new_node_pos = ImGui::GetMousePos();
        ImGui::OpenPopup("Add Node Menu");
    }
    else if (ed::ShowNodeContextMenu(&contextNodeId))
    {
        nodeId = int(contextNodeId.Get() - 1) / 3;
        ImGui::OpenPopup("Node Menu");
    }
    else if (ed::ShowLinkContextMenu(&contextLinkId))
    {
        linkId = int(contextLinkId.Get());
        ImGui::OpenPopup("Link Menu");
    }
}

void Hachiko::WindowStateMachine::ShowAddNodeMenu()
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
                    ed::SetNodePosition(node_idx * 3 + 1, ed::ScreenToCanvas(new_node_pos));
                    AddAnimationNode(i);

                    unsigned int out_node = 0;
                    if (new_node_pin != ed::PinId::Invalid)
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

void Hachiko::WindowStateMachine::ShowNodeMenu() 
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
            ed::DeleteNode(ed::NodeId((nodeId + 1) * 3));
            r_state_machine->RemoveNode(r_state_machine->GetNodeName(nodeId).c_str());
            ImGui::CloseCurrentPopup();
        }
        
        r_state_machine->Save();
        ImGui::EndPopup();
    }
}

void Hachiko::WindowStateMachine::ShowLinkMenu() 
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
            ed::DeleteLink(ed::LinkId((linkId + 1) + 2));
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
        ImGui::Text("Notes:");
        ImGui::Text("If you select \"none\" as the animation clip a node \"ANY\" will be created.");
        ImGui::Text("\"ANY\" nodes will have their output transitions checked at every trigger.");
        ImGui::Text("\"ANY\" outputs will have the lowest priority over other transitions with");
        ImGui::Text("the same trigger.");
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

void Hachiko::WindowStateMachine::AddAnimationNode(unsigned int index) 
{
    std::string clip = r_state_machine->GetClipName(index);
    std::string name;

    if (clip == r_state_machine->none_clip_name)
    {
        name = r_state_machine->any_state_node_name;
    }
    else
    {
        name = r_state_machine->GetClipName(index);
    }

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

void Hachiko::WindowStateMachine::SetStateMachine(ResourceStateMachine* resourceStateMachine) 
{
    r_state_machine = resourceStateMachine;
}
