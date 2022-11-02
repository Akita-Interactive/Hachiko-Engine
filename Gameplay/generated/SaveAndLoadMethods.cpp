#include "scriptingUtil/gameplaypch.h"
#include <yaml-cpp/yaml.h>
#include <core/serialization/TypeConverter.h>
#include "CutsceneManager.h"
#include "PauseMenu.h"
#include "TriggerAnim.h"
#include "effects/BloomAnimator.h"
#include "effects/BounceEffect.h"
#include "effects/HoverEffect.h"
#include "effects/RotationEffect.h"
#include "entities/Stats.h"
#include "entities/crystals/CrystalExplosion.h"
#include "entities/crystals/Stalagmite.h"
#include "entities/enemies/BossController.h"
#include "entities/enemies/BugAnimationManager.h"
#include "entities/enemies/EnemyController.h"
#include "entities/player/CombatManager.h"
#include "entities/player/CombatVisualEffectsPool.h"
#include "entities/player/PlayerAnimationManager.h"
#include "entities/player/PlayerCamera.h"
#include "entities/player/PlayerController.h"
#include "entities/player/PlayerSoundManager.h"
#include "entities/player/RoomTeleporter.h"
#include "misc/AssetsObstacle.h"
#include "misc/AudioManager.h"
#include "misc/BlinkingLight.h"
#include "misc/BossLaserController.h"
#include "misc/CameraPosChange.h"
#include "misc/Centipedes.h"
#include "misc/CrystalPlatform.h"
#include "misc/DoorController.h"
#include "misc/DynamicCamera.h"
#include "misc/FancyLights.h"
#include "misc/GauntletManager.h"
#include "misc/LaserController.h"
#include "misc/LevelManager.h"
#include "misc/PillarCheckpoint.h"
#include "misc/Spawner.h"
#include "misc/StalagmiteManager.h"
#include "misc/TimeManager.h"
#include "misc/Tutorial.h"
#include "ui/BackToMainMenu.h"
#include "ui/DebugManager.h"
#include "ui/MainMenuManager.h"



void Hachiko::Scripting::CutsceneManager::OnSave(YAML::Node& node) const
{
	if (_cutscene != nullptr)
	{
		node["'_cutscene@GameObject*'"] = _cutscene->GetID();
	}
	else
	{
		node["'_cutscene@GameObject*'"] = 0;
	}

	node["'_next_level@unsigned'"] = _next_level;
}

void Hachiko::Scripting::CutsceneManager::OnLoad()
{
	if (load_node["'_cutscene@GameObject*'"].IsDefined())
	{
		_cutscene = SceneManagement::FindInCurrentScene(load_node["'_cutscene@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_next_level@unsigned'"].IsDefined())
	{
		_next_level = load_node["'_next_level@unsigned'"].as<unsigned>();
	}
}

void Hachiko::Scripting::PauseMenu::OnSave(YAML::Node& node) const
{
	if (_pause_ui != nullptr)
	{
		node["'_pause_ui@GameObject*'"] = _pause_ui->GetID();
	}
	else
	{
		node["'_pause_ui@GameObject*'"] = 0;
	}

	if (_player_hud != nullptr)
	{
		node["'_player_hud@GameObject*'"] = _player_hud->GetID();
	}
	else
	{
		node["'_player_hud@GameObject*'"] = 0;
	}

	if (_resume_button != nullptr && _resume_button->GetGameObject() != nullptr)
	{
		node["'_resume_button@ComponentButton*'"] = _resume_button->GetGameObject()->GetID();
	}
	else
	{
		node["'_resume_button@ComponentButton*'"] = 0;
	}

	if (_quit_button != nullptr && _quit_button->GetGameObject() != nullptr)
	{
		node["'_quit_button@ComponentButton*'"] = _quit_button->GetGameObject()->GetID();
	}
	else
	{
		node["'_quit_button@ComponentButton*'"] = 0;
	}
}

void Hachiko::Scripting::PauseMenu::OnLoad()
{
	if (load_node["'_pause_ui@GameObject*'"].IsDefined())
	{
		_pause_ui = SceneManagement::FindInCurrentScene(load_node["'_pause_ui@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_player_hud@GameObject*'"].IsDefined())
	{
		_player_hud = SceneManagement::FindInCurrentScene(load_node["'_player_hud@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_resume_button@ComponentButton*'"].IsDefined())
	{
		GameObject* _resume_button_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_resume_button@ComponentButton*'"].as<unsigned long long>());
		if (_resume_button_owner__temp != nullptr)
		{
			_resume_button = _resume_button_owner__temp->GetComponent<ComponentButton>();
		}
	}

	if (load_node["'_quit_button@ComponentButton*'"].IsDefined())
	{
		GameObject* _quit_button_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_quit_button@ComponentButton*'"].as<unsigned long long>());
		if (_quit_button_owner__temp != nullptr)
		{
			_quit_button = _quit_button_owner__temp->GetComponent<ComponentButton>();
		}
	}
}

void Hachiko::Scripting::TriggerAnim::OnSave(YAML::Node& node) const
{
}

void Hachiko::Scripting::TriggerAnim::OnLoad()
{
}

void Hachiko::Scripting::BloomAnimator::OnSave(YAML::Node& node) const
{
	if (_bloom_target != nullptr)
	{
		node["'_bloom_target@GameObject*'"] = _bloom_target->GetID();
	}
	else
	{
		node["'_bloom_target@GameObject*'"] = 0;
	}

	node["'_is_automatic@bool'"] = _is_automatic;

	node["'_is_randomized@bool'"] = _is_randomized;

	node["'_randomized_duration_min@float'"] = _randomized_duration_min;

	node["'_randomized_duration_max@float'"] = _randomized_duration_max;

	node["'_automatic_lerp_duration@float'"] = _automatic_lerp_duration;

	node["'_initial_emissive_color@float4'"] = _initial_emissive_color;
}

void Hachiko::Scripting::BloomAnimator::OnLoad()
{
	if (load_node["'_bloom_target@GameObject*'"].IsDefined())
	{
		_bloom_target = SceneManagement::FindInCurrentScene(load_node["'_bloom_target@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_is_automatic@bool'"].IsDefined())
	{
		_is_automatic = load_node["'_is_automatic@bool'"].as<bool>();
	}

	if (load_node["'_is_randomized@bool'"].IsDefined())
	{
		_is_randomized = load_node["'_is_randomized@bool'"].as<bool>();
	}

	if (load_node["'_randomized_duration_min@float'"].IsDefined())
	{
		_randomized_duration_min = load_node["'_randomized_duration_min@float'"].as<float>();
	}

	if (load_node["'_randomized_duration_max@float'"].IsDefined())
	{
		_randomized_duration_max = load_node["'_randomized_duration_max@float'"].as<float>();
	}

	if (load_node["'_automatic_lerp_duration@float'"].IsDefined())
	{
		_automatic_lerp_duration = load_node["'_automatic_lerp_duration@float'"].as<float>();
	}

	if (load_node["'_initial_emissive_color@float4'"].IsDefined())
	{
		_initial_emissive_color = load_node["'_initial_emissive_color@float4'"].as<float4>();
	}
}

void Hachiko::Scripting::BounceEffect::OnSave(YAML::Node& node) const
{
	node["'_altitude@float'"] = _altitude;

	node["'_magnitude@float'"] = _magnitude;

	node["'_speed@float'"] = _speed;
}

void Hachiko::Scripting::BounceEffect::OnLoad()
{
	if (load_node["'_altitude@float'"].IsDefined())
	{
		_altitude = load_node["'_altitude@float'"].as<float>();
	}

	if (load_node["'_magnitude@float'"].IsDefined())
	{
		_magnitude = load_node["'_magnitude@float'"].as<float>();
	}

	if (load_node["'_speed@float'"].IsDefined())
	{
		_speed = load_node["'_speed@float'"].as<float>();
	}
}

void Hachiko::Scripting::HoverEffect::OnSave(YAML::Node& node) const
{
	node["'_altitude@float'"] = _altitude;

	node["'_magnitude@float'"] = _magnitude;

	node["'_speed@float'"] = _speed;
}

void Hachiko::Scripting::HoverEffect::OnLoad()
{
	if (load_node["'_altitude@float'"].IsDefined())
	{
		_altitude = load_node["'_altitude@float'"].as<float>();
	}

	if (load_node["'_magnitude@float'"].IsDefined())
	{
		_magnitude = load_node["'_magnitude@float'"].as<float>();
	}

	if (load_node["'_speed@float'"].IsDefined())
	{
		_speed = load_node["'_speed@float'"].as<float>();
	}
}

void Hachiko::Scripting::RotationEffect::OnSave(YAML::Node& node) const
{
	node["'_rotate_x@bool'"] = _rotate_x;

	node["'_rotate_y@bool'"] = _rotate_y;

	node["'_rotate_z@bool'"] = _rotate_z;

	node["'_rotation_speed@float'"] = _rotation_speed;
}

void Hachiko::Scripting::RotationEffect::OnLoad()
{
	if (load_node["'_rotate_x@bool'"].IsDefined())
	{
		_rotate_x = load_node["'_rotate_x@bool'"].as<bool>();
	}

	if (load_node["'_rotate_y@bool'"].IsDefined())
	{
		_rotate_y = load_node["'_rotate_y@bool'"].as<bool>();
	}

	if (load_node["'_rotate_z@bool'"].IsDefined())
	{
		_rotate_z = load_node["'_rotate_z@bool'"].as<bool>();
	}

	if (load_node["'_rotation_speed@float'"].IsDefined())
	{
		_rotation_speed = load_node["'_rotation_speed@float'"].as<float>();
	}
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

	node["'_shake_intensity@float'"] = _shake_intensity;

	node["'_seconds_shaking@float'"] = _seconds_shaking;

	node["'_crashing_index@unsigned'"] = _crashing_index;

	node["'_detecting_radius@float'"] = _detecting_radius;

	node["'_explosion_radius@float'"] = _explosion_radius;

	node["'_timer_explosion@float'"] = _timer_explosion;

	node["'_explosive_crystal@bool'"] = _explosive_crystal;

	node["'_regen_time@float'"] = _regen_time;

	node["'_should_regen@bool'"] = _should_regen;

	node["'damage_effect_duration@float'"] = damage_effect_duration;
}

void Hachiko::Scripting::CrystalExplosion::OnLoad()
{
	if (load_node["'_explosion_indicator_helper@GameObject*'"].IsDefined())
	{
		_explosion_indicator_helper = SceneManagement::FindInCurrentScene(load_node["'_explosion_indicator_helper@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_explosion_effect@GameObject*'"].IsDefined())
	{
		_explosion_effect = SceneManagement::FindInCurrentScene(load_node["'_explosion_effect@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_shake_intensity@float'"].IsDefined())
	{
		_shake_intensity = load_node["'_shake_intensity@float'"].as<float>();
	}

	if (load_node["'_seconds_shaking@float'"].IsDefined())
	{
		_seconds_shaking = load_node["'_seconds_shaking@float'"].as<float>();
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

	if (load_node["'_should_regen@bool'"].IsDefined())
	{
		_should_regen = load_node["'_should_regen@bool'"].as<bool>();
	}

	if (load_node["'damage_effect_duration@float'"].IsDefined())
	{
		damage_effect_duration = load_node["'damage_effect_duration@float'"].as<float>();
	}
}

void Hachiko::Scripting::Stalagmite::OnSave(YAML::Node& node) const
{
	if (_explosion_effect != nullptr)
	{
		node["'_explosion_effect@GameObject*'"] = _explosion_effect->GetID();
	}
	else
	{
		node["'_explosion_effect@GameObject*'"] = 0;
	}

	if (GEO != nullptr)
	{
		node["'GEO@GameObject*'"] = GEO->GetID();
	}
	else
	{
		node["'GEO@GameObject*'"] = 0;
	}

	if (_obstacle != nullptr)
	{
		node["'_obstacle@GameObject*'"] = _obstacle->GetID();
	}
	else
	{
		node["'_obstacle@GameObject*'"] = 0;
	}
}

void Hachiko::Scripting::Stalagmite::OnLoad()
{
	if (load_node["'_explosion_effect@GameObject*'"].IsDefined())
	{
		_explosion_effect = SceneManagement::FindInCurrentScene(load_node["'_explosion_effect@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'GEO@GameObject*'"].IsDefined())
	{
		GEO = SceneManagement::FindInCurrentScene(load_node["'GEO@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_obstacle@GameObject*'"].IsDefined())
	{
		_obstacle = SceneManagement::FindInCurrentScene(load_node["'_obstacle@GameObject*'"].as<unsigned long long>());
	}
}

void Hachiko::Scripting::BossController::OnSave(YAML::Node& node) const
{
	node["'state_value@int'"] = state_value;

	node["'combat_state_value@int'"] = combat_state_value;

	node["'second_phase@bool'"] = second_phase;

	if (hp_bar_go != nullptr)
	{
		node["'hp_bar_go@GameObject*'"] = hp_bar_go->GetID();
	}
	else
	{
		node["'hp_bar_go@GameObject*'"] = 0;
	}

	if (crystal_target_go != nullptr)
	{
		node["'crystal_target_go@GameObject*'"] = crystal_target_go->GetID();
	}
	else
	{
		node["'crystal_target_go@GameObject*'"] = 0;
	}

	if (cocoons_parent != nullptr)
	{
		node["'cocoons_parent@GameObject*'"] = cocoons_parent->GetID();
	}
	else
	{
		node["'cocoons_parent@GameObject*'"] = 0;
	}

	if (gauntlet_go != nullptr)
	{
		node["'gauntlet_go@GameObject*'"] = gauntlet_go->GetID();
	}
	else
	{
		node["'gauntlet_go@GameObject*'"] = 0;
	}

	node["'_current_index_crystals@int'"] = _current_index_crystals;

	if (crystal_pool != nullptr)
	{
		node["'crystal_pool@GameObject*'"] = crystal_pool->GetID();
	}
	else
	{
		node["'crystal_pool@GameObject*'"] = 0;
	}

	node["'attack_delay@float'"] = attack_delay;

	node["'after_attack_wait@float'"] = after_attack_wait;

	node["'camera_transition_speed@float'"] = camera_transition_speed;

	node["'encounter_start_duration@float'"] = encounter_start_duration;

	node["'pre_combat_camera_offset@float3'"] = pre_combat_camera_offset;

	node["'_jump_start_delay@float'"] = _jump_start_delay;

	node["'_jump_ascending_duration@float'"] = _jump_ascending_duration;

	node["'_jump_post_ascending_delay@float'"] = _jump_post_ascending_delay;

	node["'_jump_on_highest_point_duration@float'"] = _jump_on_highest_point_duration;

	node["'_jump_post_on_highest_point_delay@float'"] = _jump_post_on_highest_point_delay;

	node["'_jump_descending_duration@float'"] = _jump_descending_duration;

	node["'_jump_post_descending_delay@float'"] = _jump_post_descending_delay;

	node["'_jump_getting_up_duration@float'"] = _jump_getting_up_duration;

	node["'_jump_skill_delay@float'"] = _jump_skill_delay;

	node["'_jump_skill_duration@float'"] = _jump_skill_duration;

	node["'_jump_post_skill_delay@float'"] = _jump_post_skill_delay;

	node["'_jump_height@float'"] = _jump_height;

	node["'_jump_offset@float'"] = _jump_offset;

	node["'_damage_AOE@float'"] = _damage_AOE;

	node["'_crystal_jump_position@float3'"] = _crystal_jump_position;

	if (_stalagmite_manager_go != nullptr)
	{
		node["'_stalagmite_manager_go@GameObject*'"] = _stalagmite_manager_go->GetID();
	}
	else
	{
		node["'_stalagmite_manager_go@GameObject*'"] = 0;
	}

	if (_boss_state_text_ui != nullptr && _boss_state_text_ui->GetGameObject() != nullptr)
	{
		node["'_boss_state_text_ui@ComponentText*'"] = _boss_state_text_ui->GetGameObject()->GetID();
	}
	else
	{
		node["'_boss_state_text_ui@ComponentText*'"] = 0;
	}

	node["'time_between_enemies@float'"] = time_between_enemies;

	if (enemy_pool != nullptr)
	{
		node["'enemy_pool@GameObject*'"] = enemy_pool->GetID();
	}
	else
	{
		node["'enemy_pool@GameObject*'"] = 0;
	}

	node["'damage_effect_duration@float'"] = damage_effect_duration;

	node["'chasing_time_limit@float'"] = chasing_time_limit;

	if (_rotating_lasers != nullptr)
	{
		node["'_rotating_lasers@GameObject*'"] = _rotating_lasers->GetID();
	}
	else
	{
		node["'_rotating_lasers@GameObject*'"] = 0;
	}

	node["'_rotating_lasers_speed@float'"] = _rotating_lasers_speed;

	if (_laser_wall != nullptr)
	{
		node["'_laser_wall@GameObject*'"] = _laser_wall->GetID();
	}
	else
	{
		node["'_laser_wall@GameObject*'"] = 0;
	}

	node["'_laser_wall_duration@float'"] = _laser_wall_duration;

	node["'_laser_jump_height@float'"] = _laser_jump_height;

	if (_melee_trail_right != nullptr)
	{
		node["'_melee_trail_right@GameObject*'"] = _melee_trail_right->GetID();
	}
	else
	{
		node["'_melee_trail_right@GameObject*'"] = 0;
	}
}

void Hachiko::Scripting::BossController::OnLoad()
{
	if (load_node["'state_value@int'"].IsDefined())
	{
		state_value = load_node["'state_value@int'"].as<int>();
	}

	if (load_node["'combat_state_value@int'"].IsDefined())
	{
		combat_state_value = load_node["'combat_state_value@int'"].as<int>();
	}

	if (load_node["'second_phase@bool'"].IsDefined())
	{
		second_phase = load_node["'second_phase@bool'"].as<bool>();
	}

	if (load_node["'hp_bar_go@GameObject*'"].IsDefined())
	{
		hp_bar_go = SceneManagement::FindInCurrentScene(load_node["'hp_bar_go@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'crystal_target_go@GameObject*'"].IsDefined())
	{
		crystal_target_go = SceneManagement::FindInCurrentScene(load_node["'crystal_target_go@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'cocoons_parent@GameObject*'"].IsDefined())
	{
		cocoons_parent = SceneManagement::FindInCurrentScene(load_node["'cocoons_parent@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'gauntlet_go@GameObject*'"].IsDefined())
	{
		gauntlet_go = SceneManagement::FindInCurrentScene(load_node["'gauntlet_go@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_current_index_crystals@int'"].IsDefined())
	{
		_current_index_crystals = load_node["'_current_index_crystals@int'"].as<int>();
	}

	if (load_node["'crystal_pool@GameObject*'"].IsDefined())
	{
		crystal_pool = SceneManagement::FindInCurrentScene(load_node["'crystal_pool@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'attack_delay@float'"].IsDefined())
	{
		attack_delay = load_node["'attack_delay@float'"].as<float>();
	}

	if (load_node["'after_attack_wait@float'"].IsDefined())
	{
		after_attack_wait = load_node["'after_attack_wait@float'"].as<float>();
	}

	if (load_node["'camera_transition_speed@float'"].IsDefined())
	{
		camera_transition_speed = load_node["'camera_transition_speed@float'"].as<float>();
	}

	if (load_node["'encounter_start_duration@float'"].IsDefined())
	{
		encounter_start_duration = load_node["'encounter_start_duration@float'"].as<float>();
	}

	if (load_node["'pre_combat_camera_offset@float3'"].IsDefined())
	{
		pre_combat_camera_offset = load_node["'pre_combat_camera_offset@float3'"].as<float3>();
	}

	if (load_node["'_jump_start_delay@float'"].IsDefined())
	{
		_jump_start_delay = load_node["'_jump_start_delay@float'"].as<float>();
	}

	if (load_node["'_jump_ascending_duration@float'"].IsDefined())
	{
		_jump_ascending_duration = load_node["'_jump_ascending_duration@float'"].as<float>();
	}

	if (load_node["'_jump_post_ascending_delay@float'"].IsDefined())
	{
		_jump_post_ascending_delay = load_node["'_jump_post_ascending_delay@float'"].as<float>();
	}

	if (load_node["'_jump_on_highest_point_duration@float'"].IsDefined())
	{
		_jump_on_highest_point_duration = load_node["'_jump_on_highest_point_duration@float'"].as<float>();
	}

	if (load_node["'_jump_post_on_highest_point_delay@float'"].IsDefined())
	{
		_jump_post_on_highest_point_delay = load_node["'_jump_post_on_highest_point_delay@float'"].as<float>();
	}

	if (load_node["'_jump_descending_duration@float'"].IsDefined())
	{
		_jump_descending_duration = load_node["'_jump_descending_duration@float'"].as<float>();
	}

	if (load_node["'_jump_post_descending_delay@float'"].IsDefined())
	{
		_jump_post_descending_delay = load_node["'_jump_post_descending_delay@float'"].as<float>();
	}

	if (load_node["'_jump_getting_up_duration@float'"].IsDefined())
	{
		_jump_getting_up_duration = load_node["'_jump_getting_up_duration@float'"].as<float>();
	}

	if (load_node["'_jump_skill_delay@float'"].IsDefined())
	{
		_jump_skill_delay = load_node["'_jump_skill_delay@float'"].as<float>();
	}

	if (load_node["'_jump_skill_duration@float'"].IsDefined())
	{
		_jump_skill_duration = load_node["'_jump_skill_duration@float'"].as<float>();
	}

	if (load_node["'_jump_post_skill_delay@float'"].IsDefined())
	{
		_jump_post_skill_delay = load_node["'_jump_post_skill_delay@float'"].as<float>();
	}

	if (load_node["'_jump_height@float'"].IsDefined())
	{
		_jump_height = load_node["'_jump_height@float'"].as<float>();
	}

	if (load_node["'_jump_offset@float'"].IsDefined())
	{
		_jump_offset = load_node["'_jump_offset@float'"].as<float>();
	}

	if (load_node["'_damage_AOE@float'"].IsDefined())
	{
		_damage_AOE = load_node["'_damage_AOE@float'"].as<float>();
	}

	if (load_node["'_crystal_jump_position@float3'"].IsDefined())
	{
		_crystal_jump_position = load_node["'_crystal_jump_position@float3'"].as<float3>();
	}

	if (load_node["'_stalagmite_manager_go@GameObject*'"].IsDefined())
	{
		_stalagmite_manager_go = SceneManagement::FindInCurrentScene(load_node["'_stalagmite_manager_go@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_boss_state_text_ui@ComponentText*'"].IsDefined())
	{
		GameObject* _boss_state_text_ui_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_boss_state_text_ui@ComponentText*'"].as<unsigned long long>());
		if (_boss_state_text_ui_owner__temp != nullptr)
		{
			_boss_state_text_ui = _boss_state_text_ui_owner__temp->GetComponent<ComponentText>();
		}
	}

	if (load_node["'time_between_enemies@float'"].IsDefined())
	{
		time_between_enemies = load_node["'time_between_enemies@float'"].as<float>();
	}

	if (load_node["'enemy_pool@GameObject*'"].IsDefined())
	{
		enemy_pool = SceneManagement::FindInCurrentScene(load_node["'enemy_pool@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'damage_effect_duration@float'"].IsDefined())
	{
		damage_effect_duration = load_node["'damage_effect_duration@float'"].as<float>();
	}

	if (load_node["'chasing_time_limit@float'"].IsDefined())
	{
		chasing_time_limit = load_node["'chasing_time_limit@float'"].as<float>();
	}

	if (load_node["'_rotating_lasers@GameObject*'"].IsDefined())
	{
		_rotating_lasers = SceneManagement::FindInCurrentScene(load_node["'_rotating_lasers@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_rotating_lasers_speed@float'"].IsDefined())
	{
		_rotating_lasers_speed = load_node["'_rotating_lasers_speed@float'"].as<float>();
	}

	if (load_node["'_laser_wall@GameObject*'"].IsDefined())
	{
		_laser_wall = SceneManagement::FindInCurrentScene(load_node["'_laser_wall@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_laser_wall_duration@float'"].IsDefined())
	{
		_laser_wall_duration = load_node["'_laser_wall_duration@float'"].as<float>();
	}

	if (load_node["'_laser_jump_height@float'"].IsDefined())
	{
		_laser_jump_height = load_node["'_laser_jump_height@float'"].as<float>();
	}

	if (load_node["'_melee_trail_right@GameObject*'"].IsDefined())
	{
		_melee_trail_right = SceneManagement::FindInCurrentScene(load_node["'_melee_trail_right@GameObject*'"].as<unsigned long long>());
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
	node["'_state_num@int'"] = _state_num;

	node["'_worm@bool'"] = _worm;

	node["'_aggro_range@int'"] = _aggro_range;

	node["'_attack_range@float'"] = _attack_range;

	node["'_attack_delay@float'"] = _attack_delay;

	node["'_idle_cooldown@float'"] = _idle_cooldown;

	node["'_patrol_cooldown@float'"] = _patrol_cooldown;

	node["'_spawning_time@float'"] = _spawning_time;

	node["'_chase_cooldown@float'"] = _chase_cooldown;

	node["'_spawn_pos@float3'"] = _spawn_pos;

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

	if (_projectile_particles != nullptr)
	{
		node["'_projectile_particles@GameObject*'"] = _projectile_particles->GetID();
	}
	else
	{
		node["'_projectile_particles@GameObject*'"] = 0;
	}

	if (_explosion_particles != nullptr)
	{
		node["'_explosion_particles@GameObject*'"] = _explosion_particles->GetID();
	}
	else
	{
		node["'_explosion_particles@GameObject*'"] = 0;
	}

	node["'_already_in_combat@bool'"] = _already_in_combat;

	node["'_is_from_gautlet@bool'"] = _is_from_gautlet;

	node["'_is_from_boss@bool'"] = _is_from_boss;

	node["'_will_die@bool'"] = _will_die;

	node["'_attack_animation_duration@float'"] = _attack_animation_duration;

	node["'_attack_animation_timer@float'"] = _attack_animation_timer;

	node["'damage_effect_duration@float'"] = damage_effect_duration;
}

void Hachiko::Scripting::EnemyController::OnLoad()
{
	if (load_node["'_state_num@int'"].IsDefined())
	{
		_state_num = load_node["'_state_num@int'"].as<int>();
	}

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

	if (load_node["'_patrol_cooldown@float'"].IsDefined())
	{
		_patrol_cooldown = load_node["'_patrol_cooldown@float'"].as<float>();
	}

	if (load_node["'_spawning_time@float'"].IsDefined())
	{
		_spawning_time = load_node["'_spawning_time@float'"].as<float>();
	}

	if (load_node["'_chase_cooldown@float'"].IsDefined())
	{
		_chase_cooldown = load_node["'_chase_cooldown@float'"].as<float>();
	}

	if (load_node["'_spawn_pos@float3'"].IsDefined())
	{
		_spawn_pos = load_node["'_spawn_pos@float3'"].as<float3>();
	}

	if (load_node["'_enemy_body@GameObject*'"].IsDefined())
	{
		_enemy_body = SceneManagement::FindInCurrentScene(load_node["'_enemy_body@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_parasite@GameObject*'"].IsDefined())
	{
		_parasite = SceneManagement::FindInCurrentScene(load_node["'_parasite@GameObject*'"].as<unsigned long long>());
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

	if (load_node["'_projectile_particles@GameObject*'"].IsDefined())
	{
		_projectile_particles = SceneManagement::FindInCurrentScene(load_node["'_projectile_particles@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_explosion_particles@GameObject*'"].IsDefined())
	{
		_explosion_particles = SceneManagement::FindInCurrentScene(load_node["'_explosion_particles@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_already_in_combat@bool'"].IsDefined())
	{
		_already_in_combat = load_node["'_already_in_combat@bool'"].as<bool>();
	}

	if (load_node["'_is_from_gautlet@bool'"].IsDefined())
	{
		_is_from_gautlet = load_node["'_is_from_gautlet@bool'"].as<bool>();
	}

	if (load_node["'_is_from_boss@bool'"].IsDefined())
	{
		_is_from_boss = load_node["'_is_from_boss@bool'"].as<bool>();
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

	if (load_node["'damage_effect_duration@float'"].IsDefined())
	{
		damage_effect_duration = load_node["'damage_effect_duration@float'"].as<float>();
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
}

void Hachiko::Scripting::CombatVisualEffectsPool::OnLoad()
{
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

	if (_hammer_weapon != nullptr)
	{
		node["'_hammer_weapon@GameObject*'"] = _hammer_weapon->GetID();
	}
	else
	{
		node["'_hammer_weapon@GameObject*'"] = 0;
	}

	if (_attack_indicator != nullptr)
	{
		node["'_attack_indicator@GameObject*'"] = _attack_indicator->GetID();
	}
	else
	{
		node["'_attack_indicator@GameObject*'"] = 0;
	}

	if (_aim_indicator != nullptr)
	{
		node["'_aim_indicator@GameObject*'"] = _aim_indicator->GetID();
	}
	else
	{
		node["'_aim_indicator@GameObject*'"] = 0;
	}

	if (_combat_manager != nullptr)
	{
		node["'_combat_manager@GameObject*'"] = _combat_manager->GetID();
	}
	else
	{
		node["'_combat_manager@GameObject*'"] = 0;
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

	if (_dash_trail_vfx != nullptr)
	{
		node["'_dash_trail_vfx@GameObject*'"] = _dash_trail_vfx->GetID();
	}
	else
	{
		node["'_dash_trail_vfx@GameObject*'"] = 0;
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

	if (_damage_effect != nullptr)
	{
		node["'_damage_effect@GameObject*'"] = _damage_effect->GetID();
	}
	else
	{
		node["'_damage_effect@GameObject*'"] = 0;
	}

	if (_parasite_pickup_effect != nullptr)
	{
		node["'_parasite_pickup_effect@GameObject*'"] = _parasite_pickup_effect->GetID();
	}
	else
	{
		node["'_parasite_pickup_effect@GameObject*'"] = 0;
	}

	if (_melee_trail_right != nullptr)
	{
		node["'_melee_trail_right@GameObject*'"] = _melee_trail_right->GetID();
	}
	else
	{
		node["'_melee_trail_right@GameObject*'"] = 0;
	}

	if (_melee_trail_left != nullptr)
	{
		node["'_melee_trail_left@GameObject*'"] = _melee_trail_left->GetID();
	}
	else
	{
		node["'_melee_trail_left@GameObject*'"] = 0;
	}

	if (_melee_trail_center != nullptr)
	{
		node["'_melee_trail_center@GameObject*'"] = _melee_trail_center->GetID();
	}
	else
	{
		node["'_melee_trail_center@GameObject*'"] = 0;
	}

	if (_claws_trail_right != nullptr)
	{
		node["'_claws_trail_right@GameObject*'"] = _claws_trail_right->GetID();
	}
	else
	{
		node["'_claws_trail_right@GameObject*'"] = 0;
	}

	if (_claws_trail_left != nullptr)
	{
		node["'_claws_trail_left@GameObject*'"] = _claws_trail_left->GetID();
	}
	else
	{
		node["'_claws_trail_left@GameObject*'"] = 0;
	}

	if (_claws_trail_center != nullptr)
	{
		node["'_claws_trail_center@GameObject*'"] = _claws_trail_center->GetID();
	}
	else
	{
		node["'_claws_trail_center@GameObject*'"] = 0;
	}

	if (_sword_trail_right != nullptr)
	{
		node["'_sword_trail_right@GameObject*'"] = _sword_trail_right->GetID();
	}
	else
	{
		node["'_sword_trail_right@GameObject*'"] = 0;
	}

	if (_sword_trail_left != nullptr)
	{
		node["'_sword_trail_left@GameObject*'"] = _sword_trail_left->GetID();
	}
	else
	{
		node["'_sword_trail_left@GameObject*'"] = 0;
	}

	if (_sword_trail_center != nullptr)
	{
		node["'_sword_trail_center@GameObject*'"] = _sword_trail_center->GetID();
	}
	else
	{
		node["'_sword_trail_center@GameObject*'"] = 0;
	}

	if (_hammer_trail_right != nullptr)
	{
		node["'_hammer_trail_right@GameObject*'"] = _hammer_trail_right->GetID();
	}
	else
	{
		node["'_hammer_trail_right@GameObject*'"] = 0;
	}

	if (_hammer_trail_left != nullptr)
	{
		node["'_hammer_trail_left@GameObject*'"] = _hammer_trail_left->GetID();
	}
	else
	{
		node["'_hammer_trail_left@GameObject*'"] = 0;
	}

	if (_hammer_trail_center != nullptr)
	{
		node["'_hammer_trail_center@GameObject*'"] = _hammer_trail_center->GetID();
	}
	else
	{
		node["'_hammer_trail_center@GameObject*'"] = 0;
	}

	node["'_rotation_duration@float'"] = _rotation_duration;

	if (_death_screen != nullptr)
	{
		node["'_death_screen@GameObject*'"] = _death_screen->GetID();
	}
	else
	{
		node["'_death_screen@GameObject*'"] = 0;
	}

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

	if (_hp_cell_extra != nullptr)
	{
		node["'_hp_cell_extra@GameObject*'"] = _hp_cell_extra->GetID();
	}
	else
	{
		node["'_hp_cell_extra@GameObject*'"] = 0;
	}

	if (_magic_parasyte != nullptr)
	{
		node["'_magic_parasyte@GameObject*'"] = _magic_parasyte->GetID();
	}
	else
	{
		node["'_magic_parasyte@GameObject*'"] = 0;
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

	if (_sword_ui_addon != nullptr)
	{
		node["'_sword_ui_addon@GameObject*'"] = _sword_ui_addon->GetID();
	}
	else
	{
		node["'_sword_ui_addon@GameObject*'"] = 0;
	}

	if (_maze_ui_addon != nullptr)
	{
		node["'_maze_ui_addon@GameObject*'"] = _maze_ui_addon->GetID();
	}
	else
	{
		node["'_maze_ui_addon@GameObject*'"] = 0;
	}

	if (_claw_ui_addon != nullptr)
	{
		node["'_claw_ui_addon@GameObject*'"] = _claw_ui_addon->GetID();
	}
	else
	{
		node["'_claw_ui_addon@GameObject*'"] = 0;
	}

	if (_weapon_charge_bar_go != nullptr)
	{
		node["'_weapon_charge_bar_go@GameObject*'"] = _weapon_charge_bar_go->GetID();
	}
	else
	{
		node["'_weapon_charge_bar_go@GameObject*'"] = 0;
	}

	if (_keyboard_tooltip_display != nullptr)
	{
		node["'_keyboard_tooltip_display@GameObject*'"] = _keyboard_tooltip_display->GetID();
	}
	else
	{
		node["'_keyboard_tooltip_display@GameObject*'"] = 0;
	}

	if (_controller_tooltip_display != nullptr)
	{
		node["'_controller_tooltip_display@GameObject*'"] = _controller_tooltip_display->GetID();
	}
	else
	{
		node["'_controller_tooltip_display@GameObject*'"] = 0;
	}

	node["'tooltip_y_offset@float'"] = tooltip_y_offset;

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

	node["'_heal_effect_fade_duration@float'"] = _heal_effect_fade_duration;

	node["'damage_effect_duration@float'"] = damage_effect_duration;
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

	if (load_node["'_hammer_weapon@GameObject*'"].IsDefined())
	{
		_hammer_weapon = SceneManagement::FindInCurrentScene(load_node["'_hammer_weapon@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_attack_indicator@GameObject*'"].IsDefined())
	{
		_attack_indicator = SceneManagement::FindInCurrentScene(load_node["'_attack_indicator@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_aim_indicator@GameObject*'"].IsDefined())
	{
		_aim_indicator = SceneManagement::FindInCurrentScene(load_node["'_aim_indicator@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_combat_manager@GameObject*'"].IsDefined())
	{
		_combat_manager = SceneManagement::FindInCurrentScene(load_node["'_combat_manager@GameObject*'"].as<unsigned long long>());
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

	if (load_node["'_dash_trail_vfx@GameObject*'"].IsDefined())
	{
		_dash_trail_vfx = SceneManagement::FindInCurrentScene(load_node["'_dash_trail_vfx@GameObject*'"].as<unsigned long long>());
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

	if (load_node["'_damage_effect@GameObject*'"].IsDefined())
	{
		_damage_effect = SceneManagement::FindInCurrentScene(load_node["'_damage_effect@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_parasite_pickup_effect@GameObject*'"].IsDefined())
	{
		_parasite_pickup_effect = SceneManagement::FindInCurrentScene(load_node["'_parasite_pickup_effect@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_melee_trail_right@GameObject*'"].IsDefined())
	{
		_melee_trail_right = SceneManagement::FindInCurrentScene(load_node["'_melee_trail_right@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_melee_trail_left@GameObject*'"].IsDefined())
	{
		_melee_trail_left = SceneManagement::FindInCurrentScene(load_node["'_melee_trail_left@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_melee_trail_center@GameObject*'"].IsDefined())
	{
		_melee_trail_center = SceneManagement::FindInCurrentScene(load_node["'_melee_trail_center@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_claws_trail_right@GameObject*'"].IsDefined())
	{
		_claws_trail_right = SceneManagement::FindInCurrentScene(load_node["'_claws_trail_right@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_claws_trail_left@GameObject*'"].IsDefined())
	{
		_claws_trail_left = SceneManagement::FindInCurrentScene(load_node["'_claws_trail_left@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_claws_trail_center@GameObject*'"].IsDefined())
	{
		_claws_trail_center = SceneManagement::FindInCurrentScene(load_node["'_claws_trail_center@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_sword_trail_right@GameObject*'"].IsDefined())
	{
		_sword_trail_right = SceneManagement::FindInCurrentScene(load_node["'_sword_trail_right@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_sword_trail_left@GameObject*'"].IsDefined())
	{
		_sword_trail_left = SceneManagement::FindInCurrentScene(load_node["'_sword_trail_left@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_sword_trail_center@GameObject*'"].IsDefined())
	{
		_sword_trail_center = SceneManagement::FindInCurrentScene(load_node["'_sword_trail_center@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_hammer_trail_right@GameObject*'"].IsDefined())
	{
		_hammer_trail_right = SceneManagement::FindInCurrentScene(load_node["'_hammer_trail_right@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_hammer_trail_left@GameObject*'"].IsDefined())
	{
		_hammer_trail_left = SceneManagement::FindInCurrentScene(load_node["'_hammer_trail_left@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_hammer_trail_center@GameObject*'"].IsDefined())
	{
		_hammer_trail_center = SceneManagement::FindInCurrentScene(load_node["'_hammer_trail_center@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_rotation_duration@float'"].IsDefined())
	{
		_rotation_duration = load_node["'_rotation_duration@float'"].as<float>();
	}

	if (load_node["'_death_screen@GameObject*'"].IsDefined())
	{
		_death_screen = SceneManagement::FindInCurrentScene(load_node["'_death_screen@GameObject*'"].as<unsigned long long>());
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

	if (load_node["'_hp_cell_extra@GameObject*'"].IsDefined())
	{
		_hp_cell_extra = SceneManagement::FindInCurrentScene(load_node["'_hp_cell_extra@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_magic_parasyte@GameObject*'"].IsDefined())
	{
		_magic_parasyte = SceneManagement::FindInCurrentScene(load_node["'_magic_parasyte@GameObject*'"].as<unsigned long long>());
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

	if (load_node["'_sword_ui_addon@GameObject*'"].IsDefined())
	{
		_sword_ui_addon = SceneManagement::FindInCurrentScene(load_node["'_sword_ui_addon@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_maze_ui_addon@GameObject*'"].IsDefined())
	{
		_maze_ui_addon = SceneManagement::FindInCurrentScene(load_node["'_maze_ui_addon@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_claw_ui_addon@GameObject*'"].IsDefined())
	{
		_claw_ui_addon = SceneManagement::FindInCurrentScene(load_node["'_claw_ui_addon@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_weapon_charge_bar_go@GameObject*'"].IsDefined())
	{
		_weapon_charge_bar_go = SceneManagement::FindInCurrentScene(load_node["'_weapon_charge_bar_go@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_keyboard_tooltip_display@GameObject*'"].IsDefined())
	{
		_keyboard_tooltip_display = SceneManagement::FindInCurrentScene(load_node["'_keyboard_tooltip_display@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_controller_tooltip_display@GameObject*'"].IsDefined())
	{
		_controller_tooltip_display = SceneManagement::FindInCurrentScene(load_node["'_controller_tooltip_display@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'tooltip_y_offset@float'"].IsDefined())
	{
		tooltip_y_offset = load_node["'tooltip_y_offset@float'"].as<float>();
	}

	if (load_node["'_camera@GameObject*'"].IsDefined())
	{
		_camera = SceneManagement::FindInCurrentScene(load_node["'_camera@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_ui_damage@GameObject*'"].IsDefined())
	{
		_ui_damage = SceneManagement::FindInCurrentScene(load_node["'_ui_damage@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_heal_effect_fade_duration@float'"].IsDefined())
	{
		_heal_effect_fade_duration = load_node["'_heal_effect_fade_duration@float'"].as<float>();
	}

	if (load_node["'damage_effect_duration@float'"].IsDefined())
	{
		damage_effect_duration = load_node["'damage_effect_duration@float'"].as<float>();
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

void Hachiko::Scripting::AssetsObstacle::OnSave(YAML::Node& node) const
{
}

void Hachiko::Scripting::AssetsObstacle::OnLoad()
{
}

void Hachiko::Scripting::AudioManager::OnSave(YAML::Node& node) const
{
}

void Hachiko::Scripting::AudioManager::OnLoad()
{
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

void Hachiko::Scripting::BossLaserController::OnSave(YAML::Node& node) const
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

	node["'_track_if_active@bool'"] = _track_if_active;

	node["'_tracking_speed@float'"] = _tracking_speed;

	node["'_toggle_activation@bool'"] = _toggle_activation;

	node["'_toggle_active_time@float'"] = _toggle_active_time;

	node["'_toggle_inactive_time@float'"] = _toggle_inactive_time;
}

void Hachiko::Scripting::BossLaserController::OnLoad()
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

	if (load_node["'_track_if_active@bool'"].IsDefined())
	{
		_track_if_active = load_node["'_track_if_active@bool'"].as<bool>();
	}

	if (load_node["'_tracking_speed@float'"].IsDefined())
	{
		_tracking_speed = load_node["'_tracking_speed@float'"].as<float>();
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

	node["'_do_mouse_movement@bool'"] = _do_mouse_movement;

	node["'_speed@float'"] = _speed;

	node["'_duration@float'"] = _duration;

	node["'_relative_position@float3'"] = _relative_position;

	node["'_rotation@float3'"] = _rotation;

	node["'_check_box@bool'"] = _check_box;
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

	if (load_node["'_do_mouse_movement@bool'"].IsDefined())
	{
		_do_mouse_movement = load_node["'_do_mouse_movement@bool'"].as<bool>();
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

	if (load_node["'_check_box@bool'"].IsDefined())
	{
		_check_box = load_node["'_check_box@bool'"].as<bool>();
	}
}

void Hachiko::Scripting::Centipedes::OnSave(YAML::Node& node) const
{
	if (_damage_effect != nullptr)
	{
		node["'_damage_effect@GameObject*'"] = _damage_effect->GetID();
	}
	else
	{
		node["'_damage_effect@GameObject*'"] = 0;
	}
}

void Hachiko::Scripting::Centipedes::OnLoad()
{
	if (load_node["'_damage_effect@GameObject*'"].IsDefined())
	{
		_damage_effect = SceneManagement::FindInCurrentScene(load_node["'_damage_effect@GameObject*'"].as<unsigned long long>());
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

	if (_enable_after != nullptr)
	{
		node["'_enable_after@GameObject*'"] = _enable_after->GetID();
	}
	else
	{
		node["'_enable_after@GameObject*'"] = 0;
	}
}

void Hachiko::Scripting::DynamicCamera::OnLoad()
{
	if (load_node["'_speed@float'"].IsDefined())
	{
		_speed = load_node["'_speed@float'"].as<float>();
	}

	if (load_node["'_enable_after@GameObject*'"].IsDefined())
	{
		_enable_after = SceneManagement::FindInCurrentScene(load_node["'_enable_after@GameObject*'"].as<unsigned long long>());
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

	if (_closing_door != nullptr)
	{
		node["'_closing_door@GameObject*'"] = _closing_door->GetID();
	}
	else
	{
		node["'_closing_door@GameObject*'"] = 0;
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

	if (_camera_anchor != nullptr)
	{
		node["'_camera_anchor@GameObject*'"] = _camera_anchor->GetID();
	}
	else
	{
		node["'_camera_anchor@GameObject*'"] = 0;
	}

	if (_central_anchor != nullptr)
	{
		node["'_central_anchor@GameObject*'"] = _central_anchor->GetID();
	}
	else
	{
		node["'_central_anchor@GameObject*'"] = 0;
	}

	node["'_relative_position@float3'"] = _relative_position;

	node["'_camera_follows_player@float'"] = _camera_follows_player;
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

	if (load_node["'_closing_door@GameObject*'"].IsDefined())
	{
		_closing_door = SceneManagement::FindInCurrentScene(load_node["'_closing_door@GameObject*'"].as<unsigned long long>());
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

	if (load_node["'_camera_anchor@GameObject*'"].IsDefined())
	{
		_camera_anchor = SceneManagement::FindInCurrentScene(load_node["'_camera_anchor@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_central_anchor@GameObject*'"].IsDefined())
	{
		_central_anchor = SceneManagement::FindInCurrentScene(load_node["'_central_anchor@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_relative_position@float3'"].IsDefined())
	{
		_relative_position = load_node["'_relative_position@float3'"].as<float3>();
	}

	if (load_node["'_camera_follows_player@float'"].IsDefined())
	{
		_camera_follows_player = load_node["'_camera_follows_player@float'"].as<float>();
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

	node["'_dissolving_time@float'"] = _dissolving_time;

	if (_sparks != nullptr)
	{
		node["'_sparks@GameObject*'"] = _sparks->GetID();
	}
	else
	{
		node["'_sparks@GameObject*'"] = 0;
	}
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

	if (load_node["'_dissolving_time@float'"].IsDefined())
	{
		_dissolving_time = load_node["'_dissolving_time@float'"].as<float>();
	}

	if (load_node["'_sparks@GameObject*'"].IsDefined())
	{
		_sparks = SceneManagement::FindInCurrentScene(load_node["'_sparks@GameObject*'"].as<unsigned long long>());
	}
}

void Hachiko::Scripting::LevelManager::OnSave(YAML::Node& node) const
{
	node["'_level@unsigned'"] = _level;

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

	node["'_gauntlets_easy_mode@bool'"] = _gauntlets_easy_mode;

	node["'_modify_fog@bool'"] = _modify_fog;

	node["'_fog_frequency@float'"] = _fog_frequency;

	node["'_fog_max_density@float'"] = _fog_max_density;

	node["'_fog_min_density@float'"] = _fog_min_density;

	if (_audio_manager_go != nullptr)
	{
		node["'_audio_manager_go@GameObject*'"] = _audio_manager_go->GetID();
	}
	else
	{
		node["'_audio_manager_go@GameObject*'"] = 0;
	}

	if (_player_sound_manager_go != nullptr)
	{
		node["'_player_sound_manager_go@GameObject*'"] = _player_sound_manager_go->GetID();
	}
	else
	{
		node["'_player_sound_manager_go@GameObject*'"] = 0;
	}

	if (_victory_screen != nullptr)
	{
		node["'_victory_screen@GameObject*'"] = _victory_screen->GetID();
	}
	else
	{
		node["'_victory_screen@GameObject*'"] = 0;
	}
}

void Hachiko::Scripting::LevelManager::OnLoad()
{
	if (load_node["'_level@unsigned'"].IsDefined())
	{
		_level = load_node["'_level@unsigned'"].as<unsigned>();
	}

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

	if (load_node["'_gauntlets_easy_mode@bool'"].IsDefined())
	{
		_gauntlets_easy_mode = load_node["'_gauntlets_easy_mode@bool'"].as<bool>();
	}

	if (load_node["'_modify_fog@bool'"].IsDefined())
	{
		_modify_fog = load_node["'_modify_fog@bool'"].as<bool>();
	}

	if (load_node["'_fog_frequency@float'"].IsDefined())
	{
		_fog_frequency = load_node["'_fog_frequency@float'"].as<float>();
	}

	if (load_node["'_fog_max_density@float'"].IsDefined())
	{
		_fog_max_density = load_node["'_fog_max_density@float'"].as<float>();
	}

	if (load_node["'_fog_min_density@float'"].IsDefined())
	{
		_fog_min_density = load_node["'_fog_min_density@float'"].as<float>();
	}

	if (load_node["'_audio_manager_go@GameObject*'"].IsDefined())
	{
		_audio_manager_go = SceneManagement::FindInCurrentScene(load_node["'_audio_manager_go@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_player_sound_manager_go@GameObject*'"].IsDefined())
	{
		_player_sound_manager_go = SceneManagement::FindInCurrentScene(load_node["'_player_sound_manager_go@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_victory_screen@GameObject*'"].IsDefined())
	{
		_victory_screen = SceneManagement::FindInCurrentScene(load_node["'_victory_screen@GameObject*'"].as<unsigned long long>());
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

	if (_light_go != nullptr)
	{
		node["'_light_go@GameObject*'"] = _light_go->GetID();
	}
	else
	{
		node["'_light_go@GameObject*'"] = 0;
	}
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

	if (load_node["'_light_go@GameObject*'"].IsDefined())
	{
		_light_go = SceneManagement::FindInCurrentScene(load_node["'_light_go@GameObject*'"].as<unsigned long long>());
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

void Hachiko::Scripting::StalagmiteManager::OnSave(YAML::Node& node) const
{
	node["'_falling_cooldown@float'"] = _falling_cooldown;
}

void Hachiko::Scripting::StalagmiteManager::OnLoad()
{
	if (load_node["'_falling_cooldown@float'"].IsDefined())
	{
		_falling_cooldown = load_node["'_falling_cooldown@float'"].as<float>();
	}
}

void Hachiko::Scripting::TimeManager::OnSave(YAML::Node& node) const
{
}

void Hachiko::Scripting::TimeManager::OnLoad()
{
}

void Hachiko::Scripting::Tutorial::OnSave(YAML::Node& node) const
{
	if (_tutorial_keyboard_movement != nullptr)
	{
		node["'_tutorial_keyboard_movement@GameObject*'"] = _tutorial_keyboard_movement->GetID();
	}
	else
	{
		node["'_tutorial_keyboard_movement@GameObject*'"] = 0;
	}

	if (_tutorial_controller_movement != nullptr)
	{
		node["'_tutorial_controller_movement@GameObject*'"] = _tutorial_controller_movement->GetID();
	}
	else
	{
		node["'_tutorial_controller_movement@GameObject*'"] = 0;
	}

	if (_tutorial_keyboard_attacks != nullptr)
	{
		node["'_tutorial_keyboard_attacks@GameObject*'"] = _tutorial_keyboard_attacks->GetID();
	}
	else
	{
		node["'_tutorial_keyboard_attacks@GameObject*'"] = 0;
	}

	if (_tutorial_controller_attacks != nullptr)
	{
		node["'_tutorial_controller_attacks@GameObject*'"] = _tutorial_controller_attacks->GetID();
	}
	else
	{
		node["'_tutorial_controller_attacks@GameObject*'"] = 0;
	}

	if (_tutorial_keyboard_pickup != nullptr)
	{
		node["'_tutorial_keyboard_pickup@GameObject*'"] = _tutorial_keyboard_pickup->GetID();
	}
	else
	{
		node["'_tutorial_keyboard_pickup@GameObject*'"] = 0;
	}

	if (_tutorial_controller_pickup != nullptr)
	{
		node["'_tutorial_controller_pickup@GameObject*'"] = _tutorial_controller_pickup->GetID();
	}
	else
	{
		node["'_tutorial_controller_pickup@GameObject*'"] = 0;
	}

	if (_first_enemy != nullptr)
	{
		node["'_first_enemy@GameObject*'"] = _first_enemy->GetID();
	}
	else
	{
		node["'_first_enemy@GameObject*'"] = 0;
	}

	node["'_tutorials_screen_shown@int'"] = _tutorials_screen_shown;
}

void Hachiko::Scripting::Tutorial::OnLoad()
{
	if (load_node["'_tutorial_keyboard_movement@GameObject*'"].IsDefined())
	{
		_tutorial_keyboard_movement = SceneManagement::FindInCurrentScene(load_node["'_tutorial_keyboard_movement@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_tutorial_controller_movement@GameObject*'"].IsDefined())
	{
		_tutorial_controller_movement = SceneManagement::FindInCurrentScene(load_node["'_tutorial_controller_movement@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_tutorial_keyboard_attacks@GameObject*'"].IsDefined())
	{
		_tutorial_keyboard_attacks = SceneManagement::FindInCurrentScene(load_node["'_tutorial_keyboard_attacks@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_tutorial_controller_attacks@GameObject*'"].IsDefined())
	{
		_tutorial_controller_attacks = SceneManagement::FindInCurrentScene(load_node["'_tutorial_controller_attacks@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_tutorial_keyboard_pickup@GameObject*'"].IsDefined())
	{
		_tutorial_keyboard_pickup = SceneManagement::FindInCurrentScene(load_node["'_tutorial_keyboard_pickup@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_tutorial_controller_pickup@GameObject*'"].IsDefined())
	{
		_tutorial_controller_pickup = SceneManagement::FindInCurrentScene(load_node["'_tutorial_controller_pickup@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_first_enemy@GameObject*'"].IsDefined())
	{
		_first_enemy = SceneManagement::FindInCurrentScene(load_node["'_first_enemy@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_tutorials_screen_shown@int'"].IsDefined())
	{
		_tutorials_screen_shown = load_node["'_tutorials_screen_shown@int'"].as<int>();
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

	if (_weapon_claws != nullptr && _weapon_claws->GetGameObject() != nullptr)
	{
		node["'_weapon_claws@ComponentButton*'"] = _weapon_claws->GetGameObject()->GetID();
	}
	else
	{
		node["'_weapon_claws@ComponentButton*'"] = 0;
	}

	if (_weapon_sword != nullptr && _weapon_sword->GetGameObject() != nullptr)
	{
		node["'_weapon_sword@ComponentButton*'"] = _weapon_sword->GetGameObject()->GetID();
	}
	else
	{
		node["'_weapon_sword@ComponentButton*'"] = 0;
	}

	if (_weapon_hammer != nullptr && _weapon_hammer->GetGameObject() != nullptr)
	{
		node["'_weapon_hammer@ComponentButton*'"] = _weapon_hammer->GetGameObject()->GetID();
	}
	else
	{
		node["'_weapon_hammer@ComponentButton*'"] = 0;
	}

	if (_reload_ammo != nullptr && _reload_ammo->GetGameObject() != nullptr)
	{
		node["'_reload_ammo@ComponentButton*'"] = _reload_ammo->GetGameObject()->GetID();
	}
	else
	{
		node["'_reload_ammo@ComponentButton*'"] = 0;
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

	if (_text_atck != nullptr && _text_atck->GetGameObject() != nullptr)
	{
		node["'_text_atck@ComponentText*'"] = _text_atck->GetGameObject()->GetID();
	}
	else
	{
		node["'_text_atck@ComponentText*'"] = 0;
	}

	if (_text_mvspd != nullptr && _text_mvspd->GetGameObject() != nullptr)
	{
		node["'_text_mvspd@ComponentText*'"] = _text_mvspd->GetGameObject()->GetID();
	}
	else
	{
		node["'_text_mvspd@ComponentText*'"] = 0;
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

	if (_player_stats != nullptr)
	{
		node["'_player_stats@GameObject*'"] = _player_stats->GetID();
	}
	else
	{
		node["'_player_stats@GameObject*'"] = 0;
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

	if (load_node["'_weapon_claws@ComponentButton*'"].IsDefined())
	{
		GameObject* _weapon_claws_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_weapon_claws@ComponentButton*'"].as<unsigned long long>());
		if (_weapon_claws_owner__temp != nullptr)
		{
			_weapon_claws = _weapon_claws_owner__temp->GetComponent<ComponentButton>();
		}
	}

	if (load_node["'_weapon_sword@ComponentButton*'"].IsDefined())
	{
		GameObject* _weapon_sword_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_weapon_sword@ComponentButton*'"].as<unsigned long long>());
		if (_weapon_sword_owner__temp != nullptr)
		{
			_weapon_sword = _weapon_sword_owner__temp->GetComponent<ComponentButton>();
		}
	}

	if (load_node["'_weapon_hammer@ComponentButton*'"].IsDefined())
	{
		GameObject* _weapon_hammer_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_weapon_hammer@ComponentButton*'"].as<unsigned long long>());
		if (_weapon_hammer_owner__temp != nullptr)
		{
			_weapon_hammer = _weapon_hammer_owner__temp->GetComponent<ComponentButton>();
		}
	}

	if (load_node["'_reload_ammo@ComponentButton*'"].IsDefined())
	{
		GameObject* _reload_ammo_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_reload_ammo@ComponentButton*'"].as<unsigned long long>());
		if (_reload_ammo_owner__temp != nullptr)
		{
			_reload_ammo = _reload_ammo_owner__temp->GetComponent<ComponentButton>();
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

	if (load_node["'_text_atck@ComponentText*'"].IsDefined())
	{
		GameObject* _text_atck_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_text_atck@ComponentText*'"].as<unsigned long long>());
		if (_text_atck_owner__temp != nullptr)
		{
			_text_atck = _text_atck_owner__temp->GetComponent<ComponentText>();
		}
	}

	if (load_node["'_text_mvspd@ComponentText*'"].IsDefined())
	{
		GameObject* _text_mvspd_owner__temp = SceneManagement::FindInCurrentScene(load_node["'_text_mvspd@ComponentText*'"].as<unsigned long long>());
		if (_text_mvspd_owner__temp != nullptr)
		{
			_text_mvspd = _text_mvspd_owner__temp->GetComponent<ComponentText>();
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

	if (load_node["'_player_stats@GameObject*'"].IsDefined())
	{
		_player_stats = SceneManagement::FindInCurrentScene(load_node["'_player_stats@GameObject*'"].as<unsigned long long>());
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