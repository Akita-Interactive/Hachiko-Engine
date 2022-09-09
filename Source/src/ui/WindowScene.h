#pragma once

#include "Window.h"

#include <imgui.h>
#include <ImGuizmo.h>

namespace Hachiko
{
    class GameObject;
    class ComponentCamera;
    class Scene;

    class WindowScene final : public Window
    {
        struct PlayModeBlinker
        {
            const float duration = 1.0f;
            const float4 active_color = {0.0f, 0.5f, 1.0f, 1.0f};
            const float4 passive_color = {0.0f, 0.5f, 1.0f, 0.1f};
            float4 current_color = {0.0f, 0.0f, 0.0f, 1.0f};
            float progress = 0.0f;
            bool is_passive_to_active = true;
        };

    public:
        WindowScene();

        void Init() override;
        void Update() override;

        [[nodiscard]] bool IsHovering() const
        {
            return hovering;
        }

        [[nodiscard]] bool IsFocused() const
        {
            return focused;
        }

        // Returns IsHovering() && IsFocused.
        [[nodiscard]] bool IsActive() const
        {
            return IsHovering() && IsFocused();
        }

        // Checks if mouse_position is partially outside of the window meaning
        // that if the mouse_position.x or mouse_position.y is outside, returns
        // the latest position that was inside the window, if the
        // mouse_position is fully inside the window, leaves mouse_position
        // untouched.
        void ClampMousePosition(float2& mouse_position) const;

        // Normalizes the position inside the scene window, not the whole
        // engine editor window.
        [[nodiscard]] float2 NormalizePositionToScene(const float2& position) const;

        [[nodiscard]] const float2& GetViewportSize() const;
        [[nodiscard]] const float2& GetViewportPosition() const;

    private:
        void GuizmoOptionsController();
        void ToolbarMenu() const;
        void DrawScene();
        void Controller() const;
        void UpdatePlayModeBlinker();

        GameObject* SelectObject(const ComponentCamera* camera, const Scene* scene) const;        

        ImVec2 imguizmo_size = {100.0f, 100.0f};
        bool using_guizmo = false;
        bool focused = false;
        bool hovering = false;

        ImGuizmo::OPERATION guizmo_operation = ImGuizmo::TRANSLATE;
        ImGuizmo::MODE guizmo_mode = ImGuizmo::WORLD;

        ImVec2 guizmo_rect_origin = {0.0f, 0.0f};

        PlayModeBlinker play_mode_blinker;

        float2 texture_position = float2::zero;
        float2 texture_size = float2::zero;
        float2 viewport_position = float2::zero;
        float2 viewport_size = float2::zero;

        float2 last_hover_position = float2::zero;
        float2 viewport_max_position = float2::zero;
        float2 viewport_min_position = float2::zero;

        bool changed_game_object = false;
    };
}
