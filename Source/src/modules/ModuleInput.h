#pragma once
#include "Module.h"
#include "Globals.h"
#include "events/MouseEventPayload.h"

#include <SDL.h>


namespace Hachiko
{   
    using Uint8 = unsigned __int8;

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

        [[nodiscard]] KeyState GetKey(SDL_Scancode key) const
        {
            return keyboard[key];
        }

        [[nodiscard]] bool GetKeyMod(SDL_Keymod modifier) const
        {
            return (keymods & modifier);
        }

        [[nodiscard]] bool GetMouseButton(int button) const
        {
            return (mouse & SDL_BUTTON(button));
        }

        [[nodiscard]] int GetScrollDelta() const
        {
            return scroll_delta;
        }

        // TODO: Rename to GetMouseDeltaPixels.
        void GetMouseDelta(int& x, int& y) const
        {
            x = mouse_delta_x;
            y = mouse_delta_y;
        }

        // TODO: Rename to GetMouseDelta.
        void GetMouseDeltaRelative(float& x, float& y) const
        {
            x = _mouse_delta_x_relative;
            y = _mouse_delta_y_relative;
        }

        // TODO: Rename to GetMousePositionPixels.
        void GetMousePosition(int& x, int& y) const
        {
            x = mouse_x;
            y = mouse_y;
        }

        // TODO: Rename to GetPosition.
        void GetMousePositionRelative(float& x, float& y) const
        {
            x = _mouse_x_relative;
            y = _mouse_y_relative;
        }

    private:
        void UpdateInputMaps();
        // TODO: Make ModuleWindow store window size instead of monitor
        // size. And get width and height from there.
        void UpdateWindowSizeInversedCaches(int width, int height);
        void NotifyMouseAction(float2 position, MouseEventPayload::Action action);

    private:
        KeyState* keyboard = nullptr;
        Uint32 mouse{};
        SDL_Keymod keymods{};

        int mouse_x{}, mouse_y{};
        int mouse_delta_x{}, mouse_delta_y{};

        float _mouse_x_relative{};
        float _mouse_y_relative{};
        float _mouse_delta_x_relative{};
        float _mouse_delta_y_relative{};
        float _window_width_inverse{};
        float _window_height_inverse{};
        int scroll_delta{};
    };
}
