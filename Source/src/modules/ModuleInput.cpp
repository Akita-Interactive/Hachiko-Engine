#include "core/hepch.h"
#include "ModuleInput.h"

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleSceneManager.h"
#include "ModuleEvent.h"
#include "events/Event.h"

Hachiko::ModuleInput::ModuleInput() :
    keyboard(new KeyState[MAX_KEYS])
{
    memset(keyboard, static_cast<int>(KeyState::KEY_IDLE), sizeof(KeyState) * MAX_KEYS);
    memset(mouse, static_cast<int>(KeyState::KEY_IDLE), sizeof(KeyState) * NUM_MOUSE_BUTTONS);
}

Hachiko::ModuleInput::~ModuleInput()
{
    RELEASE_ARRAY(keyboard);
}

// Called before render is available
bool Hachiko::ModuleInput::Init()
{
    HE_LOG("Init SDL input event system");
    bool ret = true;
    SDL_Init(0);

    if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
    {
        HE_LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
        ret = false;
    }

    return ret;
}

UpdateStatus Hachiko::ModuleInput::PreUpdate(const float delta)
{
    UpdateInputMaps();

    ImVec2 m_pos = ImGui::GetMousePos();
    mouse_position.x = m_pos.x;
    mouse_position.y = m_pos.y;

    mouse_normalized_motion.x = 0;
    mouse_normalized_motion.y = 0;
    mouse_pixels_motion.x = 0;
    mouse_pixels_motion.y = 0;

    SDL_Event sdl_event;

    scroll_delta = 0;

    while (SDL_PollEvent(&sdl_event) != 0)
    {
        ImGui_ImplSDL2_ProcessEvent(&sdl_event);
        switch (sdl_event.type)
        {
        case SDL_QUIT:
            return UpdateStatus::UPDATE_STOP;
        case SDL_WINDOWEVENT:
            if (sdl_event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
            {
                App->window->WindowResized();

                // Update the cached inverses of window size so that we have a
                // sensitive MousePositionDelta and MousePosition.
                UpdateWindowSizeInversedCaches(sdl_event.window.data1, sdl_event.window.data2);
            }

            break;
        case SDL_MOUSEBUTTONDOWN:
            mouse[sdl_event.button.button - 1] = KeyState::KEY_DOWN;
            if (sdl_event.button.button == SDL_BUTTON_LEFT)
            {
                const ImVec2 mouse_pos = ImGui::GetMousePos();
                NotifyMouseAction(float2(mouse_pos.x, mouse_pos.y), MouseEventPayload::Action::CLICK);
            }
            break;
        case SDL_MOUSEBUTTONUP:
            mouse[sdl_event.button.button - 1] = KeyState::KEY_UP;
            if (sdl_event.button.button == SDL_BUTTON_LEFT)
            {
                const ImVec2 mouse_pos = ImGui::GetMousePos();
                NotifyMouseAction(float2(mouse_pos.x, mouse_pos.y), MouseEventPayload::Action::RELEASE);
            }
            break;
        case SDL_MOUSEMOTION:
            mouse_normalized_motion.x = sdl_event.motion.xrel * _window_width_inverse;
            mouse_normalized_motion.y = sdl_event.motion.yrel * _window_height_inverse;

            mouse_pixels_motion.x = sdl_event.motion.xrel;
            mouse_pixels_motion.y = sdl_event.motion.yrel;

            break;
        case SDL_MOUSEWHEEL:
            scroll_delta = sdl_event.wheel.y;
            break;
        case SDL_DROPFILE:
        {
            HE_LOG("Dropped file: %s", sdl_event.drop.file);
            Hachiko::Event fileDropped(Event::Type::FILE_ADDED);
            fileDropped.SetEventData<FileAddedEventPayload>(sdl_event.drop.file);
            App->event->Publish(fileDropped);
            SDL_free(sdl_event.drop.file);
        }
        break;
        default:
            scroll_delta = 0.0f;
            break;
        }
    }

    return UpdateStatus::UPDATE_CONTINUE;
}

void Hachiko::ModuleInput::UpdateInputMaps()
{
    mouse_normalized_motion = {0, 0};

    const Uint8* keys = SDL_GetKeyboardState(nullptr);

    for (int i = 0; i < MAX_KEYS; ++i)
    {
        if (keys[i] == 1)
        {
            if (keyboard[i] == KeyState::KEY_IDLE)
            {
                keyboard[i] = KeyState::KEY_DOWN;
            }
            else
            {
                keyboard[i] = KeyState::KEY_REPEAT;
            }
        }
        else
        {
            if (keyboard[i] == KeyState::KEY_REPEAT || keyboard[i] == KeyState::KEY_DOWN)
            {
                keyboard[i] = KeyState::KEY_UP;
            }
            else
            {
                keyboard[i] = KeyState::KEY_IDLE;
            }
        }
    }

    for (auto& mouse_button : mouse)
    {
        if (mouse_button == KeyState::KEY_DOWN)
        {
            mouse_button = KeyState::KEY_REPEAT;
        }

        if (mouse_button == KeyState::KEY_UP)
        {
            mouse_button = KeyState::KEY_IDLE;
        }
    }
}

void Hachiko::ModuleInput::UpdateWindowSizeInversedCaches(int width,
                                                          int height)
{
    _window_width_inverse = 1.0f / width;
    _window_height_inverse = 1.0f / height;
}

void Hachiko::ModuleInput::NotifyMouseAction(float2 position, Hachiko::MouseEventPayload::Action action)
{
    Event mouse_action(Event::Type::MOUSE_ACTION);
    mouse_action.SetEventData<MouseEventPayload>(action, position);
    App->event->Publish(mouse_action);
}

bool Hachiko::ModuleInput::CleanUp()
{
    HE_LOG("Quitting SDL input event subsystem");
    SDL_QuitSubSystem(SDL_INIT_EVENTS);
    return true;
}
