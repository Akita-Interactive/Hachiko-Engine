#pragma once
#include "Window.h"
#include "resources/ResourceStateMachine.h"

namespace ax
{
    namespace NodeEditor
    {
        struct PinId;
        struct EditorContext;
    }
}

namespace Hachiko
{
    class WindowStateMachine final : public Window
    {
    public:

        WindowStateMachine();

        ~WindowStateMachine() override;

        void Update() override;

        void DrawNodes(ResourceStateMachine* r_state_machine);
        void DrawTransitions(ResourceStateMachine* r_state_machine);
        void CreateTransitions(ResourceStateMachine* r_state_machine);

        void ShowContextMenus();
        void ShowAddNodeMenu(ResourceStateMachine* r_state_machine);
        void ShowNodeMenu(ResourceStateMachine* r_state_machine);
        void ShowLinkMenu(ResourceStateMachine* r_state_machine);

        void ShowHelp(); 

        void AddAnimationNode(ResourceStateMachine* r_state_machine, unsigned int index);

    private:

        int nodeId = 0;
        int linkId = 0;

        bool editTrigger = false;
        bool editIT = false;

        ImVec2 new_node_pos;
        ax::NodeEditor::PinId new_node_pin;

        ax::NodeEditor::EditorContext* context = nullptr;
    };
} // namespace Hachiko