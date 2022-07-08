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
			GameObject* _player;
			PlayerCamera* _camera;
			bool _is_inside = false;
			SERIALIZE_FIELD(GameObject*, _objective);
			SERIALIZE_FIELD(bool, _do_look_ahead);
			SERIALIZE_FIELD(float, _speed);
			SERIALIZE_FIELD(float, _duration);
			SERIALIZE_FIELD(float3, _relative_position);
			SERIALIZE_FIELD(float3, _rotation);
		};
	}
}