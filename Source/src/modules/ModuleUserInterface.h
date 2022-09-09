#pragma once
#include "Module.h"

namespace Hachiko
{
class ModuleUserInterface : public Module
{
public:
    ModuleUserInterface() = default;
    ~ModuleUserInterface() override = default;

    bool Init() override;
    UpdateStatus Update(float delta) override;
    bool CleanUp() override;

    void DrawUI(const Scene* scene) const;

    static void RecursiveDrawUI(
        GameObject* __restrict game_object, 
        Program* img_program, 
        Program* txt_program);

    static void RecursiveCheckMousePos(
        GameObject* game_object, 
        const float2& mouse_pos, 
        bool is_click = false);
    
    static void HandleMouseAction(MouseEventPayload::Action mouse_action);

    void CreateSquare();
    void BindSquare() const;
    void UnbindSquare() const;
    void RemoveSquare();

private:
    unsigned vbo = 0;
    unsigned ebo = 0;
    unsigned vao = 0;
};
} // namespace Hachiko
