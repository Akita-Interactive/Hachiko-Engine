#pragma once
#include <scripting/Script.h>
#include "entities/Stats.h"
#include "entities/player/PlayerController.h"

namespace Hachiko
{
	class ComponentButton;
	class ComponentText;
	class GameObject;

	namespace Scripting
	{
		class PlayerController;
		class DebugManager : public Script
		{
			SERIALIZATION_METHODS(false)

		public:
			DebugManager(GameObject* game_object);
			~DebugManager() override = default;

			void OnAwake() override;
			void OnStart() override;
			void OnUpdate() override;
		private:
			void HandleButtonInteraction();

		public:
			bool is_active;
			
		private:
			SERIALIZE_FIELD(GameObject*, _player);

			// BUTTONS
			SERIALIZE_FIELD(ComponentButton*, _button_back);
			SERIALIZE_FIELD(ComponentButton*, _teleport_next_pos);
			SERIALIZE_FIELD(ComponentButton*, _teleport_prev_pos);
			SERIALIZE_FIELD(ComponentButton*, _teleport_add_pos);
			SERIALIZE_FIELD(ComponentButton*, _add_health);
			SERIALIZE_FIELD(ComponentButton*, _remove_health);
			SERIALIZE_FIELD(ComponentButton*, _increase_max_hp);
			SERIALIZE_FIELD(ComponentButton*, _decrease_max_hp);
			SERIALIZE_FIELD(ComponentButton*, _increase_move_speed);
			SERIALIZE_FIELD(ComponentButton*, _decrease_move_speed);
			SERIALIZE_FIELD(ComponentButton*, _increase_attack_cd);
			SERIALIZE_FIELD(ComponentButton*, _decrease_attack_cd);
			SERIALIZE_FIELD(ComponentButton*, _increase_attack_power);
			SERIALIZE_FIELD(ComponentButton*, _decrease_attack_power);
			SERIALIZE_FIELD(ComponentButton*, _god_mode);
			SERIALIZE_FIELD(ComponentButton*, _spawn_enemy);
			SERIALIZE_FIELD(ComponentButton*, _unlock_skills);
			SERIALIZE_FIELD(ComponentButton*, _toggle_performance_output);
			SERIALIZE_FIELD(ComponentButton*, _toggle_vsync);
			SERIALIZE_FIELD(ComponentButton*, _toggle_wireframe);
			SERIALIZE_FIELD(ComponentButton*, _toggle_show_colliders);
			SERIALIZE_FIELD(ComponentButton*, _exit_debug);

			// TEXT
			SERIALIZE_FIELD(ComponentText*, _text_fps);
			SERIALIZE_FIELD(ComponentText*, _text_ms);

			// DEFAULT POSITIONS
			SERIALIZE_FIELD(GameObject*, _tp_pos1);
			SERIALIZE_FIELD(GameObject*, _tp_pos2);
			SERIALIZE_FIELD(GameObject*, _tp_pos3);

			SERIALIZE_FIELD(GameObject*, _performance_menu);

			std::vector<math::float3> teleport_positions;
			int teleport_iterator = -1;
			PlayerController* _player_controller;
			bool is_wireframe = false;
			bool is_performance = false;
			bool is_vsync = false;
			bool is_navmesh = false;
		};
	} // namespace Scripting
} // namespace Hachiko