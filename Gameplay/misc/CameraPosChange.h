#pragma once

#include <scripting/Script.h>

namespace Hachiko
{
	class GameObject;

	namespace Scripting
	{
		class PlayerCamera;

		class CameraPosChange : public Script
		{
			SERIALIZATION_METHODS(false)

		public:
			CameraPosChange(GameObject* game_object);
			~CameraPosChange() override = default;

			void OnAwake() override;
			void OnStart() override;
			void OnUpdate() override;
		private:
			GameObject* _player{};
			PlayerCamera* _camera{};
			bool _is_inside = false;
			OBB boundingbox;
			float3 global_pos = float3::zero;
			float4x4 local_matrix = float4x4::zero;

			SERIALIZE_FIELD(GameObject*, _objective);
			SERIALIZE_FIELD(bool, _do_look_ahead);
			SERIALIZE_FIELD(bool, _do_mouse_movement);
			SERIALIZE_FIELD(float, _speed);
			SERIALIZE_FIELD(float, _duration);
			SERIALIZE_FIELD(float3, _relative_position);
			SERIALIZE_FIELD(float3, _rotation);


			SERIALIZE_FIELD(bool, _check_box);
			bool InsideOBB();
		};
	}
}