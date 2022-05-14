#pragma once
#include "Module.h"
#include "Globals.h"
#include "events/MouseEventPayload.h"

#include <SDL.h>


namespace Hachiko
{
    constexpr int MAX_KEYS = 300;
    constexpr int NUM_MOUSE_BUTTONS = 5;

    enum class KeyState
    {
        KEY_IDLE = 0,
        KEY_DOWN,
        KEY_REPEAT,
        KEY_UP
    };

    class ModuleInput : public Module
    {
    public:
        ModuleInput();
        ~ModuleInput() override;

        bool Init() override;
        UpdateStatus PreUpdate(float delta) override;
        bool CleanUp() override;

        //general function to get the state of a key
        [[nodiscard]] KeyState GetKey(SDL_Scancode key) const
        {
            return keyboard[key];
        }

        //general function to get the state of a mouse button
        [[nodiscard]] KeyState GetMouseButton(const int id) const
        {
            return mouse[id - 1];
        }

        [[nodiscard]] bool IsModifierPressed(SDL_Keymod modifier) const
        {
            return (SDL_GetModState() & modifier);
        }

        [[nodiscard]] bool IsKeyPressed(const int id) const
        {
            return keyboard[id] == KeyState::KEY_DOWN || keyboard[id] == KeyState::KEY_REPEAT;
        }

        [[nodiscard]] bool IsKeyDown(const int id) const
        {
            return keyboard[id] == KeyState::KEY_DOWN;
        }

        [[nodiscard]] bool IsKeyUp(const int id) const
        {
            return keyboard[id] == KeyState::KEY_UP;
        }

        [[nodiscard]] bool IsMouseButtonPressed(const int id) const
        {
            return mouse[id - 1] == KeyState::KEY_DOWN || mouse[id - 1] == KeyState::KEY_REPEAT;
        }

        [[nodiscard]] bool IsMouseButtonUp(const int id) const
        {
            return mouse[id - 1] == KeyState::KEY_UP;
        }

        [[nodiscard]] bool IsMouseButtonDown(const int id) const
        {
            return mouse[id - 1] == KeyState::KEY_DOWN;
        }

        [[nodiscard]] int GetScrollDelta() const
        {
            return scroll_delta;
        }

        [[nodiscard]] const float2& GetMousePixelPosition() const
        {
            return mouse_pixel_position;
        }

        [[nodiscard]] const float2& GetMouseNormalizedPosition() const
        {
            return mouse_normalized_position;
        }

        [[nodiscard]] const float2& GetMouseNormalizedMotion() const
        {
            return mouse_normalized_motion;
        }

        [[nodiscard]] const float2& GetMousePixelsMotion() const
        {
            return mouse_pixels_motion;
        }

    private:
        void UpdateInputMaps();
        // TODO: Make ModuleWindow store window size instead of monitor
        // size. And get width and height from there.
        void UpdateWindowSizeInversedCaches(int width, int height);
        void NotifyMouseAction(float2 position, MouseEventPayload::Action action);

    private:
        KeyState* keyboard = nullptr;
        KeyState mouse[NUM_MOUSE_BUTTONS]{};
        float2 mouse_pixel_position = float2::zero;
        float2 mouse_normalized_position = float2::zero;
        float2 mouse_normalized_motion = float2::zero;
        float2 mouse_pixels_motion = float2::zero;
        int scroll_delta{};

        float _window_width_inverse{};
        float _window_height_inverse{};
    };
}
