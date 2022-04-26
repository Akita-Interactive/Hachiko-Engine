#pragma once

#include <scripting/Script.h>

namespace Hachiko
{ 
class GameObject;

namespace Scripting
{
class PlayerController : public Script
{
	SERIALIZATION_METHODS(false)

public:
	PlayerController(GameObject* game_object);
	~PlayerController() override = default;

	void OnAwake() override;
	void OnStart() override;
	void OnUpdate() override;

private:
	SERIALIZE_FIELD(float, _movement_speed);
	SERIALIZE_FIELD(float, _dash_duration);
	SERIALIZE_FIELD(float, _dash_distance);
	SERIALIZE_FIELD(bool, _is_dashing);
	SERIALIZE_FIELD(float, _dash_progress);
	SERIALIZE_FIELD(math::float3, _dash_start);
	SERIALIZE_FIELD(float, _rotation_speed);
	SERIALIZE_FIELD(bool, _is_falling);
	SERIALIZE_FIELD(float, _original_y);
	SERIALIZE_FIELD(float, _speed_y);
	SERIALIZE_FIELD(math::float3, _starting_position);
};
} // namespace Scripting
} // namespace Hachiko