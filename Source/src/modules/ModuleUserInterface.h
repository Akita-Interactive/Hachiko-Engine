#pragma once
#include "Module.h"
#include "Globals.h"
#include "MathGeoLib.h"

#include "ui/WindowScene.h"

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
        
        void HandleMouseAction(Event& evt);

        void CreateSquare();
        void BindSquare();
        void UnbindSuare();
        void RemoveSquare();

    private:
        unsigned vbo = 0, ebo = 0, vao = 0;
    };
}
