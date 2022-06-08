#include "scriptingUtil/gameplaypch.h"
#include <yaml-cpp/yaml.h>
#include <core/serialization/TypeConverter.h>
#include "entities/Stats.h"
#include "entities/crystals/CrystalExplosion.h"
#include "entities/enemies/BugAnimationManager.h"
#include "entities/enemies/EnemyController.h"
#include "entities/player/BulletController.h"
#include "entities/player/PlayerAnimationManager.h"
#include "entities/player/PlayerCamera.h"
#include "entities/player/PlayerController.h"
#include "entities/player/PlayerSoundManager.h"
#include "entities/player/RoomTeleporter.h"
#include "misc/DynamicCamera.h"
#include "misc/FancyLights.h"
#include "ui/BackToMainMenu.h"
#include "ui/DebugManager.h"
#include "ui/MainMenuManager.h"



void Hachiko::Scripting::Stats::OnSave(YAML::Node& node) const
{
	node["'_attack_power@int'"] = _attack_power;

	node["'_attack_cd@int'"] = _attack_cd;

	node["'_attack_range@float'"] = _attack_range;

	node["'_move_speed@float'"] = _move_speed;

	node["'_max_hp@int'"] = _max_hp;
}

void Hachiko::Scripting::Stats::OnLoad()
{
	if (load_node["'_attack_power@int'"].IsDefined())
	{
		_attack_power = load_node["'_attack_power@int'"].as<int>();
	}

	if (load_node["'_attack_cd@int'"].IsDefined())
	{
		_attack_cd = load_node["'_attack_cd@int'"].as<int>();
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
}

void Hachiko::Scripting::CrystalExplosion::OnSave(YAML::Node& node) const
{
	if (_player != nullptr)
	{
		node["'_player@GameObject*'"] = _player->GetID();
	}
	else
	{
		node["'_player@GameObject*'"] = 0;
	}

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

	node["'_crashing_index@unsigned'"] = _crashing_index;

	node["'_detecting_radius@float'"] = _detecting_radius;

	node["'_explosion_radius@float'"] = _explosion_radius;

	node["'_explosive_crystal@bool'"] = _explosive_crystal;
}

void Hachiko::Scripting::CrystalExplosion::OnLoad()
{
	if (load_node["'_player@GameObject*'"].IsDefined())
	{
		_player = SceneManagement::FindInCurrentScene(load_node["'_player@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_explosion_crystal@GameObject*'"].IsDefined())
	{
		_explosion_crystal = SceneManagement::FindInCurrentScene(load_node["'_explosion_crystal@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_static_crystal@GameObject*'"].IsDefined())
	{
		_static_crystal = SceneManagement::FindInCurrentScene(load_node["'_static_crystal@GameObject*'"].as<unsigned long long>());
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

	if (load_node["'_explosive_crystal@bool'"].IsDefined())
	{
		_explosive_crystal = load_node["'_explosive_crystal@bool'"].as<bool>();
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
	node["'_aggro_range@int'"] = _aggro_range;

	node["'_attack_range@int'"] = _attack_range;

	node["'_spawn_pos@float3'"] = _spawn_pos;

	node["'_spawn_is_initial@bool'"] = _spawn_is_initial;

	if (_player != nullptr)
	{
		node["'_player@GameObject*'"] = _player->GetID();
	}
	else
	{
		node["'_player@GameObject*'"] = 0;
	}

	node["'_attack_animation_duration@float'"] = _attack_animation_duration;

	node["'_attack_animation_timer@float'"] = _attack_animation_timer;
}

void Hachiko::Scripting::EnemyController::OnLoad()
{
	if (load_node["'_aggro_range@int'"].IsDefined())
	{
		_aggro_range = load_node["'_aggro_range@int'"].as<int>();
	}

	if (load_node["'_attack_range@int'"].IsDefined())
	{
		_attack_range = load_node["'_attack_range@int'"].as<int>();
	}

	if (load_node["'_spawn_pos@float3'"].IsDefined())
	{
		_spawn_pos = load_node["'_spawn_pos@float3'"].as<float3>();
	}

	if (load_node["'_spawn_is_initial@bool'"].IsDefined())
	{
		_spawn_is_initial = load_node["'_spawn_is_initial@bool'"].as<bool>();
	}

	if (load_node["'_player@GameObject*'"].IsDefined())
	{
		_player = SceneManagement::FindInCurrentScene(load_node["'_player@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_attack_animation_duration@float'"].IsDefined())
	{
		_attack_animation_duration = load_node["'_attack_animation_duration@float'"].as<float>();
	}

	if (load_node["'_attack_animation_timer@float'"].IsDefined())
	{
		_attack_animation_timer = load_node["'_attack_animation_timer@float'"].as<float>();
	}
}

void Hachiko::Scripting::BulletController::OnSave(YAML::Node& node) const
{
	node["'_move_speed@float'"] = _move_speed;

	node["'_lifetime@float'"] = _lifetime;

	node["'_collider_radius@float'"] = _collider_radius;
}

void Hachiko::Scripting::BulletController::OnLoad()
{
	if (load_node["'_move_speed@float'"].IsDefined())
	{
		_move_speed = load_node["'_move_speed@float'"].as<float>();
	}

	if (load_node["'_lifetime@float'"].IsDefined())
	{
		_lifetime = load_node["'_lifetime@float'"].as<float>();
	}

	if (load_node["'_collider_radius@float'"].IsDefined())
	{
		_collider_radius = load_node["'_collider_radius@float'"].as<float>();
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

	if (_player != nullptr)
	{
		node["'_player@GameObject*'"] = _player->GetID();
	}
	else
	{
		node["'_player@GameObject*'"] = 0;
	}

	node["'_follow_delay@float'"] = _follow_delay;
}

void Hachiko::Scripting::PlayerCamera::OnLoad()
{
	if (load_node["'_relative_position_to_player@math::float3'"].IsDefined())
	{
		_relative_position_to_player = load_node["'_relative_position_to_player@math::float3'"].as<math::float3>();
	}

	if (load_node["'_player@GameObject*'"].IsDefined())
	{
		_player = SceneManagement::FindInCurrentScene(load_node["'_player@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_follow_delay@float'"].IsDefined())
	{
		_follow_delay = load_node["'_follow_delay@float'"].as<float>();
	}
}

void Hachiko::Scripting::PlayerController::OnSave(YAML::Node& node) const
{
	if (_attack_indicator != nullptr)
	{
		node["'_attack_indicator@GameObject*'"] = _attack_indicator->GetID();
	}
	else
	{
		node["'_attack_indicator@GameObject*'"] = 0;
	}

	if (_goal != nullptr)
	{
		node["'_goal@GameObject*'"] = _goal->GetID();
	}
	else
	{
		node["'_goal@GameObject*'"] = 0;
	}

	node["'_dash_duration@float'"] = _dash_duration;

	node["'_dash_distance@float'"] = _dash_distance;

	node["'_dash_cooldown@float'"] = _dash_cooldown;

	node["'_max_dash_charges@int'"] = _max_dash_charges;

	node["'_attack_duration@float'"] = _attack_duration;

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
	if (load_node["'_attack_indicator@GameObject*'"].IsDefined())
	{
		_attack_indicator = SceneManagement::FindInCurrentScene(load_node["'_attack_indicator@GameObject*'"].as<unsigned long long>());
	}

	if (load_node["'_goal@GameObject*'"].IsDefined())
	{
		_goal = SceneManagement::FindInCurrentScene(load_node["'_goal@GameObject*'"].as<unsigned long long>());
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

	if (load_node["'_max_dash_charges@int'"].IsDefined())
	{
		_max_dash_charges = load_node["'_max_dash_charges@int'"].as<int>();
	}

	if (load_node["'_attack_duration@float'"].IsDefined())
	{
		_attack_duration = load_node["'_attack_duration@float'"].as<float>();
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

void Hachiko::Scripting::DynamicCamera::OnSave(YAML::Node& node) const
{
	node["'_start_point@math::float3'"] = _start_point;

	node["'_end_point@math::float3'"] = _end_point;

	node["'_speed@float'"] = _speed;

	node["'_lerp_position@float'"] = _lerp_position;
}

void Hachiko::Scripting::DynamicCamera::OnLoad()
{
	if (load_node["'_start_point@math::float3'"].IsDefined())
	{
		_start_point = load_node["'_start_point@math::float3'"].as<math::float3>();
	}

	if (load_node["'_end_point@math::float3'"].IsDefined())
	{
		_end_point = load_node["'_end_point@math::float3'"].as<math::float3>();
	}

	if (load_node["'_speed@float'"].IsDefined())
	{
		_speed = load_node["'_speed@float'"].as<float>();
	}

	if (load_node["'_lerp_position@float'"].IsDefined())
	{
		_lerp_position = load_node["'_lerp_position@float'"].as<float>();
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