#include "scriptingUtil/gameplaypch.h"
#include "misc/DynamicCamera.h"

#include "LevelManager.h"
#include "constants/Scenes.h"
#include "entities/player/PlayerCamera.h"

Hachiko::Scripting::DynamicCamera::DynamicCamera(GameObject* game_object)
    : Script(game_object, "DynamicCamera")
      , _speed(0.0f)
{
}

void Hachiko::Scripting::DynamicCamera::SetCurrentShot(unsigned target_idx)
{
    if (target_idx >= _camera_positions.size()) return;

    ComponentTransform* target = _camera_positions[_current_target]->GetTransform();
    _camera_transform->SetGlobalPosition(target->GetGlobalPosition());
    _camera_transform->SetGlobalRotation(target->GetGlobalRotation());
}

void Hachiko::Scripting::DynamicCamera::OnAwake()
{
    _level_manager = Scenes::GetLevelManager()->GetComponent<LevelManager>();
    _player_camera = Scenes::GetMainCamera()->GetComponent<PlayerCamera>();

    _camera_go = game_object->children[0];
    // Initially teleport camera to sequence start
    _player_camera->SetObjective(_camera_go, false);
    _player_camera->ChangeRelativePosition(float3(0.0f, 30.0f, 20.0f));

    // Use all children except first as camera positions
    _camera_positions = std::vector(game_object->children.begin() + 1, game_object->children.end());

    // Make all cameras inactive
    for (GameObject* _camera_position : _camera_positions)
    {
        _camera_position->SetActive(false);
    }
    // Camera 0 is the traveling camera
    _camera_transform = _camera_go->GetTransform();
    _camera = _camera_go->GetComponent<ComponentCamera>();

    SetCurrentShot(0);
    GetNextTargetShot();
    StartCameraTravel();
}

void Hachiko::Scripting::DynamicCamera::OnUpdate()
{
    if (!_playing) return;

    _level_manager->BlockInputs(true);


    bool skip = Input::IsKeyDown(Input::KeyCode::KEY_Y) ||
        Input::IsKeyDown(Input::KeyCode::KEY_F) ||
        Input::IsKeyDown(Input::KeyCode::KEY_SPACE) ||
        Input::IsGameControllerButtonDown(Input::GameControllerButton::CONTROLLER_BUTTON_Y) ||
        Input::IsGameControllerButtonDown(Input::GameControllerButton::CONTROLLER_BUTTON_A) ||
        Input::IsGameControllerButtonDown(Input::GameControllerButton::CONTROLLER_BUTTON_B);
    if (skip)
    {
        _playing = false;
        _current_target = 0;
        OnFinish(skip);
        return;
    }

    _transition_progress += Time::DeltaTime() / _transition_duration;
    if (_transition_progress >= 1.f)
    {
        _transition_progress = 1.f;
        MoveToCurrentTargetShot();
        GetNextTargetShot();
        return;
    }
    MoveToCurrentTargetShot();
}

void Hachiko::Scripting::DynamicCamera::MoveToCurrentTargetShot()
{
    ComponentTransform* target = _camera_positions[_current_target]->GetTransform();
    _camera_transform->SetGlobalPosition(float3::Lerp(_start_pos, target->GetGlobalPosition(), _transition_progress));
    _camera_transform->SetGlobalRotation(Quat::Lerp(_start_rot, target->GetGlobalRotation(), _transition_progress));
}

void Hachiko::Scripting::DynamicCamera::GetNextTargetShot()
{
    ++_current_target;
    if (_camera_positions.empty())
    {
        _current_target = 0;
        _playing = false;
        return;
    }
    if (_current_target >= _camera_positions.size())
    {
        _current_target = 1;
        _playing = false;
        OnFinish(false);
        return;
    }
    SetTravelTarget(_current_target);
}

void Hachiko::Scripting::DynamicCamera::SetTravelTarget(unsigned target_idx)
{
    _current_target = target_idx;
    ComputeTransition(_current_target);
}

void Hachiko::Scripting::DynamicCamera::ComputeTransition(unsigned target_idx)
{
    ComponentTransform* target = _camera_positions[target_idx]->GetTransform();
    _start_pos = _camera_transform->GetGlobalPosition();
    _start_rot = _camera_transform->GetGlobalRotation();
    const float distance = _camera_transform->GetGlobalPosition().Distance(target->GetGlobalPosition());
    _transition_progress = 0.f;
    _transition_duration = distance / _speed;
}

void Hachiko::Scripting::DynamicCamera::OnFinish(bool skipping)
{
    _level_manager->BlockInputs(false);
    // If skipping teleport the camera
    _player_camera->SetObjective(Scenes::GetPlayer(), skipping);
    _player_camera->RevertRelativePosition(0.0f);
    if(_enable_after)
    {
        _enable_after->SetActiveNonRecursive(true);
    }
}
