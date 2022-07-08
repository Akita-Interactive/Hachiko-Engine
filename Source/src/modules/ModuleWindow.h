#pragma once

#include "Module.h"
#include <SDL.h>

namespace Hachiko
{
    class Application;
    class EditorPreferences;

    class ModuleWindow : public Module
    {
    public:
        ModuleWindow();
        ~ModuleWindow() override;

        bool Init() override;
        bool Start() override;
        [[nodiscard]] bool CleanUp() override;

        void WindowResized();
        void SetFullScreen(bool fullscreen) const;
        void SetResizable(bool resizable) const;
        void SetSize(int w, int h) const;
        void SetVsync(bool vsync_enabled);

        [[nodiscard]] int GetWidth() const
        {
            return width;
        }

        [[nodiscard]] int GetHeight() const
        {
            return height;
        }

        void OptionsMenu();

        [[nodiscard]] SDL_Surface* GetScreenSurface() const
        {
            return screen_surface;
        }

        [[nodiscard]] SDL_Window* GetWindow() const
        {
            return window;
        }

        void GetWindowSize(int& width, int& height) 
        {
            SDL_GetWindowSize(window, &width, &height);
        }

        void GetWindowPosition(int& x, int& y)
        {
            SDL_GetWindowPosition(window, &x, &y);
        }

    private:
        static void GetMonitorResolution(int& width, int& height);

        SDL_Window* window = nullptr;
        SDL_Surface* screen_surface = nullptr;
        EditorPreferences* editor_prefs = nullptr;

        bool fullscreen{};
        bool resizable{};
        int width{};
        int height{};
        int max_width{};
        int max_height{};
        int refresh_rate{};
        bool vsync = true;
    };
}
