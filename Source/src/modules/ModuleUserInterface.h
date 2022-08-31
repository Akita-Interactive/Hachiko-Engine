#pragma once
#include "Module.h"

namespace Hachiko
{
    class Event;

    class ModuleUserInterface : public Module
    {
    public:
        ModuleUserInterface();
        ~ModuleUserInterface() override;

        bool Init() override;
        UpdateStatus Update(float delta) override;
        bool CleanUp() override;

        void DrawUI(const Scene* scene);
        void RecursiveDrawUI(GameObject* game_object, Program* img_program, Program* txt_program);
        void RecursiveCheckMousePos(GameObject* game_object, const float2& mouse_pos, bool is_click = false);
        
    static void HandleMouseAction(MouseEventPayload::Action mouse_action);

        void CreateSquare();
        void BindSquare() const;
        void UnbindSquare() const;
        void RemoveSquare();

    private:
        unsigned vbo = 0, ebo = 0, vao = 0;
    };
}
