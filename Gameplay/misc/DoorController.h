#pragma once

#include <scripting/Script.h>

namespace Hachiko
{
	class GameObject;
	class ComponentTransform;
	namespace Scripting
	{
		class AudioManager;

		class DoorController : public Script
		{
			SERIALIZATION_METHODS(false)

			enum State
			{
				CLOSED = 0,
				OPENNING = 1,
				OPEN = 2
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
			SERIALIZE_FIELD(GameObject*, _audio_manager_go);
			GameObject* _closed_door_mesh = nullptr;
			GameObject* _open_door_mesh = nullptr;
			ComponentObstacle* door_obstacle = nullptr;
			AudioManager* _audio_manager = nullptr;

		private:
			State _prev_state = State::OPEN;
			State _state = State::OPEN;

			float _elapsed_time = 0.0f;
		};
	}
	
}



