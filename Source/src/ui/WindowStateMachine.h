#pragma once
#include "Window.h"
#include "resources/ResourceStateMachine.h"

namespace ed = ax::NodeEditor;

namespace Hachiko
{
    class WindowStateMachine final : public Window
    {
    public:

        WindowStateMachine();
        WindowStateMachine(std::string name);

        ~WindowStateMachine() override;

        void Update() override;

        void SetStateMachine(ResourceStateMachine* resourceStateMachine);

        void DrawNodes();
        void DrawTransitions();
        void CreateTransitions();

        void ShowContextMenus();
        void ShowAddNodeMenu();
        void ShowNodeMenu();
        void ShowLinkMenu();

        void ShowHelp(); 

        void AddAnimationNode(unsigned int index);


    private:

        int nodeId = 0;
        int linkId = 0;

        bool editTrigger = false;
        bool editIT = false;

        ImVec2 new_node_pos;
        ed::PinId new_node_pin = 0;

        ResourceStateMachine* r_state_machine = nullptr;
        ed::EditorContext* context = nullptr;
    };
} // namespace Hachiko