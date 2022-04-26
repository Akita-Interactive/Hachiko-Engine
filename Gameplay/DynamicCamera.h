#pragma once

#include <scripting/Script.h>

namespace Hachiko
{ 
class GameObject;
namespace Scripting
{
class DynamicCamera : public Script
{
	SERIALIZATION_METHODS(false)

public:
	DynamicCamera(GameObject* game_object);
	~DynamicCamera() override = default;

	void OnAwake() override;
	void OnStart() override;
	void OnUpdate() override;

private:
	SERIALIZE_FIELD(math::float3, _start_point);
	SERIALIZE_FIELD(math::float3, _end_point);
	SERIALIZE_FIELD(float, _speed);
	SERIALIZE_FIELD(float, _lerp_position);
};
} // namespace Scripting
} // namespace Hachiko