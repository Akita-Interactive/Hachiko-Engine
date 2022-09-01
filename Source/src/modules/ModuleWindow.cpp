#include "core/hepch.h"

#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "ModuleRender.h"
#include "core/preferences/src/EditorPreferences.h"

Hachiko::ModuleWindow::ModuleWindow() = default;

Hachiko::ModuleWindow::~ModuleWindow() = default;

bool Hachiko::ModuleWindow::Init()
{
    HE_LOG("INITIALIZING MODULE: WINDOW");

    GetMonitorResolution(max_width, max_height);
    width = static_cast<int>(max_width * WINDOWED_RATIO);
    height = static_cast<int>(max_height * WINDOWED_RATIO);

    editor_prefs = App->preferences->GetEditorPreference();

    fullscreen = editor_prefs->IsFullscreen();
    resizable = editor_prefs->IsResizable();
    
    HE_LOG("Init SDL window & surface");
    bool ret = true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        HE_LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
        ret = false;
    }
    else
    {
        Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_MAXIMIZED;

        if (fullscreen)
        {
            flags |= SDL_WINDOW_FULLSCREEN;
        }
        if (resizable)
        {
            flags |= SDL_WINDOW_RESIZABLE;
        }

        App->renderer->SetOpenGLAttributes();
        window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

        if (window == nullptr)
        {
            HE_LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            ret = false;
        }
        else
        {
            // Get window surface, it must be updated on resize
            screen_surface = SDL_GetWindowSurface(window);
        }
        SDL_DisplayMode mode;
        SDL_GetDisplayMode(0, 0, &mode);
        refresh_rate = mode.refresh_rate;
    }

    return ret;
}

bool Hachiko::ModuleWindow::Start()
{
    SetVsync(editor_prefs->IsVsyncActive());

    return true;
}

// Called before quitting
bool Hachiko::ModuleWindow::CleanUp()
{
    HE_LOG("Destroying SDL window and quitting all SDL systems");
    
    editor_prefs->SetFullscreen(fullscreen);
    editor_prefs->SetResizable(resizable);
    editor_prefs->SetVsync(vsync);

    //Destroy window
    if (window != nullptr)
    {
        SDL_DestroyWindow(window);
    }

    //Quit SDL subsystems
    SDL_Quit();
    return true;
}

void Hachiko::ModuleWindow::WindowResized()
{
    // Update window surface so it is correct
    SDL_UpdateWindowSurface(window);
    screen_surface = SDL_GetWindowSurface(window);
}

void Hachiko::ModuleWindow::SetFullScreen(bool fullscreen) const
{
    // Method returns negative error code on failure
    if (fullscreen)
    {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
    else
    {
        SDL_SetWindowFullscreen(window, 0);
    }
}

void Hachiko::ModuleWindow::SetResizable(bool resizable) const
{
    SDL_SetWindowResizable(window, static_cast<SDL_bool>(resizable));
}

void Hachiko::ModuleWindow::SetSize(int w, int h) const
{
    SDL_SetWindowSize(window, w, h);
}

void Hachiko::ModuleWindow::SetVsync(bool vsync_enabled)
{
    vsync = vsync_enabled;
    SDL_GL_SetSwapInterval(vsync);
    App->preferences->GetEditorPreference()->SetVsync(vsync);
}

void Hachiko::ModuleWindow::OptionsMenu()
{
    if (Widgets::Checkbox("Fullscreen", &fullscreen))
    {
        App->window->SetFullScreen(fullscreen);
    }

    if (Widgets::Checkbox("Vsync", &vsync))
    {
        SetVsync(vsync);
    }

    if (!fullscreen)
    {
        if (Widgets::Checkbox("Resizable", &resizable))
        {
            SetResizable(resizable);
        }
    }
    Widgets::Label("Monitor refresh rate", std::to_string(refresh_rate));
}

void Hachiko::ModuleWindow::GetMonitorResolution(int& width, int& height)
{
    RECT monitor;
    const HWND desktop_handle = GetDesktopWindow();
    // Get the size of screen to the variable desktop
    GetWindowRect(desktop_handle, &monitor);
    width = monitor.right;
    height = monitor.bottom;
}
