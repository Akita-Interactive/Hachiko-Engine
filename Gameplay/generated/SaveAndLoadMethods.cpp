#include "scriptingUtil/gameplaypch.h"
#include <yaml-cpp/yaml.h>
#include <core/serialization/TypeConverter.h>
#include "TriggerAnim.h"
#include "entities/Stats.h"
#include "entities/crystals/CrystalExplosion.h"
#include "entities/enemies/BugAnimationManager.h"
#include "entities/enemies/EnemyController.h"
#include "entities/player/CombatManager.h"
#include "entities/player/CombatVisualEffectsPool.h"
#include "entities/player/PlayerAnimationManager.h"
#include "entities/player/PlayerCamera.h"
#include "entities/player/PlayerController.h"
#include "entities/player/PlayerSoundManager.h"
#include "entities/player/RoomTeleporter.h"
#include "misc/AudioManager.h"
#include "misc/BlinkingLight.h"
#include "misc/CameraPosChange.h"
#include "misc/CrystalPlatform.h"
#include "misc/DoorController.h"
#include "misc/DynamicCamera.h"
#include "misc/FancyLights.h"
#include "misc/GauntletManager.h"
#include "misc/LaserController.h"
#include "misc/LevelManager.h"
#include "misc/PillarCheckpoint.h"
#include "misc/Spawner.h"
#include "ui/BackToMainMenu.h"
#include "ui/DebugManager.h"
#include "ui/MainMenuManager.h"



void Hachiko::Scripting::TriggerAnim::OnSave(YAML::Node& node) const
{
}

void Hachiko::Scripting::TriggerAnim::OnLoad()
{
}

void Hachiko::Scripting::Stats::OnSave(YAML::Node& node) const
{
	node["'_attack_power@int'"] = _attack_power;

	node["'_attack_cd@float'"] = _attack_cd;

	node["'_attack_range@float'"] = _attack_range;

	node["'_move_speed@float'"] = _move_speed;

	node["'_max_hp@int'"] = _max_hp;

	node["'_current_hp@int'"] = _current_hp;
}

void Hachiko::Scripting::Stats::OnLoad()
{
	if (load_node["'_attack_power@int'"].IsDefined())
	{
		_attack_power = load_node["'_attack_power@int'"].as<int>();
	}

	if (load_node["'_attack_cd@float'"].IsDefined())
	{
		_attack_cd = load_node["'_attack_cd@float'"].as<float>();
	}

	if (load_node["'_attack_range@float'"].IsDefined())
	{
		_attack_range = load_node["'_attack_range@float'"].as<float>();
	}

	if (load_node["'_move_speed@float'"].IsDefined())
	{
		_move_speed = load_node["'_move_speed@float'"].as<float>();
	}

	if (load_node["'_max_hp@int'"].IsDefined())
	{
		_max_hp = load_node["'_max_hp@int'"].as<int>();
	}

	if (load_node["'_current_hp@int'"].IsDefined())
	{
		_current_hp = load_node["'_current_hp@int'"].as<int>();
	}
}

void Hachiko::Scripting::CrystalExplosion::OnSave(YAML::Node& node) const
{
	if (_explosion_crystal != nullptr)
	{
		node["'_explosion_crystal@GameObject*'"] = _explosion_crystal->GetID();
	}
	else
	{
		node["'_explosion_crystal@GameObject*'"] = 0;
	}

	if (_static_crystal != nullptr)
	{
		node["'_static_crystal@GameObject*'"] = _static_crystal->GetID();
	}
	else
	{
		node["'_static_crystal@GameObject*'"] = 0;
	}

	if (_explosion_indicator_helper != nullptr)
	{
		node["'_explosion_indicator_helper@GameObject*'"] = _explosion_indicator_helper->GetID();
	}
	else
	{
		node["'_explosion_indicator_helper@GameObject*'"] = 0;
	}

	if (_explosion_effect != nullptr)
	{
		node["'_explosion_effect@GameObject*'"] = _explosion_effect->GetID();
	}
	else
	{
		node["'_explosion_effect@GameObject*'"] = 0;
	}

	node["'_crashing_index@unsigned'"] = _crashing_index;

	node["'_detecting_radius@float'"] = _detecting_radius;

	node["'_explosion_radius@float'"] = _explosion_radius;

	node["'_timer_explosion@float'"] = _timer_explosion;

	node["'_explosive_crystal@bool'"] = _explosive_crystal;

	node["'_regen_time@float'"] = _regen_time;
}

void Hachiko::Scripting::CrystalExplosion::OnLoad()
{
	if (load_node["'_explosion_crystal@GameObject*'"].IsDefined())
	{
		_explosion_crystal = SceneManagement::FindInCurrentScene(load_node["'_explosion_crystal@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_static_crystal@GameObject*'"].IsDefined())
	{
		_static_crystal = SceneManagement::FindInCurrentScene(load_node["'_static_crystal@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_explosion_indicator_helper@GameObject*'"].IsDefined())
	{
		_explosion_indicator_helper = SceneManagement::FindInCurrentScene(load_node["'_explosion_indicator_helper@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_explosion_effect@GameObject*'"].IsDefined())
	{
		_explosion_effect = SceneManagement::FindInCurrentScene(load_node["'_explosion_effect@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_crashing_index@unsigned'"].IsDefined())
	{
		_crashing_index = load_node["'_crashing_index@unsigned'"].as<unsigned>();
	}

	if (load_node["'_detecting_radius@float'"].IsDefined())
	{
		_detecting_radius = load_node["'_detecting_radius@float'"].as<float>();
	}

	if (load_node["'_explosion_radius@float'"].IsDefined())
	{
		_explosion_radius = load_node["'_explosion_radius@float'"].as<float>();
	}

	if (load_node["'_timer_explosion@float'"].IsDefined())
	{
		_timer_explosion = load_node["'_timer_explosion@float'"].as<float>();
	}

	if (load_node["'_explosive_crystal@bool'"].IsDefined())
	{
		_explosive_crystal = load_node["'_explosive_crystal@bool'"].as<bool>();
	}

	if (load_node["'_regen_time@float'"].IsDefined())
	{
		_regen_time = load_node["'_regen_time@float'"].as<float>();
	}
}

void Hachiko::Scripting::BugAnimationManager::OnSave(YAML::Node& node) const
{
	if (_animator != nullptr && _animator->GetGameObject() != nullptr)
	{
		node["'_animator@ComponentAnimation*'"] = _animator->GetGameObject()->GetID();
	}
	else
	{
		node["'_animator@ComponentAnimation*'"] = 0;
	}

	node["'_state_string@std::string'"] = _state_string;

	node["'_idle_index@unsigned'"] = _idle_index;

	node["'_attacking_index@unsigned'"] = _attacking_index;
}

void Hachiko::Scripting::BugAnimationManager::OnLoad()
{
	if (load_node["'_animator@ComponentAnimation*'"].IsDefined())
	{
		GameObject* _animator_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_animator@ComponentAnimation*'"].as<unsigned long long>());
		if (_animator_owner__temp != nullptr)
		{
			_animator = _animator_owner__temp->GetComponent<ComponentAnimation>();
		}
	}

	if (load_node["'_state_string@std::string'"].IsDefined())
	{
		_state_string = load_node["'_state_string@std::string'"].as<std::string>();
	}

	if (load_node["'_idle_index@unsigned'"].IsDefined())
	{
		_idle_index = load_node["'_idle_index@unsigned'"].as<unsigned>();
	}

	if (load_node["'_attacking_index@unsigned'"].IsDefined())
	{
		_attacking_index = load_node["'_attacking_index@unsigned'"].as<unsigned>();
	}
}

void Hachiko::Scripting::EnemyController::OnSave(YAML::Node& node) const
{
	node["'_worm@bool'"] = _worm;

	node["'_aggro_range@int'"] = _aggro_range;

	node["'_attack_range@float'"] = _attack_range;

	node["'_attack_delay@float'"] = _attack_delay;

	node["'_idle_cooldown@float'"] = _idle_cooldown;

	node["'_spawning_time@float'"] = _spawning_time;

	node["'_spawn_pos@float3'"] = _spawn_pos;

	node["'_spawn_is_initial@bool'"] = _spawn_is_initial;

	if (_enemy_body != nullptr)
	{
		node["'_enemy_body@GameObject*'"] = _enemy_body->GetID();
	}
	else
	{
		node["'_enemy_body@GameObject*'"] = 0;
	}

	if (_parasite != nullptr)
	{
		node["'_parasite@GameObject*'"] = _parasite->GetID();
	}
	else
	{
		node["'_parasite@GameObject*'"] = 0;
	}

	if (_blood_trail != nullptr)
	{
		node["'_blood_trail@GameObject*'"] = _blood_trail->GetID();
	}
	else
	{
		node["'_blood_trail@GameObject*'"] = 0;
	}

	if (_small_dust != nullptr)
	{
		node["'_small_dust@GameObject*'"] = _small_dust->GetID();
	}
	else
	{
		node["'_small_dust@GameObject*'"] = 0;
	}

	if (_big_dust != nullptr)
	{
		node["'_big_dust@GameObject*'"] = _big_dust->GetID();
	}
	else
	{
		node["'_big_dust@GameObject*'"] = 0;
	}

	if (_attack_zone != nullptr)
	{
		node["'_attack_zone@GameObject*'"] = _attack_zone->GetID();
	}
	else
	{
		node["'_attack_zone@GameObject*'"] = 0;
	}

	if (_inner_indicator != nullptr)
	{
		node["'_inner_indicator@GameObject*'"] = _inner_indicator->GetID();
	}
	else
	{
		node["'_inner_indicator@GameObject*'"] = 0;
	}

	if (_outer_indicator != nullptr)
	{
		node["'_outer_indicator@GameObject*'"] = _outer_indicator->GetID();
	}
	else
	{
		node["'_outer_indicator@GameObject*'"] = 0;
	}

	node["'_already_in_combat@bool'"] = _already_in_combat;

	node["'_is_from_gautlet@bool'"] = _is_from_gautlet;

	node["'_will_die@bool'"] = _will_die;

	node["'_attack_animation_duration@float'"] = _attack_animation_duration;

	node["'_attack_animation_timer@float'"] = _attack_animation_timer;

	node["'_current_spawning_time@float'"] = _current_spawning_time;
}

void Hachiko::Scripting::EnemyController::OnLoad()
{
	if (load_node["'_worm@bool'"].IsDefined())
	{
		_worm = load_node["'_worm@bool'"].as<bool>();
	}

	if (load_node["'_aggro_range@int'"].IsDefined())
	{
		_aggro_range = load_node["'_aggro_range@int'"].as<int>();
	}

	if (load_node["'_attack_range@float'"].IsDefined())
	{
		_attack_range = load_node["'_attack_range@float'"].as<float>();
	}

	if (load_node["'_attack_delay@float'"].IsDefined())
	{
		_attack_delay = load_node["'_attack_delay@float'"].as<float>();
	}

	if (load_node["'_idle_cooldown@float'"].IsDefined())
	{
		_idle_cooldown = load_node["'_idle_cooldown@float'"].as<float>();
	}

	if (load_node["'_spawning_time@float'"].IsDefined())
	{
		_spawning_time = load_node["'_spawning_time@float'"].as<float>();
	}

	if (load_node["'_spawn_pos@float3'"].IsDefined())
	{
		_spawn_pos = load_node["'_spawn_pos@float3'"].as<float3>();
	}

	if (load_node["'_spawn_is_initial@bool'"].IsDefined())
	{
		_spawn_is_initial = load_node["'_spawn_is_initial@bool'"].as<bool>();
	}

	if (load_node["'_enemy_body@GameObject*'"].IsDefined())
	{
		_enemy_body = SceneManagement::FindInCurrentScene(load_node["'_enemy_body@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_parasite@GameObject*'"].IsDefined())
	{
		_parasite = SceneManagement::FindInCurrentScene(load_node["'_parasite@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_blood_trail@GameObject*'"].IsDefined())
	{
		_blood_trail = SceneManagement::FindInCurrentScene(load_node["'_blood_trail@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_small_dust@GameObject*'"].IsDefined())
	{
		_small_dust = SceneManagement::FindInCurrentScene(load_node["'_small_dust@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_big_dust@GameObject*'"].IsDefined())
	{
		_big_dust = SceneManagement::FindInCurrentScene(load_node["'_big_dust@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_attack_zone@GameObject*'"].IsDefined())
	{
		_attack_zone = SceneManagement::FindInCurrentScene(load_node["'_attack_zone@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_inner_indicator@GameObject*'"].IsDefined())
	{
		_inner_indicator = SceneManagement::FindInCurrentScene(load_node["'_inner_indicator@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_outer_indicator@GameObject*'"].IsDefined())
	{
		_outer_indicator = SceneManagement::FindInCurrentScene(load_node["'_outer_indicator@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_already_in_combat@bool'"].IsDefined())
	{
		_already_in_combat = load_node["'_already_in_combat@bool'"].as<bool>();
	}

	if (load_node["'_is_from_gautlet@bool'"].IsDefined())
	{
		_is_from_gautlet = load_node["'_is_from_gautlet@bool'"].as<bool>();
	}

	if (load_node["'_will_die@bool'"].IsDefined())
	{
		_will_die = load_node["'_will_die@bool'"].as<bool>();
	}

	if (load_node["'_attack_animation_duration@float'"].IsDefined())
	{
		_attack_animation_duration = load_node["'_attack_animation_duration@float'"].as<float>();
	}

	if (load_node["'_attack_animation_timer@float'"].IsDefined())
	{
		_attack_animation_timer = load_node["'_attack_animation_timer@float'"].as<float>();
	}

	if (load_node["'_current_spawning_time@float'"].IsDefined())
	{
		_current_spawning_time = load_node["'_current_spawning_time@float'"].as<float>();
	}
}

void Hachiko::Scripting::CombatManager::OnSave(YAML::Node& node) const
{
	if (_charge_vfx != nullptr)
	{
		node["'_charge_vfx@GameObject*'"] = _charge_vfx->GetID();
	}
	else
	{
		node["'_charge_vfx@GameObject*'"] = 0;
	}

	if (_shot_vfx != nullptr)
	{
		node["'_shot_vfx@GameObject*'"] = _shot_vfx->GetID();
	}
	else
	{
		node["'_shot_vfx@GameObject*'"] = 0;
	}
}

void Hachiko::Scripting::CombatManager::OnLoad()
{
	if (load_node["'_charge_vfx@GameObject*'"].IsDefined())
	{
		_charge_vfx = SceneManagement::FindInCurrentScene(load_node["'_charge_vfx@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_shot_vfx@GameObject*'"].IsDefined())
	{
		_shot_vfx = SceneManagement::FindInCurrentScene(load_node["'_shot_vfx@GameObject*'"].as<unsigned long long>());
	}
}

void Hachiko::Scripting::CombatVisualEffectsPool::OnSave(YAML::Node& node) const
{
	node["'_current_billboard_index@int'"] = _current_billboard_index;
}

void Hachiko::Scripting::CombatVisualEffectsPool::OnLoad()
{
	if (load_node["'_current_billboard_index@int'"].IsDefined())
	{
		_current_billboard_index = load_node["'_current_billboard_index@int'"].as<int>();
	}
}

void Hachiko::Scripting::PlayerAnimationManager::OnSave(YAML::Node& node) const
{
	if (_animator != nullptr && _animator->GetGameObject() != nullptr)
	{
		node["'_animator@ComponentAnimation*'"] = _animator->GetGameObject()->GetID();
	}
	else
	{
		node["'_animator@ComponentAnimation*'"] = 0;
	}

	node["'_state_string@std::string'"] = _state_string;

	node["'_idle_index@unsigned'"] = _idle_index;

	node["'_walking_index@unsigned'"] = _walking_index;

	node["'_dashing_index@unsigned'"] = _dashing_index;

	node["'_melee_index@unsigned'"] = _melee_index;

	node["'_ranged_index@unsigned'"] = _ranged_index;
}

void Hachiko::Scripting::PlayerAnimationManager::OnLoad()
{
	if (load_node["'_animator@ComponentAnimation*'"].IsDefined())
	{
		GameObject* _animator_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_animator@ComponentAnimation*'"].as<unsigned long long>());
		if (_animator_owner__temp != nullptr)
		{
			_animator = _animator_owner__temp->GetComponent<ComponentAnimation>();
		}
	}

	if (load_node["'_state_string@std::string'"].IsDefined())
	{
		_state_string = load_node["'_state_string@std::string'"].as<std::string>();
	}

	if (load_node["'_idle_index@unsigned'"].IsDefined())
	{
		_idle_index = load_node["'_idle_index@unsigned'"].as<unsigned>();
	}

	if (load_node["'_walking_index@unsigned'"].IsDefined())
	{
		_walking_index = load_node["'_walking_index@unsigned'"].as<unsigned>();
	}

	if (load_node["'_dashing_index@unsigned'"].IsDefined())
	{
		_dashing_index = load_node["'_dashing_index@unsigned'"].as<unsigned>();
	}

	if (load_node["'_melee_index@unsigned'"].IsDefined())
	{
		_melee_index = load_node["'_melee_index@unsigned'"].as<unsigned>();
	}

	if (load_node["'_ranged_index@unsigned'"].IsDefined())
	{
		_ranged_index = load_node["'_ranged_index@unsigned'"].as<unsigned>();
	}
}

void Hachiko::Scripting::PlayerCamera::OnSave(YAML::Node& node) const
{
	node["'_relative_position_to_player@math::float3'"] = _relative_position_to_player;

	if (_objective != nullptr)
	{
		node["'_objective@GameObject*'"] = _objective->GetID();
	}
	else
	{
		node["'_objective@GameObject*'"] = 0;
	}

	node["'_follow_delay@float'"] = _follow_delay;
}

void Hachiko::Scripting::PlayerCamera::OnLoad()
{
	if (load_node["'_relative_position_to_player@math::float3'"].IsDefined())
	{
		_relative_position_to_player = load_node["'_relative_position_to_player@math::float3'"].as<math::float3>();
	}

	if (load_node["'_objective@GameObject*'"].IsDefined())
	{
		_objective = SceneManagement::FindInCurrentScene(load_node["'_objective@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_follow_delay@float'"].IsDefined())
	{
		_follow_delay = load_node["'_follow_delay@float'"].as<float>();
	}
}

void Hachiko::Scripting::PlayerController::OnSave(YAML::Node& node) const
{

	if (_sword_weapon != nullptr)
	{
		node["'_sword_weapon@GameObject*'"] = _sword_weapon->GetID();
	}
	else
	{
		node["'_sword_weapon@GameObject*'"] = 0;
	}

	if (_sword_upper != nullptr)
	{
		node["'_sword_upper@GameObject*'"] = _sword_upper->GetID();
	}
	else
	{
		node["'_sword_upper@GameObject*'"] = 0;
	}

	if (_claw_weapon != nullptr)
	{
		node["'_claw_weapon@GameObject*'"] = _claw_weapon->GetID();
	}
	else
	{
		node["'_claw_weapon@GameObject*'"] = 0;
	}

	if (_attack_indicator != nullptr)
	{
		node["'_attack_indicator@GameObject*'"] = _attack_indicator->GetID();
	}
	else
	{
		node["'_attack_indicator@GameObject*'"] = 0;
	}

	if (_bullet_emitter != nullptr)
	{
		node["'_bullet_emitter@GameObject*'"] = _bullet_emitter->GetID();
	}
	else
	{
		node["'_bullet_emitter@GameObject*'"] = 0;
	}

	if (_goal != nullptr)
	{
		node["'_goal@GameObject*'"] = _goal->GetID();
	}
	else
	{
		node["'_goal@GameObject*'"] = 0;
	}

	if (_player_geometry != nullptr)
	{
		node["'_player_geometry@GameObject*'"] = _player_geometry->GetID();
	}
	else
	{
		node["'_player_geometry@GameObject*'"] = 0;
	}

	node["'_dash_duration@float'"] = _dash_duration;

	node["'_dash_distance@float'"] = _dash_distance;

	node["'_dash_cooldown@float'"] = _dash_cooldown;

	node["'_invulnerability_time@float'"] = _invulnerability_time;

	node["'_dash_scaler@unsigned'"] = _dash_scaler;

	node["'_max_dash_charges@unsigned'"] = _max_dash_charges;

	if (_dash_trail != nullptr)
	{
		node["'_dash_trail@GameObject*'"] = _dash_trail->GetID();
	}
	else
	{
		node["'_dash_trail@GameObject*'"] = 0;
	}

	node["'_trail_enlarger@float'"] = _trail_enlarger;

	if (_falling_dust != nullptr)
	{
		node["'_falling_dust@GameObject*'"] = _falling_dust->GetID();
	}
	else
	{
		node["'_falling_dust@GameObject*'"] = 0;
	}

	if (_walking_dust != nullptr)
	{
		node["'_walking_dust@GameObject*'"] = _walking_dust->GetID();
	}
	else
	{
		node["'_walking_dust@GameObject*'"] = 0;
	}

	if (_heal_effect != nullptr)
	{
		node["'_heal_effect@GameObject*'"] = _heal_effect->GetID();
	}
	else
	{
		node["'_heal_effect@GameObject*'"] = 0;
	}

	node["'_rotation_duration@float'"] = _rotation_duration;

	if (_hp_cell_1 != nullptr)
	{
		node["'_hp_cell_1@GameObject*'"] = _hp_cell_1->GetID();
	}
	else
	{
		node["'_hp_cell_1@GameObject*'"] = 0;
	}

	if (_hp_cell_2 != nullptr)
	{
		node["'_hp_cell_2@GameObject*'"] = _hp_cell_2->GetID();
	}
	else
	{
		node["'_hp_cell_2@GameObject*'"] = 0;
	}

	if (_hp_cell_3 != nullptr)
	{
		node["'_hp_cell_3@GameObject*'"] = _hp_cell_3->GetID();
	}
	else
	{
		node["'_hp_cell_3@GameObject*'"] = 0;
	}

	if (_hp_cell_4 != nullptr)
	{
		node["'_hp_cell_4@GameObject*'"] = _hp_cell_4->GetID();
	}
	else
	{
		node["'_hp_cell_4@GameObject*'"] = 0;
	}

	if (_ammo_cell_1 != nullptr)
	{
		node["'_ammo_cell_1@GameObject*'"] = _ammo_cell_1->GetID();
	}
	else
	{
		node["'_ammo_cell_1@GameObject*'"] = 0;
	}

	if (_ammo_cell_2 != nullptr)
	{
		node["'_ammo_cell_2@GameObject*'"] = _ammo_cell_2->GetID();
	}
	else
	{
		node["'_ammo_cell_2@GameObject*'"] = 0;
	}

	if (_ammo_cell_3 != nullptr)
	{
		node["'_ammo_cell_3@GameObject*'"] = _ammo_cell_3->GetID();
	}
	else
	{
		node["'_ammo_cell_3@GameObject*'"] = 0;
	}

	if (_ammo_cell_4 != nullptr)
	{
		node["'_ammo_cell_4@GameObject*'"] = _ammo_cell_4->GetID();
	}
	else
	{
		node["'_ammo_cell_4@GameObject*'"] = 0;
	}

	if (_weapon_charge_bar_go != nullptr)
	{
		node["'_weapon_charge_bar_go@GameObject*'"] = _weapon_charge_bar_go->GetID();
	}
	else
	{
		node["'_weapon_charge_bar_go@GameObject*'"] = 0;
	}

	if (_camera != nullptr)
	{
		node["'_camera@GameObject*'"] = _camera->GetID();
	}
	else
	{
		node["'_camera@GameObject*'"] = 0;
	}

	if (_ui_damage != nullptr)
	{
		node["'_ui_damage@GameObject*'"] = _ui_damage->GetID();
	}
	else
	{
		node["'_ui_damage@GameObject*'"] = 0;
	}
}

void Hachiko::Scripting::PlayerController::OnLoad()
{

	if (load_node["'_sword_weapon@GameObject*'"].IsDefined())
	{
		_sword_weapon = SceneManagement::FindInCurrentScene(load_node["'_sword_weapon@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_sword_upper@GameObject*'"].IsDefined())
	{
		_sword_upper = SceneManagement::FindInCurrentScene(load_node["'_sword_upper@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_claw_weapon@GameObject*'"].IsDefined())
	{
		_claw_weapon = SceneManagement::FindInCurrentScene(load_node["'_claw_weapon@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_attack_indicator@GameObject*'"].IsDefined())
	{
		_attack_indicator = SceneManagement::FindInCurrentScene(load_node["'_attack_indicator@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_bullet_emitter@GameObject*'"].IsDefined())
	{
		_bullet_emitter = SceneManagement::FindInCurrentScene(load_node["'_bullet_emitter@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_goal@GameObject*'"].IsDefined())
	{
		_goal = SceneManagement::FindInCurrentScene(load_node["'_goal@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_player_geometry@GameObject*'"].IsDefined())
	{
		_player_geometry = SceneManagement::FindInCurrentScene(load_node["'_player_geometry@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_dash_duration@float'"].IsDefined())
	{
		_dash_duration = load_node["'_dash_duration@float'"].as<float>();
	}

	if (load_node["'_dash_distance@float'"].IsDefined())
	{
		_dash_distance = load_node["'_dash_distance@float'"].as<float>();
	}

	if (load_node["'_dash_cooldown@float'"].IsDefined())
	{
		_dash_cooldown = load_node["'_dash_cooldown@float'"].as<float>();
	}

	if (load_node["'_invulnerability_time@float'"].IsDefined())
	{
		_invulnerability_time = load_node["'_invulnerability_time@float'"].as<float>();
	}

	if (load_node["'_dash_scaler@unsigned'"].IsDefined())
	{
		_dash_scaler = load_node["'_dash_scaler@unsigned'"].as<unsigned>();
	}

	if (load_node["'_max_dash_charges@unsigned'"].IsDefined())
	{
		_max_dash_charges = load_node["'_max_dash_charges@unsigned'"].as<unsigned>();
	}

	if (load_node["'_dash_trail@GameObject*'"].IsDefined())
	{
		_dash_trail = SceneManagement::FindInCurrentScene(load_node["'_dash_trail@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_trail_enlarger@float'"].IsDefined())
	{
		_trail_enlarger = load_node["'_trail_enlarger@float'"].as<float>();
	}

	if (load_node["'_falling_dust@GameObject*'"].IsDefined())
	{
		_falling_dust = SceneManagement::FindInCurrentScene(load_node["'_falling_dust@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_walking_dust@GameObject*'"].IsDefined())
	{
		_walking_dust = SceneManagement::FindInCurrentScene(load_node["'_walking_dust@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_heal_effect@GameObject*'"].IsDefined())
	{
		_heal_effect = SceneManagement::FindInCurrentScene(load_node["'_heal_effect@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_rotation_duration@float'"].IsDefined())
	{
		_rotation_duration = load_node["'_rotation_duration@float'"].as<float>();
	}

	if (load_node["'_hp_cell_1@GameObject*'"].IsDefined())
	{
		_hp_cell_1 = SceneManagement::FindInCurrentScene(load_node["'_hp_cell_1@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_hp_cell_2@GameObject*'"].IsDefined())
	{
		_hp_cell_2 = SceneManagement::FindInCurrentScene(load_node["'_hp_cell_2@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_hp_cell_3@GameObject*'"].IsDefined())
	{
		_hp_cell_3 = SceneManagement::FindInCurrentScene(load_node["'_hp_cell_3@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_hp_cell_4@GameObject*'"].IsDefined())
	{
		_hp_cell_4 = SceneManagement::FindInCurrentScene(load_node["'_hp_cell_4@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_ammo_cell_1@GameObject*'"].IsDefined())
	{
		_ammo_cell_1 = SceneManagement::FindInCurrentScene(load_node["'_ammo_cell_1@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_ammo_cell_2@GameObject*'"].IsDefined())
	{
		_ammo_cell_2 = SceneManagement::FindInCurrentScene(load_node["'_ammo_cell_2@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_ammo_cell_3@GameObject*'"].IsDefined())
	{
		_ammo_cell_3 = SceneManagement::FindInCurrentScene(load_node["'_ammo_cell_3@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_ammo_cell_4@GameObject*'"].IsDefined())
	{
		_ammo_cell_4 = SceneManagement::FindInCurrentScene(load_node["'_ammo_cell_4@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_weapon_charge_bar_go@GameObject*'"].IsDefined())
	{
		_weapon_charge_bar_go = SceneManagement::FindInCurrentScene(load_node["'_weapon_charge_bar_go@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_camera@GameObject*'"].IsDefined())
	{
		_camera = SceneManagement::FindInCurrentScene(load_node["'_camera@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_ui_damage@GameObject*'"].IsDefined())
	{
		_ui_damage = SceneManagement::FindInCurrentScene(load_node["'_ui_damage@GameObject*'"].as<unsigned long long>());
	}
}

void Hachiko::Scripting::PlayerSoundManager::OnSave(YAML::Node& node) const
{
	if (_audio_source != nullptr && _audio_source->GetGameObject() != nullptr)
	{
		node["'_audio_source@ComponentAudioSource*'"] = _audio_source->GetGameObject()->GetID();
	}
	else
	{
		node["'_audio_source@ComponentAudioSource*'"] = 0;
	}

	node["'_step_frequency@float'"] = _step_frequency;

	node["'_melee_frequency@float'"] = _melee_frequency;

	node["'_ranged_frequency@float'"] = _ranged_frequency;

	node["'_timer@float'"] = _timer;
}

void Hachiko::Scripting::PlayerSoundManager::OnLoad()
{
	if (load_node["'_audio_source@ComponentAudioSource*'"].IsDefined())
	{
		GameObject* _audio_source_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_audio_source@ComponentAudioSource*'"].as<unsigned long long>());
		if (_audio_source_owner__temp != nullptr)
		{
			_audio_source = _audio_source_owner__temp->GetComponent<ComponentAudioSource>();
		}
	}

	if (load_node["'_step_frequency@float'"].IsDefined())
	{
		_step_frequency = load_node["'_step_frequency@float'"].as<float>();
	}

	if (load_node["'_melee_frequency@float'"].IsDefined())
	{
		_melee_frequency = load_node["'_melee_frequency@float'"].as<float>();
	}

	if (load_node["'_ranged_frequency@float'"].IsDefined())
	{
		_ranged_frequency = load_node["'_ranged_frequency@float'"].as<float>();
	}

	if (load_node["'_timer@float'"].IsDefined())
	{
		_timer = load_node["'_timer@float'"].as<float>();
	}
}

void Hachiko::Scripting::RoomTeleporter::OnSave(YAML::Node& node) const
{
	if (_player != nullptr)
	{
		node["'_player@GameObject*'"] = _player->GetID();
	}
	else
	{
		node["'_player@GameObject*'"] = 0;
	}

	if (_room_portal != nullptr)
	{
		node["'_room_portal@GameObject*'"] = _room_portal->GetID();
	}
	else
	{
		node["'_room_portal@GameObject*'"] = 0;
	}

	if (_outdoor_portal != nullptr)
	{
		node["'_outdoor_portal@GameObject*'"] = _outdoor_portal->GetID();
	}
	else
	{
		node["'_outdoor_portal@GameObject*'"] = 0;
	}

	node["'_trigger_distance@float'"] = _trigger_distance;

	if (_fade_image != nullptr && _fade_image->GetGameObject() != nullptr)
	{
		node["'_fade_image@ComponentImage*'"] = _fade_image->GetGameObject()->GetID();
	}
	else
	{
		node["'_fade_image@ComponentImage*'"] = 0;
	}

	node["'_fade_duration@float'"] = _fade_duration;

	node["'_blackout_duration@float'"] = _blackout_duration;
}

void Hachiko::Scripting::RoomTeleporter::OnLoad()
{
	if (load_node["'_player@GameObject*'"].IsDefined())
	{
		_player = SceneManagement::FindInCurrentScene(load_node["'_player@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_room_portal@GameObject*'"].IsDefined())
	{
		_room_portal = SceneManagement::FindInCurrentScene(load_node["'_room_portal@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_outdoor_portal@GameObject*'"].IsDefined())
	{
		_outdoor_portal = SceneManagement::FindInCurrentScene(load_node["'_outdoor_portal@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_trigger_distance@float'"].IsDefined())
	{
		_trigger_distance = load_node["'_trigger_distance@float'"].as<float>();
	}

	if (load_node["'_fade_image@ComponentImage*'"].IsDefined())
	{
		GameObject* _fade_image_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_fade_image@ComponentImage*'"].as<unsigned long long>());
		if (_fade_image_owner__temp != nullptr)
		{
			_fade_image = _fade_image_owner__temp->GetComponent<ComponentImage>();
		}
	}

	if (load_node["'_fade_duration@float'"].IsDefined())
	{
		_fade_duration = load_node["'_fade_duration@float'"].as<float>();
	}

	if (load_node["'_blackout_duration@float'"].IsDefined())
	{
		_blackout_duration = load_node["'_blackout_duration@float'"].as<float>();
	}
}

void Hachiko::Scripting::AudioManager::OnSave(YAML::Node& node) const
{
	node["'_enemies_in_combat@int'"] = _enemies_in_combat;

	node["'_previous_in_combat@bool'"] = _previous_in_combat;
}

void Hachiko::Scripting::AudioManager::OnLoad()
{
	if (load_node["'_enemies_in_combat@int'"].IsDefined())
	{
		_enemies_in_combat = load_node["'_enemies_in_combat@int'"].as<int>();
	}

	if (load_node["'_previous_in_combat@bool'"].IsDefined())
	{
		_previous_in_combat = load_node["'_previous_in_combat@bool'"].as<bool>();
	}
}

void Hachiko::Scripting::BlinkingLight::OnSave(YAML::Node& node) const
{
	node["'_use_perlin_noise@bool'"] = _use_perlin_noise;

	node["'_min_radius@float'"] = _min_radius;

	node["'_max_radius@float'"] = _max_radius;

	node["'_radius_amplitude@float'"] = _radius_amplitude;

	node["'_min_duration@float'"] = _min_duration;

	node["'_max_duration@float'"] = _max_duration;

	node["'_duration_amplitude@float'"] = _duration_amplitude;

	node["'_min_intensity@float'"] = _min_intensity;

	node["'_max_intensity@float'"] = _max_intensity;

	node["'_intensity_amplitude@float'"] = _intensity_amplitude;

	node["'_starting_intensity@float'"] = _starting_intensity;

	node["'_starting_radius@float'"] = _starting_radius;

	node["'_current_intensity@float'"] = _current_intensity;

	node["'_current_duration@float'"] = _current_duration;

	node["'_current_radius@float'"] = _current_radius;

	node["'_next_duration@float'"] = _next_duration;

	node["'_next_intensity@float'"] = _next_intensity;

	node["'_next_radius@float'"] = _next_radius;
}

void Hachiko::Scripting::BlinkingLight::OnLoad()
{
	if (load_node["'_use_perlin_noise@bool'"].IsDefined())
	{
		_use_perlin_noise = load_node["'_use_perlin_noise@bool'"].as<bool>();
	}

	if (load_node["'_min_radius@float'"].IsDefined())
	{
		_min_radius = load_node["'_min_radius@float'"].as<float>();
	}

	if (load_node["'_max_radius@float'"].IsDefined())
	{
		_max_radius = load_node["'_max_radius@float'"].as<float>();
	}

	if (load_node["'_radius_amplitude@float'"].IsDefined())
	{
		_radius_amplitude = load_node["'_radius_amplitude@float'"].as<float>();
	}

	if (load_node["'_min_duration@float'"].IsDefined())
	{
		_min_duration = load_node["'_min_duration@float'"].as<float>();
	}

	if (load_node["'_max_duration@float'"].IsDefined())
	{
		_max_duration = load_node["'_max_duration@float'"].as<float>();
	}

	if (load_node["'_duration_amplitude@float'"].IsDefined())
	{
		_duration_amplitude = load_node["'_duration_amplitude@float'"].as<float>();
	}

	if (load_node["'_min_intensity@float'"].IsDefined())
	{
		_min_intensity = load_node["'_min_intensity@float'"].as<float>();
	}

	if (load_node["'_max_intensity@float'"].IsDefined())
	{
		_max_intensity = load_node["'_max_intensity@float'"].as<float>();
	}

	if (load_node["'_intensity_amplitude@float'"].IsDefined())
	{
		_intensity_amplitude = load_node["'_intensity_amplitude@float'"].as<float>();
	}

	if (load_node["'_starting_intensity@float'"].IsDefined())
	{
		_starting_intensity = load_node["'_starting_intensity@float'"].as<float>();
	}

	if (load_node["'_starting_radius@float'"].IsDefined())
	{
		_starting_radius = load_node["'_starting_radius@float'"].as<float>();
	}

	if (load_node["'_current_intensity@float'"].IsDefined())
	{
		_current_intensity = load_node["'_current_intensity@float'"].as<float>();
	}

	if (load_node["'_current_duration@float'"].IsDefined())
	{
		_current_duration = load_node["'_current_duration@float'"].as<float>();
	}

	if (load_node["'_current_radius@float'"].IsDefined())
	{
		_current_radius = load_node["'_current_radius@float'"].as<float>();
	}

	if (load_node["'_next_duration@float'"].IsDefined())
	{
		_next_duration = load_node["'_next_duration@float'"].as<float>();
	}

	if (load_node["'_next_intensity@float'"].IsDefined())
	{
		_next_intensity = load_node["'_next_intensity@float'"].as<float>();
	}

	if (load_node["'_next_radius@float'"].IsDefined())
	{
		_next_radius = load_node["'_next_radius@float'"].as<float>();
	}
}

void Hachiko::Scripting::CameraPosChange::OnSave(YAML::Node& node) const
{
	if (_objective != nullptr)
	{
		node["'_objective@GameObject*'"] = _objective->GetID();
	}
	else
	{
		node["'_objective@GameObject*'"] = 0;
	}

	node["'_do_look_ahead@bool'"] = _do_look_ahead;

	node["'_speed@float'"] = _speed;

	node["'_duration@float'"] = _duration;

	node["'_relative_position@float3'"] = _relative_position;

	node["'_rotation@float3'"] = _rotation;
}

void Hachiko::Scripting::CameraPosChange::OnLoad()
{
	if (load_node["'_objective@GameObject*'"].IsDefined())
	{
		_objective = SceneManagement::FindInCurrentScene(load_node["'_objective@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_do_look_ahead@bool'"].IsDefined())
	{
		_do_look_ahead = load_node["'_do_look_ahead@bool'"].as<bool>();
	}

	if (load_node["'_speed@float'"].IsDefined())
	{
		_speed = load_node["'_speed@float'"].as<float>();
	}

	if (load_node["'_duration@float'"].IsDefined())
	{
		_duration = load_node["'_duration@float'"].as<float>();
	}

	if (load_node["'_relative_position@float3'"].IsDefined())
	{
		_relative_position = load_node["'_relative_position@float3'"].as<float3>();
	}

	if (load_node["'_rotation@float3'"].IsDefined())
	{
		_rotation = load_node["'_rotation@float3'"].as<float3>();
	}
}

void Hachiko::Scripting::CrystalPlatform::OnSave(YAML::Node& node) const
{
	node["'_seconds_before_shaking@float'"] = _seconds_before_shaking;

	node["'_seconds_shaking@float'"] = _seconds_shaking;

	node["'_seconds_to_regen@float'"] = _seconds_to_regen;

	node["'_shake_intensity@float'"] = _shake_intensity;

	if (_invisible_obstacle != nullptr)
	{
		node["'_invisible_obstacle@GameObject*'"] = _invisible_obstacle->GetID();
	}
	else
	{
		node["'_invisible_obstacle@GameObject*'"] = 0;
	}

	if (_crystal_platform != nullptr)
	{
		node["'_crystal_platform@GameObject*'"] = _crystal_platform->GetID();
	}
	else
	{
		node["'_crystal_platform@GameObject*'"] = 0;
	}
}

void Hachiko::Scripting::CrystalPlatform::OnLoad()
{
	if (load_node["'_seconds_before_shaking@float'"].IsDefined())
	{
		_seconds_before_shaking = load_node["'_seconds_before_shaking@float'"].as<float>();
	}

	if (load_node["'_seconds_shaking@float'"].IsDefined())
	{
		_seconds_shaking = load_node["'_seconds_shaking@float'"].as<float>();
	}

	if (load_node["'_seconds_to_regen@float'"].IsDefined())
	{
		_seconds_to_regen = load_node["'_seconds_to_regen@float'"].as<float>();
	}

	if (load_node["'_shake_intensity@float'"].IsDefined())
	{
		_shake_intensity = load_node["'_shake_intensity@float'"].as<float>();
	}

	if (load_node["'_invisible_obstacle@GameObject*'"].IsDefined())
	{
		_invisible_obstacle = SceneManagement::FindInCurrentScene(load_node["'_invisible_obstacle@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_crystal_platform@GameObject*'"].IsDefined())
	{
		_crystal_platform = SceneManagement::FindInCurrentScene(load_node["'_crystal_platform@GameObject*'"].as<unsigned long long>());
	}
}

void Hachiko::Scripting::DoorController::OnSave(YAML::Node& node) const
{
	if (_door_prefab != nullptr)
	{
		node["'_door_prefab@GameObject*'"] = _door_prefab->GetID();
	}
	else
	{
		node["'_door_prefab@GameObject*'"] = 0;
	}
}

void Hachiko::Scripting::DoorController::OnLoad()
{
	if (load_node["'_door_prefab@GameObject*'"].IsDefined())
	{
		_door_prefab = SceneManagement::FindInCurrentScene(load_node["'_door_prefab@GameObject*'"].as<unsigned long long>());
	}
}

void Hachiko::Scripting::DynamicCamera::OnSave(YAML::Node& node) const
{
	node["'_speed@float'"] = _speed;
}

void Hachiko::Scripting::DynamicCamera::OnLoad()
{
	if (load_node["'_speed@float'"].IsDefined())
	{
		_speed = load_node["'_speed@float'"].as<float>();
	}
}

void Hachiko::Scripting::FancyLights::OnSave(YAML::Node& node) const
{
	node["'_rotate_on_y@bool'"] = _rotate_on_y;

	node["'_angle@float'"] = _angle;
}

void Hachiko::Scripting::FancyLights::OnLoad()
{
	if (load_node["'_rotate_on_y@bool'"].IsDefined())
	{
		_rotate_on_y = load_node["'_rotate_on_y@bool'"].as<bool>();
	}

	if (load_node["'_angle@float'"].IsDefined())
	{
		_angle = load_node["'_angle@float'"].as<float>();
	}
}

void Hachiko::Scripting::GauntletManager::OnSave(YAML::Node& node) const
{
	if (_combat_manager_go != nullptr)
	{
		node["'_combat_manager_go@GameObject*'"] = _combat_manager_go->GetID();
	}
	else
	{
		node["'_combat_manager_go@GameObject*'"] = 0;
	}

	node["'_trigger_radius@float'"] = _trigger_radius;

	node["'_round_wait_time@float'"] = _round_wait_time;

	node["'_complete_wait_time@float'"] = _complete_wait_time;

	if (_door_controller_1 != nullptr)
	{
		node["'_door_controller_1@GameObject*'"] = _door_controller_1->GetID();
	}
	else
	{
		node["'_door_controller_1@GameObject*'"] = 0;
	}

	if (_door_controller_2 != nullptr)
	{
		node["'_door_controller_2@GameObject*'"] = _door_controller_2->GetID();
	}
	else
	{
		node["'_door_controller_2@GameObject*'"] = 0;
	}

	if (_pack_1 != nullptr)
	{
		node["'_pack_1@GameObject*'"] = _pack_1->GetID();
	}
	else
	{
		node["'_pack_1@GameObject*'"] = 0;
	}

	if (_pack_2 != nullptr)
	{
		node["'_pack_2@GameObject*'"] = _pack_2->GetID();
	}
	else
	{
		node["'_pack_2@GameObject*'"] = 0;
	}

	if (_pack_3 != nullptr)
	{
		node["'_pack_3@GameObject*'"] = _pack_3->GetID();
	}
	else
	{
		node["'_pack_3@GameObject*'"] = 0;
	}
}

void Hachiko::Scripting::GauntletManager::OnLoad()
{
	if (load_node["'_combat_manager_go@GameObject*'"].IsDefined())
	{
		_combat_manager_go = SceneManagement::FindInCurrentScene(load_node["'_combat_manager_go@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_trigger_radius@float'"].IsDefined())
	{
		_trigger_radius = load_node["'_trigger_radius@float'"].as<float>();
	}

	if (load_node["'_round_wait_time@float'"].IsDefined())
	{
		_round_wait_time = load_node["'_round_wait_time@float'"].as<float>();
	}

	if (load_node["'_complete_wait_time@float'"].IsDefined())
	{
		_complete_wait_time = load_node["'_complete_wait_time@float'"].as<float>();
	}

	if (load_node["'_door_controller_1@GameObject*'"].IsDefined())
	{
		_door_controller_1 = SceneManagement::FindInCurrentScene(load_node["'_door_controller_1@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_door_controller_2@GameObject*'"].IsDefined())
	{
		_door_controller_2 = SceneManagement::FindInCurrentScene(load_node["'_door_controller_2@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_pack_1@GameObject*'"].IsDefined())
	{
		_pack_1 = SceneManagement::FindInCurrentScene(load_node["'_pack_1@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_pack_2@GameObject*'"].IsDefined())
	{
		_pack_2 = SceneManagement::FindInCurrentScene(load_node["'_pack_2@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_pack_3@GameObject*'"].IsDefined())
	{
		_pack_3 = SceneManagement::FindInCurrentScene(load_node["'_pack_3@GameObject*'"].as<unsigned long long>());
	}
}

void Hachiko::Scripting::LaserController::OnSave(YAML::Node& node) const
{
	if (_laser != nullptr)
	{
		node["'_laser@GameObject*'"] = _laser->GetID();
	}
	else
	{
		node["'_laser@GameObject*'"] = 0;
	}

	node["'_max_length@float'"] = _max_length;

	node["'_max_scale@float'"] = _max_scale;

	node["'_activation_time@float'"] = _activation_time;

	node["'_damage@float'"] = _damage;

	node["'_spin_movement@bool'"] = _spin_movement;

	node["'_spin_speed@float'"] = _spin_speed;

	node["'_horizonal_movement@bool'"] = _horizonal_movement;

	node["'_movement_speed@float'"] = _movement_speed;

	if (_movement_target != nullptr)
	{
		node["'_movement_target@GameObject*'"] = _movement_target->GetID();
	}
	else
	{
		node["'_movement_target@GameObject*'"] = 0;
	}

	node["'_toggle_activation@bool'"] = _toggle_activation;

	node["'_toggle_active_time@float'"] = _toggle_active_time;

	node["'_toggle_inactive_time@float'"] = _toggle_inactive_time;
}

void Hachiko::Scripting::LaserController::OnLoad()
{
	if (load_node["'_laser@GameObject*'"].IsDefined())
	{
		_laser = SceneManagement::FindInCurrentScene(load_node["'_laser@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_max_length@float'"].IsDefined())
	{
		_max_length = load_node["'_max_length@float'"].as<float>();
	}

	if (load_node["'_max_scale@float'"].IsDefined())
	{
		_max_scale = load_node["'_max_scale@float'"].as<float>();
	}

	if (load_node["'_activation_time@float'"].IsDefined())
	{
		_activation_time = load_node["'_activation_time@float'"].as<float>();
	}

	if (load_node["'_damage@float'"].IsDefined())
	{
		_damage = load_node["'_damage@float'"].as<float>();
	}

	if (load_node["'_spin_movement@bool'"].IsDefined())
	{
		_spin_movement = load_node["'_spin_movement@bool'"].as<bool>();
	}

	if (load_node["'_spin_speed@float'"].IsDefined())
	{
		_spin_speed = load_node["'_spin_speed@float'"].as<float>();
	}

	if (load_node["'_horizonal_movement@bool'"].IsDefined())
	{
		_horizonal_movement = load_node["'_horizonal_movement@bool'"].as<bool>();
	}

	if (load_node["'_movement_speed@float'"].IsDefined())
	{
		_movement_speed = load_node["'_movement_speed@float'"].as<float>();
	}

	if (load_node["'_movement_target@GameObject*'"].IsDefined())
	{
		_movement_target = SceneManagement::FindInCurrentScene(load_node["'_movement_target@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_toggle_activation@bool'"].IsDefined())
	{
		_toggle_activation = load_node["'_toggle_activation@bool'"].as<bool>();
	}

	if (load_node["'_toggle_active_time@float'"].IsDefined())
	{
		_toggle_active_time = load_node["'_toggle_active_time@float'"].as<float>();
	}

	if (load_node["'_toggle_inactive_time@float'"].IsDefined())
	{
		_toggle_inactive_time = load_node["'_toggle_inactive_time@float'"].as<float>();
	}
}

void Hachiko::Scripting::LevelManager::OnSave(YAML::Node& node) const
{
	node["'_respawn_position@float3'"] = _respawn_position;

	if (_gauntlet_ui_go != nullptr)
	{
		node["'_gauntlet_ui_go@GameObject*'"] = _gauntlet_ui_go->GetID();
	}
	else
	{
		node["'_gauntlet_ui_go@GameObject*'"] = 0;
	}

	if (_gauntlet_counter_go != nullptr)
	{
		node["'_gauntlet_counter_go@GameObject*'"] = _gauntlet_counter_go->GetID();
	}
	else
	{
		node["'_gauntlet_counter_go@GameObject*'"] = 0;
	}
}

void Hachiko::Scripting::LevelManager::OnLoad()
{
	if (load_node["'_respawn_position@float3'"].IsDefined())
	{
		_respawn_position = load_node["'_respawn_position@float3'"].as<float3>();
	}

	if (load_node["'_gauntlet_ui_go@GameObject*'"].IsDefined())
	{
		_gauntlet_ui_go = SceneManagement::FindInCurrentScene(load_node["'_gauntlet_ui_go@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_gauntlet_counter_go@GameObject*'"].IsDefined())
	{
		_gauntlet_counter_go = SceneManagement::FindInCurrentScene(load_node["'_gauntlet_counter_go@GameObject*'"].as<unsigned long long>());
	}
}

void Hachiko::Scripting::PillarCheckpoint::OnSave(YAML::Node& node) const
{
	if (_respawn_go != nullptr)
	{
		node["'_respawn_go@GameObject*'"] = _respawn_go->GetID();
	}
	else
	{
		node["'_respawn_go@GameObject*'"] = 0;
	}

	node["'_activation_range@float'"] = _activation_range;
}

void Hachiko::Scripting::PillarCheckpoint::OnLoad()
{
	if (load_node["'_respawn_go@GameObject*'"].IsDefined())
	{
		_respawn_go = SceneManagement::FindInCurrentScene(load_node["'_respawn_go@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_activation_range@float'"].IsDefined())
	{
		_activation_range = load_node["'_activation_range@float'"].as<float>();
	}
}

void Hachiko::Scripting::Spawner::OnSave(YAML::Node& node) const
{
	if (_enemy_pack != nullptr)
	{
		node["'_enemy_pack@GameObject*'"] = _enemy_pack->GetID();
	}
	else
	{
		node["'_enemy_pack@GameObject*'"] = 0;
	}
}

void Hachiko::Scripting::Spawner::OnLoad()
{
	if (load_node["'_enemy_pack@GameObject*'"].IsDefined())
	{
		_enemy_pack = SceneManagement::FindInCurrentScene(load_node["'_enemy_pack@GameObject*'"].as<unsigned long long>());
	}
}

void Hachiko::Scripting::BackToMainMenu::OnSave(YAML::Node& node) const
{
	if (_button_back != nullptr && _button_back->GetGameObject() != nullptr)
	{
		node["'_button_back@ComponentButton*'"] = _button_back->GetGameObject()->GetID();
	}
	else
	{
		node["'_button_back@ComponentButton*'"] = 0;
	}
}

void Hachiko::Scripting::BackToMainMenu::OnLoad()
{
	if (load_node["'_button_back@ComponentButton*'"].IsDefined())
	{
		GameObject* _button_back_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_button_back@ComponentButton*'"].as<unsigned long long>());
		if (_button_back_owner__temp != nullptr)
		{
			_button_back = _button_back_owner__temp->GetComponent<ComponentButton>();
		}
	}
}

void Hachiko::Scripting::DebugManager::OnSave(YAML::Node& node) const
{
	if (_player != nullptr)
	{
		node["'_player@GameObject*'"] = _player->GetID();
	}
	else
	{
		node["'_player@GameObject*'"] = 0;
	}

	if (_button_back != nullptr && _button_back->GetGameObject() != nullptr)
	{
		node["'_button_back@ComponentButton*'"] = _button_back->GetGameObject()->GetID();
	}
	else
	{
		node["'_button_back@ComponentButton*'"] = 0;
	}

	if (_teleport_next_pos != nullptr && _teleport_next_pos->GetGameObject() != nullptr)
	{
		node["'_teleport_next_pos@ComponentButton*'"] = _teleport_next_pos->GetGameObject()->GetID();
	}
	else
	{
		node["'_teleport_next_pos@ComponentButton*'"] = 0;
	}

	if (_teleport_prev_pos != nullptr && _teleport_prev_pos->GetGameObject() != nullptr)
	{
		node["'_teleport_prev_pos@ComponentButton*'"] = _teleport_prev_pos->GetGameObject()->GetID();
	}
	else
	{
		node["'_teleport_prev_pos@ComponentButton*'"] = 0;
	}

	if (_teleport_add_pos != nullptr && _teleport_add_pos->GetGameObject() != nullptr)
	{
		node["'_teleport_add_pos@ComponentButton*'"] = _teleport_add_pos->GetGameObject()->GetID();
	}
	else
	{
		node["'_teleport_add_pos@ComponentButton*'"] = 0;
	}

	if (_add_health != nullptr && _add_health->GetGameObject() != nullptr)
	{
		node["'_add_health@ComponentButton*'"] = _add_health->GetGameObject()->GetID();
	}
	else
	{
		node["'_add_health@ComponentButton*'"] = 0;
	}

	if (_remove_health != nullptr && _remove_health->GetGameObject() != nullptr)
	{
		node["'_remove_health@ComponentButton*'"] = _remove_health->GetGameObject()->GetID();
	}
	else
	{
		node["'_remove_health@ComponentButton*'"] = 0;
	}

	if (_increase_max_hp != nullptr && _increase_max_hp->GetGameObject() != nullptr)
	{
		node["'_increase_max_hp@ComponentButton*'"] = _increase_max_hp->GetGameObject()->GetID();
	}
	else
	{
		node["'_increase_max_hp@ComponentButton*'"] = 0;
	}

	if (_decrease_max_hp != nullptr && _decrease_max_hp->GetGameObject() != nullptr)
	{
		node["'_decrease_max_hp@ComponentButton*'"] = _decrease_max_hp->GetGameObject()->GetID();
	}
	else
	{
		node["'_decrease_max_hp@ComponentButton*'"] = 0;
	}

	if (_increase_move_speed != nullptr && _increase_move_speed->GetGameObject() != nullptr)
	{
		node["'_increase_move_speed@ComponentButton*'"] = _increase_move_speed->GetGameObject()->GetID();
	}
	else
	{
		node["'_increase_move_speed@ComponentButton*'"] = 0;
	}

	if (_decrease_move_speed != nullptr && _decrease_move_speed->GetGameObject() != nullptr)
	{
		node["'_decrease_move_speed@ComponentButton*'"] = _decrease_move_speed->GetGameObject()->GetID();
	}
	else
	{
		node["'_decrease_move_speed@ComponentButton*'"] = 0;
	}

	if (_increase_attack_cd != nullptr && _increase_attack_cd->GetGameObject() != nullptr)
	{
		node["'_increase_attack_cd@ComponentButton*'"] = _increase_attack_cd->GetGameObject()->GetID();
	}
	else
	{
		node["'_increase_attack_cd@ComponentButton*'"] = 0;
	}

	if (_decrease_attack_cd != nullptr && _decrease_attack_cd->GetGameObject() != nullptr)
	{
		node["'_decrease_attack_cd@ComponentButton*'"] = _decrease_attack_cd->GetGameObject()->GetID();
	}
	else
	{
		node["'_decrease_attack_cd@ComponentButton*'"] = 0;
	}

	if (_increase_attack_power != nullptr && _increase_attack_power->GetGameObject() != nullptr)
	{
		node["'_increase_attack_power@ComponentButton*'"] = _increase_attack_power->GetGameObject()->GetID();
	}
	else
	{
		node["'_increase_attack_power@ComponentButton*'"] = 0;
	}

	if (_decrease_attack_power != nullptr && _decrease_attack_power->GetGameObject() != nullptr)
	{
		node["'_decrease_attack_power@ComponentButton*'"] = _decrease_attack_power->GetGameObject()->GetID();
	}
	else
	{
		node["'_decrease_attack_power@ComponentButton*'"] = 0;
	}

	if (_god_mode != nullptr && _god_mode->GetGameObject() != nullptr)
	{
		node["'_god_mode@ComponentButton*'"] = _god_mode->GetGameObject()->GetID();
	}
	else
	{
		node["'_god_mode@ComponentButton*'"] = 0;
	}

	if (_spawn_enemy != nullptr && _spawn_enemy->GetGameObject() != nullptr)
	{
		node["'_spawn_enemy@ComponentButton*'"] = _spawn_enemy->GetGameObject()->GetID();
	}
	else
	{
		node["'_spawn_enemy@ComponentButton*'"] = 0;
	}

	if (_unlock_skills != nullptr && _unlock_skills->GetGameObject() != nullptr)
	{
		node["'_unlock_skills@ComponentButton*'"] = _unlock_skills->GetGameObject()->GetID();
	}
	else
	{
		node["'_unlock_skills@ComponentButton*'"] = 0;
	}

	if (_toggle_performance_output != nullptr && _toggle_performance_output->GetGameObject() != nullptr)
	{
		node["'_toggle_performance_output@ComponentButton*'"] = _toggle_performance_output->GetGameObject()->GetID();
	}
	else
	{
		node["'_toggle_performance_output@ComponentButton*'"] = 0;
	}

	if (_toggle_vsync != nullptr && _toggle_vsync->GetGameObject() != nullptr)
	{
		node["'_toggle_vsync@ComponentButton*'"] = _toggle_vsync->GetGameObject()->GetID();
	}
	else
	{
		node["'_toggle_vsync@ComponentButton*'"] = 0;
	}

	if (_toggle_wireframe != nullptr && _toggle_wireframe->GetGameObject() != nullptr)
	{
		node["'_toggle_wireframe@ComponentButton*'"] = _toggle_wireframe->GetGameObject()->GetID();
	}
	else
	{
		node["'_toggle_wireframe@ComponentButton*'"] = 0;
	}

	if (_toggle_show_colliders != nullptr && _toggle_show_colliders->GetGameObject() != nullptr)
	{
		node["'_toggle_show_colliders@ComponentButton*'"] = _toggle_show_colliders->GetGameObject()->GetID();
	}
	else
	{
		node["'_toggle_show_colliders@ComponentButton*'"] = 0;
	}

	if (_exit_debug != nullptr && _exit_debug->GetGameObject() != nullptr)
	{
		node["'_exit_debug@ComponentButton*'"] = _exit_debug->GetGameObject()->GetID();
	}
	else
	{
		node["'_exit_debug@ComponentButton*'"] = 0;
	}

	if (_text_fps != nullptr && _text_fps->GetGameObject() != nullptr)
	{
		node["'_text_fps@ComponentText*'"] = _text_fps->GetGameObject()->GetID();
	}
	else
	{
		node["'_text_fps@ComponentText*'"] = 0;
	}

	if (_text_ms != nullptr && _text_ms->GetGameObject() != nullptr)
	{
		node["'_text_ms@ComponentText*'"] = _text_ms->GetGameObject()->GetID();
	}
	else
	{
		node["'_text_ms@ComponentText*'"] = 0;
	}

	if (_tp_pos1 != nullptr)
	{
		node["'_tp_pos1@GameObject*'"] = _tp_pos1->GetID();
	}
	else
	{
		node["'_tp_pos1@GameObject*'"] = 0;
	}

	if (_tp_pos2 != nullptr)
	{
		node["'_tp_pos2@GameObject*'"] = _tp_pos2->GetID();
	}
	else
	{
		node["'_tp_pos2@GameObject*'"] = 0;
	}

	if (_tp_pos3 != nullptr)
	{
		node["'_tp_pos3@GameObject*'"] = _tp_pos3->GetID();
	}
	else
	{
		node["'_tp_pos3@GameObject*'"] = 0;
	}

	if (_performance_menu != nullptr)
	{
		node["'_performance_menu@GameObject*'"] = _performance_menu->GetID();
	}
	else
	{
		node["'_performance_menu@GameObject*'"] = 0;
	}
}

void Hachiko::Scripting::DebugManager::OnLoad()
{
	if (load_node["'_player@GameObject*'"].IsDefined())
	{
		_player = SceneManagement::FindInCurrentScene(load_node["'_player@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_button_back@ComponentButton*'"].IsDefined())
	{
		GameObject* _button_back_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_button_back@ComponentButton*'"].as<unsigned long long>());
		if (_button_back_owner__temp != nullptr)
		{
			_button_back = _button_back_owner__temp->GetComponent<ComponentButton>();
		}
	}

	if (load_node["'_teleport_next_pos@ComponentButton*'"].IsDefined())
	{
		GameObject* _teleport_next_pos_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_teleport_next_pos@ComponentButton*'"].as<unsigned long long>());
		if (_teleport_next_pos_owner__temp != nullptr)
		{
			_teleport_next_pos = _teleport_next_pos_owner__temp->GetComponent<ComponentButton>();
		}
	}

	if (load_node["'_teleport_prev_pos@ComponentButton*'"].IsDefined())
	{
		GameObject* _teleport_prev_pos_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_teleport_prev_pos@ComponentButton*'"].as<unsigned long long>());
		if (_teleport_prev_pos_owner__temp != nullptr)
		{
			_teleport_prev_pos = _teleport_prev_pos_owner__temp->GetComponent<ComponentButton>();
		}
	}

	if (load_node["'_teleport_add_pos@ComponentButton*'"].IsDefined())
	{
		GameObject* _teleport_add_pos_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_teleport_add_pos@ComponentButton*'"].as<unsigned long long>());
		if (_teleport_add_pos_owner__temp != nullptr)
		{
			_teleport_add_pos = _teleport_add_pos_owner__temp->GetComponent<ComponentButton>();
		}
	}

	if (load_node["'_add_health@ComponentButton*'"].IsDefined())
	{
		GameObject* _add_health_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_add_health@ComponentButton*'"].as<unsigned long long>());
		if (_add_health_owner__temp != nullptr)
		{
			_add_health = _add_health_owner__temp->GetComponent<ComponentButton>();
		}
	}

	if (load_node["'_remove_health@ComponentButton*'"].IsDefined())
	{
		GameObject* _remove_health_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_remove_health@ComponentButton*'"].as<unsigned long long>());
		if (_remove_health_owner__temp != nullptr)
		{
			_remove_health = _remove_health_owner__temp->GetComponent<ComponentButton>();
		}
	}

	if (load_node["'_increase_max_hp@ComponentButton*'"].IsDefined())
	{
		GameObject* _increase_max_hp_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_increase_max_hp@ComponentButton*'"].as<unsigned long long>());
		if (_increase_max_hp_owner__temp != nullptr)
		{
			_increase_max_hp = _increase_max_hp_owner__temp->GetComponent<ComponentButton>();
		}
	}

	if (load_node["'_decrease_max_hp@ComponentButton*'"].IsDefined())
	{
		GameObject* _decrease_max_hp_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_decrease_max_hp@ComponentButton*'"].as<unsigned long long>());
		if (_decrease_max_hp_owner__temp != nullptr)
		{
			_decrease_max_hp = _decrease_max_hp_owner__temp->GetComponent<ComponentButton>();
		}
	}

	if (load_node["'_increase_move_speed@ComponentButton*'"].IsDefined())
	{
		GameObject* _increase_move_speed_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_increase_move_speed@ComponentButton*'"].as<unsigned long long>());
		if (_increase_move_speed_owner__temp != nullptr)
		{
			_increase_move_speed = _increase_move_speed_owner__temp->GetComponent<ComponentButton>();
		}
	}

	if (load_node["'_decrease_move_speed@ComponentButton*'"].IsDefined())
	{
		GameObject* _decrease_move_speed_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_decrease_move_speed@ComponentButton*'"].as<unsigned long long>());
		if (_decrease_move_speed_owner__temp != nullptr)
		{
			_decrease_move_speed = _decrease_move_speed_owner__temp->GetComponent<ComponentButton>();
		}
	}

	if (load_node["'_increase_attack_cd@ComponentButton*'"].IsDefined())
	{
		GameObject* _increase_attack_cd_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_increase_attack_cd@ComponentButton*'"].as<unsigned long long>());
		if (_increase_attack_cd_owner__temp != nullptr)
		{
			_increase_attack_cd = _increase_attack_cd_owner__temp->GetComponent<ComponentButton>();
		}
	}

	if (load_node["'_decrease_attack_cd@ComponentButton*'"].IsDefined())
	{
		GameObject* _decrease_attack_cd_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_decrease_attack_cd@ComponentButton*'"].as<unsigned long long>());
		if (_decrease_attack_cd_owner__temp != nullptr)
		{
			_decrease_attack_cd = _decrease_attack_cd_owner__temp->GetComponent<ComponentButton>();
		}
	}

	if (load_node["'_increase_attack_power@ComponentButton*'"].IsDefined())
	{
		GameObject* _increase_attack_power_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_increase_attack_power@ComponentButton*'"].as<unsigned long long>());
		if (_increase_attack_power_owner__temp != nullptr)
		{
			_increase_attack_power = _increase_attack_power_owner__temp->GetComponent<ComponentButton>();
		}
	}

	if (load_node["'_decrease_attack_power@ComponentButton*'"].IsDefined())
	{
		GameObject* _decrease_attack_power_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_decrease_attack_power@ComponentButton*'"].as<unsigned long long>());
		if (_decrease_attack_power_owner__temp != nullptr)
		{
			_decrease_attack_power = _decrease_attack_power_owner__temp->GetComponent<ComponentButton>();
		}
	}

	if (load_node["'_god_mode@ComponentButton*'"].IsDefined())
	{
		GameObject* _god_mode_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_god_mode@ComponentButton*'"].as<unsigned long long>());
		if (_god_mode_owner__temp != nullptr)
		{
			_god_mode = _god_mode_owner__temp->GetComponent<ComponentButton>();
		}
	}

	if (load_node["'_spawn_enemy@ComponentButton*'"].IsDefined())
	{
		GameObject* _spawn_enemy_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_spawn_enemy@ComponentButton*'"].as<unsigned long long>());
		if (_spawn_enemy_owner__temp != nullptr)
		{
			_spawn_enemy = _spawn_enemy_owner__temp->GetComponent<ComponentButton>();
		}
	}

	if (load_node["'_unlock_skills@ComponentButton*'"].IsDefined())
	{
		GameObject* _unlock_skills_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_unlock_skills@ComponentButton*'"].as<unsigned long long>());
		if (_unlock_skills_owner__temp != nullptr)
		{
			_unlock_skills = _unlock_skills_owner__temp->GetComponent<ComponentButton>();
		}
	}

	if (load_node["'_toggle_performance_output@ComponentButton*'"].IsDefined())
	{
		GameObject* _toggle_performance_output_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_toggle_performance_output@ComponentButton*'"].as<unsigned long long>());
		if (_toggle_performance_output_owner__temp != nullptr)
		{
			_toggle_performance_output = _toggle_performance_output_owner__temp->GetComponent<ComponentButton>();
		}
	}

	if (load_node["'_toggle_vsync@ComponentButton*'"].IsDefined())
	{
		GameObject* _toggle_vsync_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_toggle_vsync@ComponentButton*'"].as<unsigned long long>());
		if (_toggle_vsync_owner__temp != nullptr)
		{
			_toggle_vsync = _toggle_vsync_owner__temp->GetComponent<ComponentButton>();
		}
	}

	if (load_node["'_toggle_wireframe@ComponentButton*'"].IsDefined())
	{
		GameObject* _toggle_wireframe_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_toggle_wireframe@ComponentButton*'"].as<unsigned long long>());
		if (_toggle_wireframe_owner__temp != nullptr)
		{
			_toggle_wireframe = _toggle_wireframe_owner__temp->GetComponent<ComponentButton>();
		}
	}

	if (load_node["'_toggle_show_colliders@ComponentButton*'"].IsDefined())
	{
		GameObject* _toggle_show_colliders_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_toggle_show_colliders@ComponentButton*'"].as<unsigned long long>());
		if (_toggle_show_colliders_owner__temp != nullptr)
		{
			_toggle_show_colliders = _toggle_show_colliders_owner__temp->GetComponent<ComponentButton>();
		}
	}

	if (load_node["'_exit_debug@ComponentButton*'"].IsDefined())
	{
		GameObject* _exit_debug_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_exit_debug@ComponentButton*'"].as<unsigned long long>());
		if (_exit_debug_owner__temp != nullptr)
		{
			_exit_debug = _exit_debug_owner__temp->GetComponent<ComponentButton>();
		}
	}

	if (load_node["'_text_fps@ComponentText*'"].IsDefined())
	{
		GameObject* _text_fps_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_text_fps@ComponentText*'"].as<unsigned long long>());
		if (_text_fps_owner__temp != nullptr)
		{
			_text_fps = _text_fps_owner__temp->GetComponent<ComponentText>();
		}
	}

	if (load_node["'_text_ms@ComponentText*'"].IsDefined())
	{
		GameObject* _text_ms_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_text_ms@ComponentText*'"].as<unsigned long long>());
		if (_text_ms_owner__temp != nullptr)
		{
			_text_ms = _text_ms_owner__temp->GetComponent<ComponentText>();
		}
	}

	if (load_node["'_tp_pos1@GameObject*'"].IsDefined())
	{
		_tp_pos1 = SceneManagement::FindInCurrentScene(load_node["'_tp_pos1@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_tp_pos2@GameObject*'"].IsDefined())
	{
		_tp_pos2 = SceneManagement::FindInCurrentScene(load_node["'_tp_pos2@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_tp_pos3@GameObject*'"].IsDefined())
	{
		_tp_pos3 = SceneManagement::FindInCurrentScene(load_node["'_tp_pos3@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_performance_menu@GameObject*'"].IsDefined())
	{
		_performance_menu = SceneManagement::FindInCurrentScene(load_node["'_performance_menu@GameObject*'"].as<unsigned long long>());
	}
}

void Hachiko::Scripting::MainMenuManager::OnSave(YAML::Node& node) const
{

	node["'_state_changed@bool'"] = _state_changed;

	if (_main_background != nullptr)
	{
		node["'_main_background@GameObject*'"] = _main_background->GetID();
	}
	else
	{
		node["'_main_background@GameObject*'"] = 0;
	}

	if (_button_background != nullptr)
	{
		node["'_button_background@GameObject*'"] = _button_background->GetID();
	}
	else
	{
		node["'_button_background@GameObject*'"] = 0;
	}

	if (_button_play != nullptr && _button_play->GetGameObject() != nullptr)
	{
		node["'_button_play@ComponentButton*'"] = _button_play->GetGameObject()->GetID();
	}
	else
	{
		node["'_button_play@ComponentButton*'"] = 0;
	}

	if (_button_quit != nullptr && _button_quit->GetGameObject() != nullptr)
	{
		node["'_button_quit@ComponentButton*'"] = _button_quit->GetGameObject()->GetID();
	}
	else
	{
		node["'_button_quit@ComponentButton*'"] = 0;
	}

	if (_button_settings != nullptr && _button_settings->GetGameObject() != nullptr)
	{
		node["'_button_settings@ComponentButton*'"] = _button_settings->GetGameObject()->GetID();
	}
	else
	{
		node["'_button_settings@ComponentButton*'"] = 0;
	}

	if (_button_credits != nullptr && _button_credits->GetGameObject() != nullptr)
	{
		node["'_button_credits@ComponentButton*'"] = _button_credits->GetGameObject()->GetID();
	}
	else
	{
		node["'_button_credits@ComponentButton*'"] = 0;
	}

	if (_game_title != nullptr)
	{
		node["'_game_title@GameObject*'"] = _game_title->GetID();
	}
	else
	{
		node["'_game_title@GameObject*'"] = 0;
	}

	if (_settings != nullptr)
	{
		node["'_settings@GameObject*'"] = _settings->GetID();
	}
	else
	{
		node["'_settings@GameObject*'"] = 0;
	}

	if (_credits != nullptr)
	{
		node["'_credits@GameObject*'"] = _credits->GetID();
	}
	else
	{
		node["'_credits@GameObject*'"] = 0;
	}

	if (_button_back != nullptr && _button_back->GetGameObject() != nullptr)
	{
		node["'_button_back@ComponentButton*'"] = _button_back->GetGameObject()->GetID();
	}
	else
	{
		node["'_button_back@ComponentButton*'"] = 0;
	}
}

void Hachiko::Scripting::MainMenuManager::OnLoad()
{

	if (load_node["'_state_changed@bool'"].IsDefined())
	{
		_state_changed = load_node["'_state_changed@bool'"].as<bool>();
	}

	if (load_node["'_main_background@GameObject*'"].IsDefined())
	{
		_main_background = SceneManagement::FindInCurrentScene(load_node["'_main_background@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_button_background@GameObject*'"].IsDefined())
	{
		_button_background = SceneManagement::FindInCurrentScene(load_node["'_button_background@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_button_play@ComponentButton*'"].IsDefined())
	{
		GameObject* _button_play_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_button_play@ComponentButton*'"].as<unsigned long long>());
		if (_button_play_owner__temp != nullptr)
		{
			_button_play = _button_play_owner__temp->GetComponent<ComponentButton>();
		}
	}

	if (load_node["'_button_quit@ComponentButton*'"].IsDefined())
	{
		GameObject* _button_quit_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_button_quit@ComponentButton*'"].as<unsigned long long>());
		if (_button_quit_owner__temp != nullptr)
		{
			_button_quit = _button_quit_owner__temp->GetComponent<ComponentButton>();
		}
	}

	if (load_node["'_button_settings@ComponentButton*'"].IsDefined())
	{
		GameObject* _button_settings_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_button_settings@ComponentButton*'"].as<unsigned long long>());
		if (_button_settings_owner__temp != nullptr)
		{
			_button_settings = _button_settings_owner__temp->GetComponent<ComponentButton>();
		}
	}

	if (load_node["'_button_credits@ComponentButton*'"].IsDefined())
	{
		GameObject* _button_credits_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_button_credits@ComponentButton*'"].as<unsigned long long>());
		if (_button_credits_owner__temp != nullptr)
		{
			_button_credits = _button_credits_owner__temp->GetComponent<ComponentButton>();
		}
	}

	if (load_node["'_game_title@GameObject*'"].IsDefined())
	{
		_game_title = SceneManagement::FindInCurrentScene(load_node["'_game_title@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_settings@GameObject*'"].IsDefined())
	{
		_settings = SceneManagement::FindInCurrentScene(load_node["'_settings@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_credits@GameObject*'"].IsDefined())
	{
		_credits = SceneManagement::FindInCurrentScene(load_node["'_credits@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_button_back@ComponentButton*'"].IsDefined())
	{
		GameObject* _button_back_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_button_back@ComponentButton*'"].as<unsigned long long>());
		if (_button_back_owner__temp != nullptr)
		{
			_button_back = _button_back_owner__temp->GetComponent<ComponentButton>();
		}
	}
}