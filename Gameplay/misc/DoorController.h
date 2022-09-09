#pragma once

#include <scripting/Script.h>

namespace Hachiko
{
	class GameObject;
	class ComponentTransform;
	namespace Scripting
	{
		class DoorController : public Script
		{
			SERIALIZATION_METHODS(false)

				enum State
			{
				CLOSED = 0,
				OPEN = 1
			};

		// Methods
		public:
			DoorController(GameObject* game_object);
			~DoorController() override = default;

			void OnAwake() override;
			void OnUpdate() override;

			void Open();
			void Close();

		private:
			void UpdateDoorState();

		// Variables
		public:
			SERIALIZE_FIELD(GameObject*, _door_prefab);
			GameObject* _closed_door_mesh = nullptr;
			GameObject* _open_door_mesh = nullptr;
			ComponentObstacle* door_obstacle = nullptr;

		private:
			State _prev_state = State::OPEN;
			State _state = State::OPEN;
		};
	}
	
}



