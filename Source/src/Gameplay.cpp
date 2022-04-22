#include "core/hepch.h"
#include "modules/ModuleInput.h"
#include "modules/ModuleSceneManager.h"
#include "modules/ModuleCamera.h"
#include "Gameplay.h"


/*TIME-----------------------------------------------------------------------*/

float Hachiko::Time::DeltaTime()
{
	// TODO: Return Gameplay Timer.
	// Added for easiness of exposing to scripts. We need to refactor that
	// timer code as it's unnecessarily complicated.
	return EngineTimer::delta_time;
}

/*---------------------------------------------------------------------------*/

/*INPUT----------------------------------------------------------------------*/

bool Hachiko::Input::GetKeyUp(KeyCode key)
{
    return App->input->GetKey(static_cast<SDL_Scancode>(key)) 
        == KeyState::KEY_UP;
}

bool Hachiko::Input::GetKey(KeyCode key)
{
    return App->input->GetKey(static_cast<SDL_Scancode>(key)) 
        == KeyState::KEY_REPEAT;
}

bool Hachiko::Input::GetKeyDown(KeyCode key)
{
    return App->input->GetKey(static_cast<SDL_Scancode>(key)) 
        == KeyState::KEY_DOWN;
}

bool Hachiko::Input::GetMouseButton(MouseButton mouse_button)
{
    return App->input->GetMouseButton(static_cast<int>(mouse_button));
}

int Hachiko::Input::GetScrollWheelDelta()
{
    return App->input->GetScrollDelta();
}

math::float2 Hachiko::Input::GetMouseDelta()
{
    float delta_x = 0;
    float delta_y = 0;

    App->input->GetMouseDeltaRelative(delta_x, delta_y);

    return math::float2(delta_x, delta_y);
}

math::float2 Hachiko::Input::GetMousePosition()
{
    float position_x = 0;
    float position_y = 0;

    App->input->GetMousePositionRelative(position_x, position_y);

    return math::float2(position_x, position_y);
}

void Hachiko::Input::GetMouseDeltaPixels(int& out_delta_x, int& out_delta_y) 
{
    App->input->GetMouseDelta(out_delta_x, out_delta_y);
}

void Hachiko::Input::GetMousePositionPixels(int& out_position_x, 
    int& out_position_y) 
{
    App->input->GetMousePosition(out_position_x, out_position_y);
}

/*---------------------------------------------------------------------------*/

/*SCENE MANAGEMENT-----------------------------------------------------------*/

void Hachiko::SceneManagement::SwitchScene(const char* name)
{
    App->scene_manager->SwitchTo(name);
}

Hachiko::GameObject* Hachiko::SceneManagement::Raycast(const float3& origin, 
    const float3& destination)
{
    return App->scene_manager->Raycast(origin, destination);
}

/*---------------------------------------------------------------------------*/

/*DEBUG----------------------------------------------------------------------*/

const Hachiko::ComponentCamera* Hachiko::Debug::GetEditorCamera() 
{
    return App->camera->GetMainCamera();
}

/*---------------------------------------------------------------------------*/