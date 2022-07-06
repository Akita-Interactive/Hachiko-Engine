#pragma once

#include <scripting/Script.h>

namespace Hachiko
{
class GameObject;
namespace Scripting
{
class PlayerController;
class PlayerCamera : public Script
{
	SERIALIZATION_METHODS(false)

public:
	PlayerCamera(GameObject* game_object);
	~PlayerCamera() override = default;

	void OnAwake() override;
	void OnStart() override;
	void OnUpdate() override;

	void Shake(float time, float intesity);

	/// <summary>
	/// Changes Camera relative position to the one defined. 
	/// If speed is NOT defined or 0 it will change instantly
	/// If duration is defined it will revert to its previous position once said time has passed
	/// </summary>
	/// <param name="new_relative_position"></param>
	/// <param name="speed"></param>
	/// <param name="duration"></param>
	void ChangeRelativePosition(math::float3 new_relative_position, float speed = 0.0f, float duration = -1.0f);

	/// <summary>
	/// Reverts Camera relative position to its previous position
	/// If speed is NOT defined or 0 it will use "_follow_delay" as value 
	/// </summary>
	/// <param name="speed"></param>
	void RevertRelativePosition(float speed = 0.0f);


	/// <summary>
	/// Switches between the defined relative position and its previous one
	/// If speed is NOT defined or 0 it will change instantly
	/// </summary>
	/// <param name="new_relative_position"></param>
	/// <param name="speed"></param>
	void SwitchRelativePosition(math::float3 new_relative_position, float speed = 0.0f);

	/// <summary>
	/// Rotates the camera, if no speed is defined it will instantly rotate
	/// </summary>
	/// <param name="new_rotation"></param>
	/// <param name="speed"></param>
	void RotateCameraTo(math::float3 new_rotation, float speed = 0.0f);

	float3 GetRelativePosition() { return _relative_position_to_player; };

private:
	void CheckForObjective();
	void SetLookAhead();
	void MoveCamera();
	void RecalculateRelativePos();
	void RecalculateRotation();
	float2 MoveCameraWithMouse();
	void ScrollWheelZoom(float3* cam_pos);
	float3 Shake();

private:
	SERIALIZE_FIELD(math::float3, _relative_position_to_player);
	SERIALIZE_FIELD(GameObject*, _objective);
	SERIALIZE_FIELD(float, _follow_delay);

	math::float3 _look_ahead = math::float3::zero;
	PlayerController* _player_ctrl = nullptr;
	GameObject* _current_objective = nullptr;
	math::float3 _relative_position_aux = math::float3::zero;
	math::float3 _updated_relative_position;
	math::Quat _updated_rotation;
	bool _is_temporary_moved = false;
	bool _is_in_position = true;
	bool _is_in_rotation = true;
	float _position_timer = 0.0f;
	float _reposition_time = 0.0f;
	float _rotation_time = 0.0f;
	float _reposition_progress = 0.0f;
	float _rotation_progress = 0.0f;
	float shake_elapsed = 0.0f;
	float shake_time = 0.0f;
	float shake_intensity = 0.0f;
	float shake_magnitude = 0.0f;
};
} // namespace Scripting
} // namespace Hachiko