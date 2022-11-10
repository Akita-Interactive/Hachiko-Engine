#pragma once
#include "Module.h"
#include "Globals.h"
#include "events/MouseEventPayload.h"

#include <SDL.h>

namespace Hachiko
{
    constexpr int MAX_KEYS = 300;
    constexpr int NUM_MOUSE_BUTTONS = 5;
    //Analog joystick dead zone
    const int JOYSTICK_DEAD_ZONE = 8000;
    const float JOYSTICK_MAX_VALUE = 32767.0f;

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
        UpdateStatus PreUpdate(const float delta) override;
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

        [[nodiscard]] const float2& GetMouseGlobalPixelPosition() const
        {
            return mouse_global_pixel_position;
        }

        [[nodiscard]] const float2& GetMouseOpenGLPosition() const
        {
            return mouse_opengl_position;
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

        [[nodiscard]] bool IsGamepadModeOn() const
        {
            return gamepad_mode;
        }

        [[nodiscard]] bool IsGameControllerButtonUp(const int id) const
        {
            return game_controller[id] == KeyState::KEY_UP;
        }

        [[nodiscard]] bool IsGameControllerButtonDown(const int id) const
        {
            return game_controller[id] == KeyState::KEY_DOWN;
        }

        [[nodiscard]] float GetAxisNormalized(const int id)
        {
            return abs(game_controller_axis[id]) > JOYSTICK_DEAD_ZONE ? game_controller_axis[id] / JOYSTICK_MAX_VALUE : 0;
        }

        [[nodiscard]] void GoBrr(float strength, float duration)
        {
            if (!sdl_haptic)
            {
                return;
            }

            if (SDL_HapticRumbleInit(sdl_haptic) != 0)
            {
                return;
            }

            SDL_HapticRumblePlay(sdl_haptic, strength, duration);
        }

        // Gets viewport_size, viewport_position and position_to_convert in
        // monitor-global pixels.
        // viewport_position must be the position of the viewport in global-
        // monitor space and should be the top-left corner of the rectangle
        // defining the viewport.
        [[nodiscard]] static float2 ConvertGlobalPixelToOpenGLPosition(
            const float2& viewport_size, 
            const float2& viewport_position, 
            const float2& position_to_convert);

    private:
        void UpdateInputMaps();
        // TODO: Make ModuleWindow store window size instead of monitor
        // size. And get width and height from there.
        void UpdateWindowSizeInvertedCaches(int width, int height);
        static void NotifyMouseAction(MouseEventPayload::Action action);

        // Gamepad Controller
        static const char* GetControllerTypeAsString(SDL_GameControllerType type);

    private:
        KeyState* keyboard = nullptr;
        KeyState mouse[NUM_MOUSE_BUTTONS]{};
        KeyState game_controller[SDL_CONTROLLER_BUTTON_MAX]{};
        float game_controller_axis[SDL_CONTROLLER_AXIS_MAX]{};
        float2 mouse_global_pixel_position = float2::zero;
        float2 mouse_opengl_position = float2::zero;
        float2 mouse_normalized_position = float2::zero;
        float2 mouse_normalized_motion = float2::zero;
        float2 mouse_pixels_motion = float2::zero;

        // Gamepad Controller
        bool gamepad_mode = false;
        SDL_GameController* sdl_game_controller = nullptr;
        SDL_Joystick* sdl_joystick = nullptr;
        SDL_Haptic* sdl_haptic = nullptr;

        int scroll_delta{};

        float _window_width_inverse{};
        float _window_height_inverse{};
    };
}
