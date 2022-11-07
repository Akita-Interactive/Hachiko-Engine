#include "scriptingUtil/gameplaypch.h"

#include "constants/Scenes.h"
#include "entities/player/PlayerController.h"
#include "ui/DebugManager.h"

#include <Algorithm/Random/LCG.h>

constexpr int MAX_AMMO = 4;

Hachiko::Scripting::DebugManager::DebugManager(GameObject* game_object)
	: Script(game_object, "DebugManager")
	, _player(nullptr)
	, _button_back(nullptr)
	, _teleport_next_pos(nullptr)
	, _teleport_prev_pos(nullptr)
	, _teleport_add_pos(nullptr)
	, _add_health(nullptr)
	, _remove_health(nullptr)
	, _increase_move_speed(nullptr)
	, _decrease_move_speed(nullptr)
	, _increase_attack_power(nullptr)
	, _decrease_attack_power(nullptr)
	, _god_mode(nullptr)
	, _weapon_claws(nullptr)
	, _weapon_sword(nullptr)
	, _weapon_hammer(nullptr)
	, _reload_ammo(nullptr)
	, _toggle_performance_output(nullptr)
	, _performance_menu(nullptr)
	, _player_stats(nullptr)
	, _toggle_vsync(nullptr)
	, _text_fps(nullptr)
	, _text_ms(nullptr)
	, _toggle_wireframe(nullptr)
	, _toggle_show_colliders(nullptr)
	, _exit_debug(nullptr)
	, _tp_pos1(nullptr)
	, _tp_pos2(nullptr)
	, _tp_pos3(nullptr)
{
}

void Hachiko::Scripting::DebugManager::OnAwake()
{
	is_active = false;
	is_wireframe = false;
	is_performance = false;
	is_navmesh = false;
	show_player_stats = false;

	if (_tp_pos1)
	{
		teleport_positions.push_back(_tp_pos1->GetTransform()->GetGlobalPosition());
	}
	if (_tp_pos2)
	{
		teleport_positions.push_back(_tp_pos2->GetTransform()->GetGlobalPosition());
	}
	if (_tp_pos3)
	{
		teleport_positions.push_back(_tp_pos3->GetTransform()->GetGlobalPosition());
	}

}

void Hachiko::Scripting::DebugManager::OnStart()
{
	_player_controller = _player->GetComponent<PlayerController>();
	is_vsync = Debug::GetVsync();
	_performance_menu->SetActive(is_performance);
	_player_stats->SetActive(show_player_stats);

	for (GameObject* child : game_object->children)
	{
		child->SetActive(is_active);
	}
	if (_button_back == nullptr)
	{
		HE_LOG("button back is NULL");
	}

}

void Hachiko::Scripting::DebugManager::OnUpdate()
{

	if (Input::IsKeyDown(Input::KeyCode::KEY_F2))
	{
		_player_controller->_isInDebug = !_player_controller->_isInDebug;
		is_active = !is_active;
		for (GameObject* child : game_object->children)
		{
			child->SetActive(is_active);
		}
		show_player_stats = !show_player_stats;
		_player_stats->SetActive(show_player_stats);
	}

	if (is_performance)
	{
		std::string fps = std::to_string(int(Debug::GetFps()));
		std::string ms = std::to_string(Debug::GetMs());

		_text_fps->SetText(fps.c_str());
		_text_ms->SetText(ms.c_str());
	}

	if (show_player_stats)
	{
		std::string attack = std::to_string(int(_player_controller->_combat_stats->_attack_power));
		std::string mvspeed = std::to_string(int(_player_controller->_combat_stats->_move_speed));

		_text_atck->SetText(attack.c_str());
		_text_mvspd->SetText(mvspeed.c_str());
	}

	if (!is_active)
	{
		return;
	}

	// register button interactions
	HandleButtonInteraction();

}

void Hachiko::Scripting::DebugManager::HandleButtonInteraction()
{
	if (_button_back->IsSelected())
	{
		HE_LOG("_button_back pressed");
		SceneManagement::SwitchScene(Scenes::MAIN_MENU);
	}

	if (_teleport_next_pos->IsSelected())
	{
		HE_LOG("_teleport_next_pos pressed");
		if (teleport_positions.size() == 0)
		{
			HE_LOG("ERROR: teleport_positions vector is empty");
		}
		else
		{
			++teleport_iterator;
			if (teleport_iterator >= teleport_positions.size())
			{
				teleport_iterator = 0;
			}
			float3 current_position = teleport_positions[teleport_iterator];
			_player->GetTransform()->SetGlobalPosition(current_position);


			HE_LOG("Iterator: %d", teleport_iterator);
		}

	}
	if (_teleport_prev_pos->IsSelected())
	{
		HE_LOG("_teleport_prev_pos pressed");
		if (teleport_positions.size() == 0)
		{
			HE_LOG("ERROR: teleport_positions vector is empty");
		}
		else
		{
			--teleport_iterator;
			if (teleport_iterator < 0)
			{
				teleport_iterator = teleport_positions.size() - 1;
			}
			float3 current_position = teleport_positions[teleport_iterator];
			_player->GetTransform()->SetGlobalPosition(current_position);

			HE_LOG("Iterator: %d", teleport_iterator);
		}

	}
	if (_teleport_add_pos->IsSelected())
	{
		teleport_positions.push_back(_player->GetTransform()->GetGlobalPosition());
	}

	// Edit Character Stats

	if (_add_health->IsSelected())
	{
		_player_controller->_combat_stats->Heal(1);
		HE_LOG("Health now %i", _player_controller->_combat_stats->_current_hp);
		_player_controller->UpdateHealthBar();
	}
	if (_remove_health->IsSelected())
	{
		_player_controller->_combat_stats->ReceiveDamage(1);
		HE_LOG("Health now %i", _player_controller->_combat_stats->_current_hp);
		_player_controller->UpdateHealthBar();
	}

	if (_increase_move_speed->IsSelected())
	{
		_player_controller->_combat_stats->_move_speed += 1.0f;
		HE_LOG("Move speed now %f", _player_controller->_combat_stats->_move_speed);
	}
	if (_decrease_move_speed->IsSelected() && _player_controller->_combat_stats->_move_speed > 1)
	{
		_player_controller->_combat_stats->_move_speed -= 1.0f;
		HE_LOG("Move speed now %f", _player_controller->_combat_stats->_move_speed);
	}

	if (_increase_attack_power->IsSelected())
	{
		_player_controller->_combat_stats->_attack_power += 1;
		HE_LOG("Attack power now %d", _player_controller->_combat_stats->_attack_power);
	}
	if (_decrease_attack_power->IsSelected() && _player_controller->_combat_stats->_attack_power > 1)
	{
		_player_controller->_combat_stats->_attack_power -= 1;
		HE_LOG("Attack power now %d", _player_controller->_combat_stats->_attack_power);
	}

	if (_reload_ammo->IsSelected())
	{
		_player_controller->ReloadAmmo(MAX_AMMO);
	}

	// GOD MODE //
	if (_god_mode->IsSelected())
	{
		HE_LOG("GOD MODE pressed");
		_player_controller->_god_mode_trigger = true;
	}

	// WEAPON SELECTION //

	if (_weapon_claws->IsSelected())
	{
		HE_LOG("_weapon_claws pressed");
		unsigned int weapon_idx = 1;
		_player_controller->ChangeWeapon(weapon_idx);

	}

	if (_weapon_sword->IsSelected())
	{
		HE_LOG("_weapon_sword pressed");
		unsigned int weapon_idx = 2;
		_player_controller->ChangeWeapon(weapon_idx);
	}

	if (_weapon_hammer->IsSelected())
	{
		HE_LOG("_weapon_hammer pressed");
		unsigned int weapon_idx = 3;
		_player_controller->ChangeWeapon(weapon_idx);
	}

	// PERFORMANCE //

	if (_toggle_performance_output->IsSelected())
	{
		is_performance = !is_performance;
		_performance_menu->SetActive(is_performance);
	}

	if (_toggle_vsync->IsSelected())
	{
		is_vsync = !is_vsync;
		Debug::SetVsync(is_vsync);
	}

	if (_toggle_show_colliders->IsSelected())
	{
		HE_LOG("_toggle_show_colliders pressed");
		is_navmesh = !is_navmesh;
		Debug::DrawNavmesh(is_navmesh);
	}

	if (_toggle_wireframe->IsSelected())
	{
		is_wireframe = !is_wireframe;
		Debug::SetPolygonMode(!is_wireframe);
	}




	if (_exit_debug->IsSelected())
	{
		is_active = !is_active;
		for (GameObject* child : game_object->children)
		{
			_player_controller->_isInDebug = !_player_controller->_isInDebug;
			child->SetActive(is_active);
		}
		show_player_stats = !show_player_stats;
		_player_stats->SetActive(show_player_stats);
	}
}