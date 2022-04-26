#include "core/hepch.h"
#include "ModuleInput.h"

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleSceneManager.h"
#include "ModuleEvent.h"
#include "events/Event.h"

#define MAX_KEYS 300

Hachiko::ModuleInput::ModuleInput() :
    keyboard(new KeyState[MAX_KEYS])
{
    memset(keyboard, static_cast<int>(KeyState::KEY_IDLE), sizeof(KeyState) * MAX_KEYS);
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
    SDL_Event event;
    mouse_delta_x = 0;
    mouse_delta_y = 0;
    scroll_delta = 0;
    while (SDL_PollEvent(&event) != 0)
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
        switch (event.type)
        {
        case SDL_QUIT:
            return UpdateStatus::UPDATE_STOP;
        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
            {
                App->window->WindowResized();
                
                // Update the cached inverses of window size so that we have a
                // sensitive MousePositionDelta and MousePosition.
                UpdateWindowSizeInversedCaches(event.window.data1, event.window.data2);
            }
            
            break;
        case SDL_MOUSEMOTION:
            mouse_delta_x = event.motion.xrel;
            mouse_delta_y = event.motion.yrel;
            _mouse_delta_x_relative = mouse_delta_x * _window_width_inverse;
            _mouse_delta_y_relative = mouse_delta_x * _window_height_inverse;
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                ImVec2 mouse_pos = ImGui::GetMousePos();
                NotifyMouseAction(float2(mouse_pos.x, mouse_pos.y), MouseEventPayload::Action::CLICK);
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                ImVec2 mouse_pos = ImGui::GetMousePos();
                NotifyMouseAction(float2(mouse_pos.x, mouse_pos.y), MouseEventPayload::Action::RELEASE);
            }
            break;
        case SDL_MOUSEWHEEL:
            scroll_delta = event.wheel.y;
            break;
        case SDL_DROPFILE: 
            {
                HE_LOG("Dropped file: %s", event.drop.file);
                Hachiko::Event fileDropped(Hachiko::Event::Type::FILE_ADDED);
                fileDropped.SetEventData<Hachiko::FileAddedEventPayload>(event.drop.file);
                App->event->Publish(fileDropped);
                SDL_free(event.drop.file);
            }
            break;
        }
    }
    UpdateInputMaps();  

    return UpdateStatus::UPDATE_CONTINUE;
}

void Hachiko::ModuleInput::UpdateInputMaps()
{
    const Uint8* keys_state = SDL_GetKeyboardState(nullptr);
    for (int i = 0; i < MAX_KEYS; ++i)
    {
        if (keys_state[i] == 1)
        {
            if (keyboard[i] == KeyState::KEY_IDLE)
            {
                keyboard[i] = KeyState::KEY_DOWN;
            }
            else if (keyboard[i] != KeyState::KEY_REPEAT)
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
    keymods = SDL_GetModState();
    mouse = SDL_GetMouseState(&mouse_x, &mouse_y);
    
    _mouse_x_relative = mouse_x * _window_width_inverse;
    _mouse_y_relative = mouse_y * _window_height_inverse;
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
