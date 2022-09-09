#include "core/hepch.h"

#include "Application.h"

#include "modules/ModuleInput.h"
#include "modules/ModuleWindow.h"
#include "modules/ModuleEvent.h"

#include "events/Event.h"

Hachiko::ModuleInput::ModuleInput() :
    keyboard(new KeyState[MAX_KEYS])
{
    memset(keyboard, static_cast<int>(KeyState::KEY_IDLE), sizeof(KeyState) * MAX_KEYS);
    memset(mouse, static_cast<int>(KeyState::KEY_IDLE), sizeof(KeyState) * NUM_MOUSE_BUTTONS);
    memset(game_controller, static_cast<int>(KeyState::KEY_IDLE), sizeof(KeyState) * SDL_CONTROLLER_BUTTON_MAX);
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
                    UpdateWindowSizeInvertedCaches(sdl_event.window.data1, sdl_event.window.data2);
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
                NotifyMouseAction(MouseEventPayload::Action::CLICK);
            }

            break;
        case SDL_MOUSEBUTTONUP:
            mouse[sdl_event.button.button - 1] = KeyState::KEY_UP;
            if (sdl_event.button.button == SDL_BUTTON_LEFT)
            {
                NotifyMouseAction(MouseEventPayload::Action::RELEASE);
            }
            break;
        case SDL_MOUSEMOTION:   
        {
            mouse_pixels_motion.x = sdl_event.motion.xrel;
            mouse_pixels_motion.y = sdl_event.motion.yrel;

            mouse_normalized_motion.x = mouse_pixels_motion.x * _window_width_inverse;
            mouse_normalized_motion.y = mouse_pixels_motion.y * _window_height_inverse;

            // Get monitor-global pixel position of mouse:
            int mouse_global_x = 0;
            int mouse_global_y = 0;
            
            SDL_GetGlobalMouseState(&mouse_global_x, &mouse_global_y);
            mouse_global_pixel_position.x = static_cast<float>(mouse_global_x);
            mouse_global_pixel_position.y = static_cast<float>(mouse_global_y);

            // Normalized mouse position of SDL:
            // NOTE: Contrary to the pixel position, normalized position does
            // not get updated by the changes when the mouse is outside the
            // window:
            mouse_normalized_position.x = static_cast<float>(sdl_event.motion.x) * _window_width_inverse;
            mouse_normalized_position.y = static_cast<float>(sdl_event.motion.y) * _window_height_inverse;

            // Get OpenGL position of mouse:
            int height = 0;
            int width = 0;
            App->window->GetWindowSize(width, height);
            int window_x = 0;
            int window_y = 0;
            App->window->GetWindowPosition(
                window_x, 
                window_y);

            mouse_opengl_position = ConvertGlobalPixelToOpenGLPosition(
                float2(static_cast<float>(width), static_cast<float>(height)), 
                float2(static_cast<float>(window_x), static_cast<float>(window_y)), 
                mouse_global_pixel_position
            );
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
                    sdl_joystick = SDL_JoystickOpen(which);
                    sdl_haptic = SDL_HapticOpen(which);
                    SDL_HapticRumbleInit(sdl_haptic);
                    SDL_HapticRumblePlay(sdl_haptic, 0.3f, 1000);
                    HE_LOG("%s number %d was added", GetControllerTypeAsString(SDL_GameControllerTypeForIndex(which)), sdl_event.cdevice.which);
                    gamepad_mode = true;
                }
            }

            break;
        case SDL_CONTROLLERDEVICEREMAPPED:
            HE_LOG("Controller %d was remapped", sdl_event.cdevice.which);
            break;
        case SDL_CONTROLLERDEVICEREMOVED:
            HE_LOG("%s was removed", SDL_GameControllerName(sdl_game_controller), sdl_event.cdevice.which);
            SDL_GameControllerClose(sdl_game_controller);
            SDL_JoystickClose(sdl_joystick);
            SDL_HapticClose(sdl_haptic);
            sdl_game_controller = nullptr;
            sdl_joystick = nullptr;
            sdl_haptic = nullptr;
            gamepad_mode = false;

            break;
        case SDL_CONTROLLERBUTTONDOWN: 
            HE_LOG("%d button down", sdl_event.cbutton.button);
            game_controller[sdl_event.cbutton.button] = KeyState::KEY_DOWN;
            
            break;
        case SDL_CONTROLLERBUTTONUP:
            game_controller[sdl_event.cbutton.button] = KeyState::KEY_UP;

            break;
        case SDL_CONTROLLERAXISMOTION: {
            // Triggers 
            if (sdl_event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT || sdl_event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT) 
            {
                game_controller_axis[sdl_event.caxis.axis] = sdl_event.caxis.value;
                HE_LOG("Axis: %d Value: %d", sdl_event.caxis.axis, sdl_event.caxis.value);
                break;
            }
            // Left Joystick Axis
            if (sdl_event.caxis.value < -JOYSTICK_DEAD_ZONE) 
            {
                game_controller_axis[sdl_event.caxis.axis] = sdl_event.caxis.value;
                HE_LOG("Axis: %d Value: %d", sdl_event.caxis.axis, sdl_event.caxis.value);
            }
            // Right Joystick Axis
            else if (sdl_event.caxis.value > JOYSTICK_DEAD_ZONE) 
            {
                game_controller_axis[sdl_event.caxis.axis] = sdl_event.caxis.value;
                HE_LOG("Axis: %d Value: %d", sdl_event.caxis.axis, sdl_event.caxis.value);
            }
            else 
            {
                game_controller_axis[sdl_event.caxis.axis] = 0;
                HE_LOG("Axis: %d Value: %d", sdl_event.caxis.axis, sdl_event.caxis.value);
            }

            break;
        }
        default:
            scroll_delta = 0.0f;
            break;
        }
    }

    return UpdateStatus::UPDATE_CONTINUE;
}

float2 Hachiko::ModuleInput::ConvertGlobalPixelToOpenGLPosition(
    const float2& viewport_size, 
    const float2& viewport_position, 
    const float2& position_to_convert)
{
    float2 converted_position = position_to_convert - viewport_position;

    converted_position.x -= viewport_size.x * 0.5f;
    converted_position.y = -converted_position.y + viewport_size.y * 0.5f;

    return converted_position;
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

    for (KeyState& mouse_button : mouse)
    {
        if (mouse_button == KeyState::KEY_DOWN)
        {
            mouse_button = KeyState::KEY_REPEAT;
        }
        else if (mouse_button == KeyState::KEY_UP)
        {
            mouse_button = KeyState::KEY_IDLE;
        }
    }

    for (KeyState& controller_button : game_controller)
    {
        if (controller_button == KeyState::KEY_DOWN) 
        {
            controller_button = KeyState::KEY_REPEAT;
        }
        else if (controller_button == KeyState::KEY_UP)
        {
            controller_button = KeyState::KEY_IDLE;
        }
    }
}

void Hachiko::ModuleInput::UpdateWindowSizeInvertedCaches(int width, int height)
{
    _window_width_inverse = 1.0f / static_cast<float>(width);
    _window_height_inverse = 1.0f / static_cast<float>(height);
}

void Hachiko::ModuleInput::NotifyMouseAction(MouseEventPayload::Action action)
{
    Event mouse_action(Event::Type::MOUSE_ACTION);
    mouse_action.SetEventData<MouseEventPayload>(action);
    App->event->Publish(mouse_action);
}

bool Hachiko::ModuleInput::CleanUp()
{
    HE_LOG("Quitting SDL input event subsystem");
    SDL_GameControllerClose(sdl_game_controller);
    SDL_JoystickClose(sdl_joystick);
    SDL_HapticClose(sdl_haptic);
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