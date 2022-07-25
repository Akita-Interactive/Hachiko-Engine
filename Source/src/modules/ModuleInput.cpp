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
    HE_LOG("INITIALIZING MODULE: INPUT");

    HE_LOG("Init SDL input event system");
    bool ret = true;
    SDL_Init(0);

    if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
    {
        HE_LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
        ret = false;
    }

    // Gamepad Controller
    SDL_Init(SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC);

    return ret;
}

UpdateStatus Hachiko::ModuleInput::PreUpdate(const float delta)
{
    UpdateInputMaps();

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
        case SDL_WINDOWEVENT:
            if (sdl_event.window.windowID == SDL_GetWindowID(App->window->GetWindow()))
            {
                if (sdl_event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                {
                    App->window->WindowResized();

                    // Update the cached inverses of window size so that we have a
                    // sensitive MousePositionDelta and MousePosition.
                    UpdateWindowSizeInversedCaches(sdl_event.window.data1, sdl_event.window.data2);
                }
                if (sdl_event.window.event == SDL_WINDOWEVENT_CLOSE)
                {
                    sdl_event.type = SDL_QUIT;
                    SDL_PushEvent(&sdl_event);
                }
            }
            break;

        case SDL_QUIT:
            return UpdateStatus::UPDATE_STOP;
        case SDL_MOUSEBUTTONDOWN:
            mouse[sdl_event.button.button - 1] = KeyState::KEY_DOWN;
            
            if (sdl_event.button.button == SDL_BUTTON_LEFT)
            {
                NotifyMouseAction(float2(mouse_pixel_position.x, mouse_pixel_position.y), MouseEventPayload::Action::CLICK);
            }

            break;
        case SDL_MOUSEBUTTONUP:
            mouse[sdl_event.button.button - 1] = KeyState::KEY_UP;
            if (sdl_event.button.button == SDL_BUTTON_LEFT)
            {
                NotifyMouseAction(float2(mouse_pixel_position.x, mouse_pixel_position.y), MouseEventPayload::Action::RELEASE);
            }
            break;
        case SDL_MOUSEMOTION:   
        {
            mouse_normalized_motion.x = sdl_event.motion.xrel * _window_width_inverse;
            mouse_normalized_motion.y = sdl_event.motion.yrel * _window_height_inverse;

            mouse_pixels_motion.x = sdl_event.motion.xrel;
            mouse_pixels_motion.y = sdl_event.motion.yrel;

            // Normalized mouse position of SDL:
            mouse_normalized_position.x = static_cast<float>(sdl_event.motion.x) * _window_width_inverse;
            mouse_normalized_position.y = static_cast<float>(sdl_event.motion.y) * _window_height_inverse;

            // Turn it to the same convention with ImGui and UI Components, and store it like that
            // to not have multiple coordinate systems:
            float2 in_imgui_coords(-1.0f + mouse_normalized_position.x * 2, -(-1.0f + mouse_normalized_position.y * 2));

            int height, width;
            App->window->GetWindowSize(width, height);

            // Store pixel based position in the same coordinate system as well:
            mouse_pixel_position.x = static_cast<float>(width) * 0.5f * in_imgui_coords.x;
            mouse_pixel_position.y = static_cast<float>(height) * 0.5f * in_imgui_coords.y;
        }
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

        case SDL_CONTROLLERDEVICEADDED:
            {
                int which = sdl_event.cdevice.which;

                if (SDL_IsGameController(which))
                {
                    sdl_game_controller = SDL_GameControllerOpen(which);
                    HE_LOG("%s number %d was added", GetControllerTypeAsString(SDL_GameControllerTypeForIndex(which)), sdl_event.cdevice.which);
                }
            }
            break;
        case SDL_CONTROLLERDEVICEREMAPPED:
            HE_LOG("Controller %d was remapped", sdl_event.cdevice.which);
            break;
        case SDL_CONTROLLERDEVICEREMOVED:
            HE_LOG("%s number was removed", SDL_GameControllerName(sdl_game_controller), sdl_event.cdevice.which);
            SDL_GameControllerClose(sdl_game_controller);
            sdl_game_controller = nullptr;
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

void Hachiko::ModuleInput::NotifyMouseAction(const float2& position, MouseEventPayload::Action action)
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

const char* Hachiko::ModuleInput::GetControllerTypeAsString(SDL_GameControllerType type)
{
    switch (type)
    {
        case SDL_GameControllerType::SDL_CONTROLLER_TYPE_PS3:
            return "PS3 Controller";
        case SDL_GameControllerType::SDL_CONTROLLER_TYPE_PS4:
            return "PS4 Controller";
        case SDL_GameControllerType::SDL_CONTROLLER_TYPE_PS5:
            return "PS5 Controller";
        case SDL_GameControllerType::SDL_CONTROLLER_TYPE_XBOX360:
            return "XBOX 360 Controller";
        case SDL_GameControllerType::SDL_CONTROLLER_TYPE_XBOXONE:
            return "XBOX One Controller";
        case SDL_GameControllerType::SDL_CONTROLLER_TYPE_NINTENDO_SWITCH_PRO:
            return "Nintendo Switch Pro Controller";
        case SDL_GameControllerType::SDL_CONTROLLER_TYPE_AMAZON_LUNA:
            return "Amazon Luna Controller";
        case SDL_GameControllerType::SDL_CONTROLLER_TYPE_GOOGLE_STADIA:
            return "Google Stadia Controller";
        default:
            return "UNKNOWN";
    }
}