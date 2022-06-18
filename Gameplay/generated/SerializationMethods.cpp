#include <vector>
#include <string>
#include <unordered_map>
#include "scriptingUtil/gameplaypch.h"
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


void Hachiko::Scripting::Stats::DeserializeFrom(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::DeserializeFrom(serialized_fields);

	if(serialized_fields.find("_attack_power") != serialized_fields.end())
	{
		const SerializedField& _attack_power_sf = serialized_fields["_attack_power"];
		if (_attack_power_sf.type_name == "int")
		{
			_attack_power = std::any_cast<int>(_attack_power_sf.copy);
		}
	}

	if(serialized_fields.find("_attack_cd") != serialized_fields.end())
	{
		const SerializedField& _attack_cd_sf = serialized_fields["_attack_cd"];
		if (_attack_cd_sf.type_name == "int")
		{
			_attack_cd = std::any_cast<int>(_attack_cd_sf.copy);
		}
	}

	if(serialized_fields.find("_attack_range") != serialized_fields.end())
	{
		const SerializedField& _attack_range_sf = serialized_fields["_attack_range"];
		if (_attack_range_sf.type_name == "float")
		{
			_attack_range = std::any_cast<float>(_attack_range_sf.copy);
		}
	}

	if(serialized_fields.find("_move_speed") != serialized_fields.end())
	{
		const SerializedField& _move_speed_sf = serialized_fields["_move_speed"];
		if (_move_speed_sf.type_name == "float")
		{
			_move_speed = std::any_cast<float>(_move_speed_sf.copy);
		}
	}

	if(serialized_fields.find("_max_hp") != serialized_fields.end())
	{
		const SerializedField& _max_hp_sf = serialized_fields["_max_hp"];
		if (_max_hp_sf.type_name == "int")
		{
			_max_hp = std::any_cast<int>(_max_hp_sf.copy);
		}
	}
}

void Hachiko::Scripting::Stats::SerializeTo(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::SerializeTo(serialized_fields);

	serialized_fields["_attack_power"] = SerializedField(std::string("_attack_power"), std::make_any<int>(_attack_power), std::string("int"));

	serialized_fields["_attack_cd"] = SerializedField(std::string("_attack_cd"), std::make_any<int>(_attack_cd), std::string("int"));

	serialized_fields["_attack_range"] = SerializedField(std::string("_attack_range"), std::make_any<float>(_attack_range), std::string("float"));

	serialized_fields["_move_speed"] = SerializedField(std::string("_move_speed"), std::make_any<float>(_move_speed), std::string("float"));

	serialized_fields["_max_hp"] = SerializedField(std::string("_max_hp"), std::make_any<int>(_max_hp), std::string("int"));
}

void Hachiko::Scripting::CrystalExplosion::DeserializeFrom(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::DeserializeFrom(serialized_fields);

	if(serialized_fields.find("_player") != serialized_fields.end())
	{
		const SerializedField& _player_sf = serialized_fields["_player"];
		if (_player_sf.type_name == "GameObject*")
		{
			_player = std::any_cast<GameObject*>(_player_sf.copy);
		}
	}

	if(serialized_fields.find("_explosion_crystal") != serialized_fields.end())
	{
		const SerializedField& _explosion_crystal_sf = serialized_fields["_explosion_crystal"];
		if (_explosion_crystal_sf.type_name == "GameObject*")
		{
			_explosion_crystal = std::any_cast<GameObject*>(_explosion_crystal_sf.copy);
		}
	}

	if(serialized_fields.find("_static_crystal") != serialized_fields.end())
	{
		const SerializedField& _static_crystal_sf = serialized_fields["_static_crystal"];
		if (_static_crystal_sf.type_name == "GameObject*")
		{
			_static_crystal = std::any_cast<GameObject*>(_static_crystal_sf.copy);
		}
	}

	if(serialized_fields.find("_crashing_index") != serialized_fields.end())
	{
		const SerializedField& _crashing_index_sf = serialized_fields["_crashing_index"];
		if (_crashing_index_sf.type_name == "unsigned")
		{
			_crashing_index = std::any_cast<unsigned>(_crashing_index_sf.copy);
		}
	}

	if(serialized_fields.find("_detecting_radius") != serialized_fields.end())
	{
		const SerializedField& _detecting_radius_sf = serialized_fields["_detecting_radius"];
		if (_detecting_radius_sf.type_name == "float")
		{
			_detecting_radius = std::any_cast<float>(_detecting_radius_sf.copy);
		}
	}

	if(serialized_fields.find("_explosion_radius") != serialized_fields.end())
	{
		const SerializedField& _explosion_radius_sf = serialized_fields["_explosion_radius"];
		if (_explosion_radius_sf.type_name == "float")
		{
			_explosion_radius = std::any_cast<float>(_explosion_radius_sf.copy);
		}
	}

	if(serialized_fields.find("_explosive_crystal") != serialized_fields.end())
	{
		const SerializedField& _explosive_crystal_sf = serialized_fields["_explosive_crystal"];
		if (_explosive_crystal_sf.type_name == "bool")
		{
			_explosive_crystal = std::any_cast<bool>(_explosive_crystal_sf.copy);
		}
	}
}

void Hachiko::Scripting::CrystalExplosion::SerializeTo(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::SerializeTo(serialized_fields);

	serialized_fields["_player"] = SerializedField(std::string("_player"), std::make_any<GameObject*>(_player), std::string("GameObject*"));

	serialized_fields["_explosion_crystal"] = SerializedField(std::string("_explosion_crystal"), std::make_any<GameObject*>(_explosion_crystal), std::string("GameObject*"));

	serialized_fields["_static_crystal"] = SerializedField(std::string("_static_crystal"), std::make_any<GameObject*>(_static_crystal), std::string("GameObject*"));

	serialized_fields["_crashing_index"] = SerializedField(std::string("_crashing_index"), std::make_any<unsigned>(_crashing_index), std::string("unsigned"));

	serialized_fields["_detecting_radius"] = SerializedField(std::string("_detecting_radius"), std::make_any<float>(_detecting_radius), std::string("float"));

	serialized_fields["_explosion_radius"] = SerializedField(std::string("_explosion_radius"), std::make_any<float>(_explosion_radius), std::string("float"));

	serialized_fields["_explosive_crystal"] = SerializedField(std::string("_explosive_crystal"), std::make_any<bool>(_explosive_crystal), std::string("bool"));
}

void Hachiko::Scripting::BugAnimationManager::DeserializeFrom(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::DeserializeFrom(serialized_fields);

	if(serialized_fields.find("_animator") != serialized_fields.end())
	{
		const SerializedField& _animator_sf = serialized_fields["_animator"];
		if (_animator_sf.type_name == "ComponentAnimation*")
		{
			_animator = std::any_cast<ComponentAnimation*>(_animator_sf.copy);
		}
	}

	if(serialized_fields.find("_previous_state") != serialized_fields.end())
	{
		const SerializedField& _previous_state_sf = serialized_fields["_previous_state"];
		if (_previous_state_sf.type_name == "BugState")
		{
			_previous_state = std::any_cast<BugState>(_previous_state_sf.copy);
		}
	}

	if(serialized_fields.find("_state_string") != serialized_fields.end())
	{
		const SerializedField& _state_string_sf = serialized_fields["_state_string"];
		if (_state_string_sf.type_name == "std::string")
		{
			_state_string = std::any_cast<std::string>(_state_string_sf.copy);
		}
	}

	if(serialized_fields.find("_idle_index") != serialized_fields.end())
	{
		const SerializedField& _idle_index_sf = serialized_fields["_idle_index"];
		if (_idle_index_sf.type_name == "unsigned")
		{
			_idle_index = std::any_cast<unsigned>(_idle_index_sf.copy);
		}
	}

	if(serialized_fields.find("_attacking_index") != serialized_fields.end())
	{
		const SerializedField& _attacking_index_sf = serialized_fields["_attacking_index"];
		if (_attacking_index_sf.type_name == "unsigned")
		{
			_attacking_index = std::any_cast<unsigned>(_attacking_index_sf.copy);
		}
	}
}

void Hachiko::Scripting::BugAnimationManager::SerializeTo(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::SerializeTo(serialized_fields);

	serialized_fields["_animator"] = SerializedField(std::string("_animator"), std::make_any<ComponentAnimation*>(_animator), std::string("ComponentAnimation*"));

	serialized_fields["_previous_state"] = SerializedField(std::string("_previous_state"), std::make_any<BugState>(_previous_state), std::string("BugState"));

	serialized_fields["_state_string"] = SerializedField(std::string("_state_string"), std::make_any<std::string>(_state_string), std::string("std::string"));

	serialized_fields["_idle_index"] = SerializedField(std::string("_idle_index"), std::make_any<unsigned>(_idle_index), std::string("unsigned"));

	serialized_fields["_attacking_index"] = SerializedField(std::string("_attacking_index"), std::make_any<unsigned>(_attacking_index), std::string("unsigned"));
}

void Hachiko::Scripting::EnemyController::DeserializeFrom(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::DeserializeFrom(serialized_fields);

	if(serialized_fields.find("_aggro_range") != serialized_fields.end())
	{
		const SerializedField& _aggro_range_sf = serialized_fields["_aggro_range"];
		if (_aggro_range_sf.type_name == "int")
		{
			_aggro_range = std::any_cast<int>(_aggro_range_sf.copy);
		}
	}

	if(serialized_fields.find("_attack_range") != serialized_fields.end())
	{
		const SerializedField& _attack_range_sf = serialized_fields["_attack_range"];
		if (_attack_range_sf.type_name == "int")
		{
			_attack_range = std::any_cast<int>(_attack_range_sf.copy);
		}
	}

	if(serialized_fields.find("_spawn_pos") != serialized_fields.end())
	{
		const SerializedField& _spawn_pos_sf = serialized_fields["_spawn_pos"];
		if (_spawn_pos_sf.type_name == "float3")
		{
			_spawn_pos = std::any_cast<float3>(_spawn_pos_sf.copy);
		}
	}

	if(serialized_fields.find("_spawn_is_initial") != serialized_fields.end())
	{
		const SerializedField& _spawn_is_initial_sf = serialized_fields["_spawn_is_initial"];
		if (_spawn_is_initial_sf.type_name == "bool")
		{
			_spawn_is_initial = std::any_cast<bool>(_spawn_is_initial_sf.copy);
		}
	}

	if(serialized_fields.find("_player") != serialized_fields.end())
	{
		const SerializedField& _player_sf = serialized_fields["_player"];
		if (_player_sf.type_name == "GameObject*")
		{
			_player = std::any_cast<GameObject*>(_player_sf.copy);
		}
	}

	if(serialized_fields.find("_state") != serialized_fields.end())
	{
		const SerializedField& _state_sf = serialized_fields["_state"];
		if (_state_sf.type_name == "BugState")
		{
			_state = std::any_cast<BugState>(_state_sf.copy);
		}
	}

	if(serialized_fields.find("_attack_animation_duration") != serialized_fields.end())
	{
		const SerializedField& _attack_animation_duration_sf = serialized_fields["_attack_animation_duration"];
		if (_attack_animation_duration_sf.type_name == "float")
		{
			_attack_animation_duration = std::any_cast<float>(_attack_animation_duration_sf.copy);
		}
	}

	if(serialized_fields.find("_attack_animation_timer") != serialized_fields.end())
	{
		const SerializedField& _attack_animation_timer_sf = serialized_fields["_attack_animation_timer"];
		if (_attack_animation_timer_sf.type_name == "float")
		{
			_attack_animation_timer = std::any_cast<float>(_attack_animation_timer_sf.copy);
		}
	}
}

void Hachiko::Scripting::EnemyController::SerializeTo(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::SerializeTo(serialized_fields);

	serialized_fields["_aggro_range"] = SerializedField(std::string("_aggro_range"), std::make_any<int>(_aggro_range), std::string("int"));

	serialized_fields["_attack_range"] = SerializedField(std::string("_attack_range"), std::make_any<int>(_attack_range), std::string("int"));

	serialized_fields["_spawn_pos"] = SerializedField(std::string("_spawn_pos"), std::make_any<float3>(_spawn_pos), std::string("float3"));

	serialized_fields["_spawn_is_initial"] = SerializedField(std::string("_spawn_is_initial"), std::make_any<bool>(_spawn_is_initial), std::string("bool"));

	serialized_fields["_player"] = SerializedField(std::string("_player"), std::make_any<GameObject*>(_player), std::string("GameObject*"));

	serialized_fields["_state"] = SerializedField(std::string("_state"), std::make_any<BugState>(_state), std::string("BugState"));

	serialized_fields["_attack_animation_duration"] = SerializedField(std::string("_attack_animation_duration"), std::make_any<float>(_attack_animation_duration), std::string("float"));

	serialized_fields["_attack_animation_timer"] = SerializedField(std::string("_attack_animation_timer"), std::make_any<float>(_attack_animation_timer), std::string("float"));
}

void Hachiko::Scripting::BulletController::DeserializeFrom(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::DeserializeFrom(serialized_fields);
}

void Hachiko::Scripting::BulletController::SerializeTo(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::SerializeTo(serialized_fields);
}

void Hachiko::Scripting::PlayerAnimationManager::DeserializeFrom(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::DeserializeFrom(serialized_fields);

	if(serialized_fields.find("_animator") != serialized_fields.end())
	{
		const SerializedField& _animator_sf = serialized_fields["_animator"];
		if (_animator_sf.type_name == "ComponentAnimation*")
		{
			_animator = std::any_cast<ComponentAnimation*>(_animator_sf.copy);
		}
	}

	if(serialized_fields.find("_previous_state") != serialized_fields.end())
	{
		const SerializedField& _previous_state_sf = serialized_fields["_previous_state"];
		if (_previous_state_sf.type_name == "PlayerState")
		{
			_previous_state = std::any_cast<PlayerState>(_previous_state_sf.copy);
		}
	}

	if(serialized_fields.find("_state_string") != serialized_fields.end())
	{
		const SerializedField& _state_string_sf = serialized_fields["_state_string"];
		if (_state_string_sf.type_name == "std::string")
		{
			_state_string = std::any_cast<std::string>(_state_string_sf.copy);
		}
	}

	if(serialized_fields.find("_idle_index") != serialized_fields.end())
	{
		const SerializedField& _idle_index_sf = serialized_fields["_idle_index"];
		if (_idle_index_sf.type_name == "unsigned")
		{
			_idle_index = std::any_cast<unsigned>(_idle_index_sf.copy);
		}
	}

	if(serialized_fields.find("_walking_index") != serialized_fields.end())
	{
		const SerializedField& _walking_index_sf = serialized_fields["_walking_index"];
		if (_walking_index_sf.type_name == "unsigned")
		{
			_walking_index = std::any_cast<unsigned>(_walking_index_sf.copy);
		}
	}

	if(serialized_fields.find("_dashing_index") != serialized_fields.end())
	{
		const SerializedField& _dashing_index_sf = serialized_fields["_dashing_index"];
		if (_dashing_index_sf.type_name == "unsigned")
		{
			_dashing_index = std::any_cast<unsigned>(_dashing_index_sf.copy);
		}
	}

	if(serialized_fields.find("_melee_index") != serialized_fields.end())
	{
		const SerializedField& _melee_index_sf = serialized_fields["_melee_index"];
		if (_melee_index_sf.type_name == "unsigned")
		{
			_melee_index = std::any_cast<unsigned>(_melee_index_sf.copy);
		}
	}

	if(serialized_fields.find("_ranged_index") != serialized_fields.end())
	{
		const SerializedField& _ranged_index_sf = serialized_fields["_ranged_index"];
		if (_ranged_index_sf.type_name == "unsigned")
		{
			_ranged_index = std::any_cast<unsigned>(_ranged_index_sf.copy);
		}
	}
}

void Hachiko::Scripting::PlayerAnimationManager::SerializeTo(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::SerializeTo(serialized_fields);

	serialized_fields["_animator"] = SerializedField(std::string("_animator"), std::make_any<ComponentAnimation*>(_animator), std::string("ComponentAnimation*"));

	serialized_fields["_previous_state"] = SerializedField(std::string("_previous_state"), std::make_any<PlayerState>(_previous_state), std::string("PlayerState"));

	serialized_fields["_state_string"] = SerializedField(std::string("_state_string"), std::make_any<std::string>(_state_string), std::string("std::string"));

	serialized_fields["_idle_index"] = SerializedField(std::string("_idle_index"), std::make_any<unsigned>(_idle_index), std::string("unsigned"));

	serialized_fields["_walking_index"] = SerializedField(std::string("_walking_index"), std::make_any<unsigned>(_walking_index), std::string("unsigned"));

	serialized_fields["_dashing_index"] = SerializedField(std::string("_dashing_index"), std::make_any<unsigned>(_dashing_index), std::string("unsigned"));

	serialized_fields["_melee_index"] = SerializedField(std::string("_melee_index"), std::make_any<unsigned>(_melee_index), std::string("unsigned"));

	serialized_fields["_ranged_index"] = SerializedField(std::string("_ranged_index"), std::make_any<unsigned>(_ranged_index), std::string("unsigned"));
}

void Hachiko::Scripting::PlayerCamera::DeserializeFrom(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::DeserializeFrom(serialized_fields);

	if(serialized_fields.find("_relative_position_to_player") != serialized_fields.end())
	{
		const SerializedField& _relative_position_to_player_sf = serialized_fields["_relative_position_to_player"];
		if (_relative_position_to_player_sf.type_name == "math::float3")
		{
			_relative_position_to_player = std::any_cast<math::float3>(_relative_position_to_player_sf.copy);
		}
	}

	if(serialized_fields.find("_player") != serialized_fields.end())
	{
		const SerializedField& _player_sf = serialized_fields["_player"];
		if (_player_sf.type_name == "GameObject*")
		{
			_player = std::any_cast<GameObject*>(_player_sf.copy);
		}
	}

	if(serialized_fields.find("_follow_delay") != serialized_fields.end())
	{
		const SerializedField& _follow_delay_sf = serialized_fields["_follow_delay"];
		if (_follow_delay_sf.type_name == "float")
		{
			_follow_delay = std::any_cast<float>(_follow_delay_sf.copy);
		}
	}
}

void Hachiko::Scripting::PlayerCamera::SerializeTo(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::SerializeTo(serialized_fields);

	serialized_fields["_relative_position_to_player"] = SerializedField(std::string("_relative_position_to_player"), std::make_any<math::float3>(_relative_position_to_player), std::string("math::float3"));

	serialized_fields["_player"] = SerializedField(std::string("_player"), std::make_any<GameObject*>(_player), std::string("GameObject*"));

	serialized_fields["_follow_delay"] = SerializedField(std::string("_follow_delay"), std::make_any<float>(_follow_delay), std::string("float"));
}

void Hachiko::Scripting::PlayerController::DeserializeFrom(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::DeserializeFrom(serialized_fields);

	if(serialized_fields.find("_attack_indicator") != serialized_fields.end())
	{
		const SerializedField& _attack_indicator_sf = serialized_fields["_attack_indicator"];
		if (_attack_indicator_sf.type_name == "GameObject*")
		{
			_attack_indicator = std::any_cast<GameObject*>(_attack_indicator_sf.copy);
		}
	}

	if(serialized_fields.find("_bullet_emitter") != serialized_fields.end())
	{
		const SerializedField& _bullet_emitter_sf = serialized_fields["_bullet_emitter"];
		if (_bullet_emitter_sf.type_name == "GameObject*")
		{
			_bullet_emitter = std::any_cast<GameObject*>(_bullet_emitter_sf.copy);
		}
	}

	if(serialized_fields.find("_goal") != serialized_fields.end())
	{
		const SerializedField& _goal_sf = serialized_fields["_goal"];
		if (_goal_sf.type_name == "GameObject*")
		{
			_goal = std::any_cast<GameObject*>(_goal_sf.copy);
		}
	}

	if(serialized_fields.find("_dash_duration") != serialized_fields.end())
	{
		const SerializedField& _dash_duration_sf = serialized_fields["_dash_duration"];
		if (_dash_duration_sf.type_name == "float")
		{
			_dash_duration = std::any_cast<float>(_dash_duration_sf.copy);
		}
	}

	if(serialized_fields.find("_dash_distance") != serialized_fields.end())
	{
		const SerializedField& _dash_distance_sf = serialized_fields["_dash_distance"];
		if (_dash_distance_sf.type_name == "float")
		{
			_dash_distance = std::any_cast<float>(_dash_distance_sf.copy);
		}
	}

	if(serialized_fields.find("_dash_cooldown") != serialized_fields.end())
	{
		const SerializedField& _dash_cooldown_sf = serialized_fields["_dash_cooldown"];
		if (_dash_cooldown_sf.type_name == "float")
		{
			_dash_cooldown = std::any_cast<float>(_dash_cooldown_sf.copy);
		}
	}

	if(serialized_fields.find("_max_dash_charges") != serialized_fields.end())
	{
		const SerializedField& _max_dash_charges_sf = serialized_fields["_max_dash_charges"];
		if (_max_dash_charges_sf.type_name == "int")
		{
			_max_dash_charges = std::any_cast<int>(_max_dash_charges_sf.copy);
		}
	}

	if(serialized_fields.find("_attack_duration") != serialized_fields.end())
	{
		const SerializedField& _attack_duration_sf = serialized_fields["_attack_duration"];
		if (_attack_duration_sf.type_name == "float")
		{
			_attack_duration = std::any_cast<float>(_attack_duration_sf.copy);
		}
	}

	if(serialized_fields.find("_rotation_duration") != serialized_fields.end())
	{
		const SerializedField& _rotation_duration_sf = serialized_fields["_rotation_duration"];
		if (_rotation_duration_sf.type_name == "float")
		{
			_rotation_duration = std::any_cast<float>(_rotation_duration_sf.copy);
		}
	}

	if(serialized_fields.find("_hp_cell_1") != serialized_fields.end())
	{
		const SerializedField& _hp_cell_1_sf = serialized_fields["_hp_cell_1"];
		if (_hp_cell_1_sf.type_name == "GameObject*")
		{
			_hp_cell_1 = std::any_cast<GameObject*>(_hp_cell_1_sf.copy);
		}
	}

	if(serialized_fields.find("_hp_cell_2") != serialized_fields.end())
	{
		const SerializedField& _hp_cell_2_sf = serialized_fields["_hp_cell_2"];
		if (_hp_cell_2_sf.type_name == "GameObject*")
		{
			_hp_cell_2 = std::any_cast<GameObject*>(_hp_cell_2_sf.copy);
		}
	}

	if(serialized_fields.find("_hp_cell_3") != serialized_fields.end())
	{
		const SerializedField& _hp_cell_3_sf = serialized_fields["_hp_cell_3"];
		if (_hp_cell_3_sf.type_name == "GameObject*")
		{
			_hp_cell_3 = std::any_cast<GameObject*>(_hp_cell_3_sf.copy);
		}
	}

	if(serialized_fields.find("_hp_cell_4") != serialized_fields.end())
	{
		const SerializedField& _hp_cell_4_sf = serialized_fields["_hp_cell_4"];
		if (_hp_cell_4_sf.type_name == "GameObject*")
		{
			_hp_cell_4 = std::any_cast<GameObject*>(_hp_cell_4_sf.copy);
		}
	}

	if(serialized_fields.find("_camera") != serialized_fields.end())
	{
		const SerializedField& _camera_sf = serialized_fields["_camera"];
		if (_camera_sf.type_name == "GameObject*")
		{
			_camera = std::any_cast<GameObject*>(_camera_sf.copy);
		}
	}

	if(serialized_fields.find("_ui_damage") != serialized_fields.end())
	{
		const SerializedField& _ui_damage_sf = serialized_fields["_ui_damage"];
		if (_ui_damage_sf.type_name == "GameObject*")
		{
			_ui_damage = std::any_cast<GameObject*>(_ui_damage_sf.copy);
		}
	}

	if(serialized_fields.find("_state") != serialized_fields.end())
	{
		const SerializedField& _state_sf = serialized_fields["_state"];
		if (_state_sf.type_name == "PlayerState")
		{
			_state = std::any_cast<PlayerState>(_state_sf.copy);
		}
	}
}

void Hachiko::Scripting::PlayerController::SerializeTo(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::SerializeTo(serialized_fields);

	serialized_fields["_attack_indicator"] = SerializedField(std::string("_attack_indicator"), std::make_any<GameObject*>(_attack_indicator), std::string("GameObject*"));

	serialized_fields["_bullet_emitter"] = SerializedField(std::string("_bullet_emitter"), std::make_any<GameObject*>(_bullet_emitter), std::string("GameObject*"));

	serialized_fields["_goal"] = SerializedField(std::string("_goal"), std::make_any<GameObject*>(_goal), std::string("GameObject*"));

	serialized_fields["_dash_duration"] = SerializedField(std::string("_dash_duration"), std::make_any<float>(_dash_duration), std::string("float"));

	serialized_fields["_dash_distance"] = SerializedField(std::string("_dash_distance"), std::make_any<float>(_dash_distance), std::string("float"));

	serialized_fields["_dash_cooldown"] = SerializedField(std::string("_dash_cooldown"), std::make_any<float>(_dash_cooldown), std::string("float"));

	serialized_fields["_max_dash_charges"] = SerializedField(std::string("_max_dash_charges"), std::make_any<int>(_max_dash_charges), std::string("int"));

	serialized_fields["_attack_duration"] = SerializedField(std::string("_attack_duration"), std::make_any<float>(_attack_duration), std::string("float"));

	serialized_fields["_rotation_duration"] = SerializedField(std::string("_rotation_duration"), std::make_any<float>(_rotation_duration), std::string("float"));

	serialized_fields["_hp_cell_1"] = SerializedField(std::string("_hp_cell_1"), std::make_any<GameObject*>(_hp_cell_1), std::string("GameObject*"));

	serialized_fields["_hp_cell_2"] = SerializedField(std::string("_hp_cell_2"), std::make_any<GameObject*>(_hp_cell_2), std::string("GameObject*"));

	serialized_fields["_hp_cell_3"] = SerializedField(std::string("_hp_cell_3"), std::make_any<GameObject*>(_hp_cell_3), std::string("GameObject*"));

	serialized_fields["_hp_cell_4"] = SerializedField(std::string("_hp_cell_4"), std::make_any<GameObject*>(_hp_cell_4), std::string("GameObject*"));

	serialized_fields["_camera"] = SerializedField(std::string("_camera"), std::make_any<GameObject*>(_camera), std::string("GameObject*"));

	serialized_fields["_ui_damage"] = SerializedField(std::string("_ui_damage"), std::make_any<GameObject*>(_ui_damage), std::string("GameObject*"));

	serialized_fields["_state"] = SerializedField(std::string("_state"), std::make_any<PlayerState>(_state), std::string("PlayerState"));
}

void Hachiko::Scripting::PlayerSoundManager::DeserializeFrom(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::DeserializeFrom(serialized_fields);

	if(serialized_fields.find("_audio_source") != serialized_fields.end())
	{
		const SerializedField& _audio_source_sf = serialized_fields["_audio_source"];
		if (_audio_source_sf.type_name == "ComponentAudioSource*")
		{
			_audio_source = std::any_cast<ComponentAudioSource*>(_audio_source_sf.copy);
		}
	}

	if(serialized_fields.find("_step_frequency") != serialized_fields.end())
	{
		const SerializedField& _step_frequency_sf = serialized_fields["_step_frequency"];
		if (_step_frequency_sf.type_name == "float")
		{
			_step_frequency = std::any_cast<float>(_step_frequency_sf.copy);
		}
	}

	if(serialized_fields.find("_melee_frequency") != serialized_fields.end())
	{
		const SerializedField& _melee_frequency_sf = serialized_fields["_melee_frequency"];
		if (_melee_frequency_sf.type_name == "float")
		{
			_melee_frequency = std::any_cast<float>(_melee_frequency_sf.copy);
		}
	}

	if(serialized_fields.find("_ranged_frequency") != serialized_fields.end())
	{
		const SerializedField& _ranged_frequency_sf = serialized_fields["_ranged_frequency"];
		if (_ranged_frequency_sf.type_name == "float")
		{
			_ranged_frequency = std::any_cast<float>(_ranged_frequency_sf.copy);
		}
	}

	if(serialized_fields.find("_timer") != serialized_fields.end())
	{
		const SerializedField& _timer_sf = serialized_fields["_timer"];
		if (_timer_sf.type_name == "float")
		{
			_timer = std::any_cast<float>(_timer_sf.copy);
		}
	}

	if(serialized_fields.find("_previous_state") != serialized_fields.end())
	{
		const SerializedField& _previous_state_sf = serialized_fields["_previous_state"];
		if (_previous_state_sf.type_name == "PlayerState")
		{
			_previous_state = std::any_cast<PlayerState>(_previous_state_sf.copy);
		}
	}
}

void Hachiko::Scripting::PlayerSoundManager::SerializeTo(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::SerializeTo(serialized_fields);

	serialized_fields["_audio_source"] = SerializedField(std::string("_audio_source"), std::make_any<ComponentAudioSource*>(_audio_source), std::string("ComponentAudioSource*"));

	serialized_fields["_step_frequency"] = SerializedField(std::string("_step_frequency"), std::make_any<float>(_step_frequency), std::string("float"));

	serialized_fields["_melee_frequency"] = SerializedField(std::string("_melee_frequency"), std::make_any<float>(_melee_frequency), std::string("float"));

	serialized_fields["_ranged_frequency"] = SerializedField(std::string("_ranged_frequency"), std::make_any<float>(_ranged_frequency), std::string("float"));

	serialized_fields["_timer"] = SerializedField(std::string("_timer"), std::make_any<float>(_timer), std::string("float"));

	serialized_fields["_previous_state"] = SerializedField(std::string("_previous_state"), std::make_any<PlayerState>(_previous_state), std::string("PlayerState"));
}

void Hachiko::Scripting::RoomTeleporter::DeserializeFrom(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::DeserializeFrom(serialized_fields);

	if(serialized_fields.find("_player") != serialized_fields.end())
	{
		const SerializedField& _player_sf = serialized_fields["_player"];
		if (_player_sf.type_name == "GameObject*")
		{
			_player = std::any_cast<GameObject*>(_player_sf.copy);
		}
	}

	if(serialized_fields.find("_room_portal") != serialized_fields.end())
	{
		const SerializedField& _room_portal_sf = serialized_fields["_room_portal"];
		if (_room_portal_sf.type_name == "GameObject*")
		{
			_room_portal = std::any_cast<GameObject*>(_room_portal_sf.copy);
		}
	}

	if(serialized_fields.find("_outdoor_portal") != serialized_fields.end())
	{
		const SerializedField& _outdoor_portal_sf = serialized_fields["_outdoor_portal"];
		if (_outdoor_portal_sf.type_name == "GameObject*")
		{
			_outdoor_portal = std::any_cast<GameObject*>(_outdoor_portal_sf.copy);
		}
	}

	if(serialized_fields.find("_trigger_distance") != serialized_fields.end())
	{
		const SerializedField& _trigger_distance_sf = serialized_fields["_trigger_distance"];
		if (_trigger_distance_sf.type_name == "float")
		{
			_trigger_distance = std::any_cast<float>(_trigger_distance_sf.copy);
		}
	}

	if(serialized_fields.find("_fade_image") != serialized_fields.end())
	{
		const SerializedField& _fade_image_sf = serialized_fields["_fade_image"];
		if (_fade_image_sf.type_name == "ComponentImage*")
		{
			_fade_image = std::any_cast<ComponentImage*>(_fade_image_sf.copy);
		}
	}

	if(serialized_fields.find("_fade_duration") != serialized_fields.end())
	{
		const SerializedField& _fade_duration_sf = serialized_fields["_fade_duration"];
		if (_fade_duration_sf.type_name == "float")
		{
			_fade_duration = std::any_cast<float>(_fade_duration_sf.copy);
		}
	}

	if(serialized_fields.find("_blackout_duration") != serialized_fields.end())
	{
		const SerializedField& _blackout_duration_sf = serialized_fields["_blackout_duration"];
		if (_blackout_duration_sf.type_name == "float")
		{
			_blackout_duration = std::any_cast<float>(_blackout_duration_sf.copy);
		}
	}
}

void Hachiko::Scripting::RoomTeleporter::SerializeTo(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::SerializeTo(serialized_fields);

	serialized_fields["_player"] = SerializedField(std::string("_player"), std::make_any<GameObject*>(_player), std::string("GameObject*"));

	serialized_fields["_room_portal"] = SerializedField(std::string("_room_portal"), std::make_any<GameObject*>(_room_portal), std::string("GameObject*"));

	serialized_fields["_outdoor_portal"] = SerializedField(std::string("_outdoor_portal"), std::make_any<GameObject*>(_outdoor_portal), std::string("GameObject*"));

	serialized_fields["_trigger_distance"] = SerializedField(std::string("_trigger_distance"), std::make_any<float>(_trigger_distance), std::string("float"));

	serialized_fields["_fade_image"] = SerializedField(std::string("_fade_image"), std::make_any<ComponentImage*>(_fade_image), std::string("ComponentImage*"));

	serialized_fields["_fade_duration"] = SerializedField(std::string("_fade_duration"), std::make_any<float>(_fade_duration), std::string("float"));

	serialized_fields["_blackout_duration"] = SerializedField(std::string("_blackout_duration"), std::make_any<float>(_blackout_duration), std::string("float"));
}

void Hachiko::Scripting::DynamicCamera::DeserializeFrom(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::DeserializeFrom(serialized_fields);

	if(serialized_fields.find("_start_point") != serialized_fields.end())
	{
		const SerializedField& _start_point_sf = serialized_fields["_start_point"];
		if (_start_point_sf.type_name == "math::float3")
		{
			_start_point = std::any_cast<math::float3>(_start_point_sf.copy);
		}
	}

	if(serialized_fields.find("_end_point") != serialized_fields.end())
	{
		const SerializedField& _end_point_sf = serialized_fields["_end_point"];
		if (_end_point_sf.type_name == "math::float3")
		{
			_end_point = std::any_cast<math::float3>(_end_point_sf.copy);
		}
	}

	if(serialized_fields.find("_speed") != serialized_fields.end())
	{
		const SerializedField& _speed_sf = serialized_fields["_speed"];
		if (_speed_sf.type_name == "float")
		{
			_speed = std::any_cast<float>(_speed_sf.copy);
		}
	}

	if(serialized_fields.find("_lerp_position") != serialized_fields.end())
	{
		const SerializedField& _lerp_position_sf = serialized_fields["_lerp_position"];
		if (_lerp_position_sf.type_name == "float")
		{
			_lerp_position = std::any_cast<float>(_lerp_position_sf.copy);
		}
	}
}

void Hachiko::Scripting::DynamicCamera::SerializeTo(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::SerializeTo(serialized_fields);

	serialized_fields["_start_point"] = SerializedField(std::string("_start_point"), std::make_any<math::float3>(_start_point), std::string("math::float3"));

	serialized_fields["_end_point"] = SerializedField(std::string("_end_point"), std::make_any<math::float3>(_end_point), std::string("math::float3"));

	serialized_fields["_speed"] = SerializedField(std::string("_speed"), std::make_any<float>(_speed), std::string("float"));

	serialized_fields["_lerp_position"] = SerializedField(std::string("_lerp_position"), std::make_any<float>(_lerp_position), std::string("float"));
}

void Hachiko::Scripting::FancyLights::DeserializeFrom(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::DeserializeFrom(serialized_fields);

	if(serialized_fields.find("_rotate_on_y") != serialized_fields.end())
	{
		const SerializedField& _rotate_on_y_sf = serialized_fields["_rotate_on_y"];
		if (_rotate_on_y_sf.type_name == "bool")
		{
			_rotate_on_y = std::any_cast<bool>(_rotate_on_y_sf.copy);
		}
	}

	if(serialized_fields.find("_angle") != serialized_fields.end())
	{
		const SerializedField& _angle_sf = serialized_fields["_angle"];
		if (_angle_sf.type_name == "float")
		{
			_angle = std::any_cast<float>(_angle_sf.copy);
		}
	}
}

void Hachiko::Scripting::FancyLights::SerializeTo(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::SerializeTo(serialized_fields);

	serialized_fields["_rotate_on_y"] = SerializedField(std::string("_rotate_on_y"), std::make_any<bool>(_rotate_on_y), std::string("bool"));

	serialized_fields["_angle"] = SerializedField(std::string("_angle"), std::make_any<float>(_angle), std::string("float"));
}

void Hachiko::Scripting::BackToMainMenu::DeserializeFrom(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::DeserializeFrom(serialized_fields);

	if(serialized_fields.find("_button_back") != serialized_fields.end())
	{
		const SerializedField& _button_back_sf = serialized_fields["_button_back"];
		if (_button_back_sf.type_name == "ComponentButton*")
		{
			_button_back = std::any_cast<ComponentButton*>(_button_back_sf.copy);
		}
	}
}

void Hachiko::Scripting::BackToMainMenu::SerializeTo(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::SerializeTo(serialized_fields);

	serialized_fields["_button_back"] = SerializedField(std::string("_button_back"), std::make_any<ComponentButton*>(_button_back), std::string("ComponentButton*"));
}

void Hachiko::Scripting::DebugManager::DeserializeFrom(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::DeserializeFrom(serialized_fields);

	if(serialized_fields.find("_player") != serialized_fields.end())
	{
		const SerializedField& _player_sf = serialized_fields["_player"];
		if (_player_sf.type_name == "GameObject*")
		{
			_player = std::any_cast<GameObject*>(_player_sf.copy);
		}
	}

	if(serialized_fields.find("_button_back") != serialized_fields.end())
	{
		const SerializedField& _button_back_sf = serialized_fields["_button_back"];
		if (_button_back_sf.type_name == "ComponentButton*")
		{
			_button_back = std::any_cast<ComponentButton*>(_button_back_sf.copy);
		}
	}

	if(serialized_fields.find("_teleport_next_pos") != serialized_fields.end())
	{
		const SerializedField& _teleport_next_pos_sf = serialized_fields["_teleport_next_pos"];
		if (_teleport_next_pos_sf.type_name == "ComponentButton*")
		{
			_teleport_next_pos = std::any_cast<ComponentButton*>(_teleport_next_pos_sf.copy);
		}
	}

	if(serialized_fields.find("_teleport_prev_pos") != serialized_fields.end())
	{
		const SerializedField& _teleport_prev_pos_sf = serialized_fields["_teleport_prev_pos"];
		if (_teleport_prev_pos_sf.type_name == "ComponentButton*")
		{
			_teleport_prev_pos = std::any_cast<ComponentButton*>(_teleport_prev_pos_sf.copy);
		}
	}

	if(serialized_fields.find("_teleport_add_pos") != serialized_fields.end())
	{
		const SerializedField& _teleport_add_pos_sf = serialized_fields["_teleport_add_pos"];
		if (_teleport_add_pos_sf.type_name == "ComponentButton*")
		{
			_teleport_add_pos = std::any_cast<ComponentButton*>(_teleport_add_pos_sf.copy);
		}
	}

	if(serialized_fields.find("_add_health") != serialized_fields.end())
	{
		const SerializedField& _add_health_sf = serialized_fields["_add_health"];
		if (_add_health_sf.type_name == "ComponentButton*")
		{
			_add_health = std::any_cast<ComponentButton*>(_add_health_sf.copy);
		}
	}

	if(serialized_fields.find("_remove_health") != serialized_fields.end())
	{
		const SerializedField& _remove_health_sf = serialized_fields["_remove_health"];
		if (_remove_health_sf.type_name == "ComponentButton*")
		{
			_remove_health = std::any_cast<ComponentButton*>(_remove_health_sf.copy);
		}
	}

	if(serialized_fields.find("_increase_max_hp") != serialized_fields.end())
	{
		const SerializedField& _increase_max_hp_sf = serialized_fields["_increase_max_hp"];
		if (_increase_max_hp_sf.type_name == "ComponentButton*")
		{
			_increase_max_hp = std::any_cast<ComponentButton*>(_increase_max_hp_sf.copy);
		}
	}

	if(serialized_fields.find("_decrease_max_hp") != serialized_fields.end())
	{
		const SerializedField& _decrease_max_hp_sf = serialized_fields["_decrease_max_hp"];
		if (_decrease_max_hp_sf.type_name == "ComponentButton*")
		{
			_decrease_max_hp = std::any_cast<ComponentButton*>(_decrease_max_hp_sf.copy);
		}
	}

	if(serialized_fields.find("_increase_move_speed") != serialized_fields.end())
	{
		const SerializedField& _increase_move_speed_sf = serialized_fields["_increase_move_speed"];
		if (_increase_move_speed_sf.type_name == "ComponentButton*")
		{
			_increase_move_speed = std::any_cast<ComponentButton*>(_increase_move_speed_sf.copy);
		}
	}

	if(serialized_fields.find("_decrease_move_speed") != serialized_fields.end())
	{
		const SerializedField& _decrease_move_speed_sf = serialized_fields["_decrease_move_speed"];
		if (_decrease_move_speed_sf.type_name == "ComponentButton*")
		{
			_decrease_move_speed = std::any_cast<ComponentButton*>(_decrease_move_speed_sf.copy);
		}
	}

	if(serialized_fields.find("_increase_attack_cd") != serialized_fields.end())
	{
		const SerializedField& _increase_attack_cd_sf = serialized_fields["_increase_attack_cd"];
		if (_increase_attack_cd_sf.type_name == "ComponentButton*")
		{
			_increase_attack_cd = std::any_cast<ComponentButton*>(_increase_attack_cd_sf.copy);
		}
	}

	if(serialized_fields.find("_decrease_attack_cd") != serialized_fields.end())
	{
		const SerializedField& _decrease_attack_cd_sf = serialized_fields["_decrease_attack_cd"];
		if (_decrease_attack_cd_sf.type_name == "ComponentButton*")
		{
			_decrease_attack_cd = std::any_cast<ComponentButton*>(_decrease_attack_cd_sf.copy);
		}
	}

	if(serialized_fields.find("_increase_attack_power") != serialized_fields.end())
	{
		const SerializedField& _increase_attack_power_sf = serialized_fields["_increase_attack_power"];
		if (_increase_attack_power_sf.type_name == "ComponentButton*")
		{
			_increase_attack_power = std::any_cast<ComponentButton*>(_increase_attack_power_sf.copy);
		}
	}

	if(serialized_fields.find("_decrease_attack_power") != serialized_fields.end())
	{
		const SerializedField& _decrease_attack_power_sf = serialized_fields["_decrease_attack_power"];
		if (_decrease_attack_power_sf.type_name == "ComponentButton*")
		{
			_decrease_attack_power = std::any_cast<ComponentButton*>(_decrease_attack_power_sf.copy);
		}
	}

	if(serialized_fields.find("_god_mode") != serialized_fields.end())
	{
		const SerializedField& _god_mode_sf = serialized_fields["_god_mode"];
		if (_god_mode_sf.type_name == "ComponentButton*")
		{
			_god_mode = std::any_cast<ComponentButton*>(_god_mode_sf.copy);
		}
	}

	if(serialized_fields.find("_spawn_enemy") != serialized_fields.end())
	{
		const SerializedField& _spawn_enemy_sf = serialized_fields["_spawn_enemy"];
		if (_spawn_enemy_sf.type_name == "ComponentButton*")
		{
			_spawn_enemy = std::any_cast<ComponentButton*>(_spawn_enemy_sf.copy);
		}
	}

	if(serialized_fields.find("_unlock_skills") != serialized_fields.end())
	{
		const SerializedField& _unlock_skills_sf = serialized_fields["_unlock_skills"];
		if (_unlock_skills_sf.type_name == "ComponentButton*")
		{
			_unlock_skills = std::any_cast<ComponentButton*>(_unlock_skills_sf.copy);
		}
	}

	if(serialized_fields.find("_toggle_performance_output") != serialized_fields.end())
	{
		const SerializedField& _toggle_performance_output_sf = serialized_fields["_toggle_performance_output"];
		if (_toggle_performance_output_sf.type_name == "ComponentButton*")
		{
			_toggle_performance_output = std::any_cast<ComponentButton*>(_toggle_performance_output_sf.copy);
		}
	}

	if(serialized_fields.find("_toggle_vsync") != serialized_fields.end())
	{
		const SerializedField& _toggle_vsync_sf = serialized_fields["_toggle_vsync"];
		if (_toggle_vsync_sf.type_name == "ComponentButton*")
		{
			_toggle_vsync = std::any_cast<ComponentButton*>(_toggle_vsync_sf.copy);
		}
	}

	if(serialized_fields.find("_toggle_wireframe") != serialized_fields.end())
	{
		const SerializedField& _toggle_wireframe_sf = serialized_fields["_toggle_wireframe"];
		if (_toggle_wireframe_sf.type_name == "ComponentButton*")
		{
			_toggle_wireframe = std::any_cast<ComponentButton*>(_toggle_wireframe_sf.copy);
		}
	}

	if(serialized_fields.find("_toggle_show_colliders") != serialized_fields.end())
	{
		const SerializedField& _toggle_show_colliders_sf = serialized_fields["_toggle_show_colliders"];
		if (_toggle_show_colliders_sf.type_name == "ComponentButton*")
		{
			_toggle_show_colliders = std::any_cast<ComponentButton*>(_toggle_show_colliders_sf.copy);
		}
	}

	if(serialized_fields.find("_exit_debug") != serialized_fields.end())
	{
		const SerializedField& _exit_debug_sf = serialized_fields["_exit_debug"];
		if (_exit_debug_sf.type_name == "ComponentButton*")
		{
			_exit_debug = std::any_cast<ComponentButton*>(_exit_debug_sf.copy);
		}
	}

	if(serialized_fields.find("_text_fps") != serialized_fields.end())
	{
		const SerializedField& _text_fps_sf = serialized_fields["_text_fps"];
		if (_text_fps_sf.type_name == "ComponentText*")
		{
			_text_fps = std::any_cast<ComponentText*>(_text_fps_sf.copy);
		}
	}

	if(serialized_fields.find("_text_ms") != serialized_fields.end())
	{
		const SerializedField& _text_ms_sf = serialized_fields["_text_ms"];
		if (_text_ms_sf.type_name == "ComponentText*")
		{
			_text_ms = std::any_cast<ComponentText*>(_text_ms_sf.copy);
		}
	}

	if(serialized_fields.find("_tp_pos1") != serialized_fields.end())
	{
		const SerializedField& _tp_pos1_sf = serialized_fields["_tp_pos1"];
		if (_tp_pos1_sf.type_name == "GameObject*")
		{
			_tp_pos1 = std::any_cast<GameObject*>(_tp_pos1_sf.copy);
		}
	}

	if(serialized_fields.find("_tp_pos2") != serialized_fields.end())
	{
		const SerializedField& _tp_pos2_sf = serialized_fields["_tp_pos2"];
		if (_tp_pos2_sf.type_name == "GameObject*")
		{
			_tp_pos2 = std::any_cast<GameObject*>(_tp_pos2_sf.copy);
		}
	}

	if(serialized_fields.find("_tp_pos3") != serialized_fields.end())
	{
		const SerializedField& _tp_pos3_sf = serialized_fields["_tp_pos3"];
		if (_tp_pos3_sf.type_name == "GameObject*")
		{
			_tp_pos3 = std::any_cast<GameObject*>(_tp_pos3_sf.copy);
		}
	}

	if(serialized_fields.find("_performance_menu") != serialized_fields.end())
	{
		const SerializedField& _performance_menu_sf = serialized_fields["_performance_menu"];
		if (_performance_menu_sf.type_name == "GameObject*")
		{
			_performance_menu = std::any_cast<GameObject*>(_performance_menu_sf.copy);
		}
	}
}

void Hachiko::Scripting::DebugManager::SerializeTo(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::SerializeTo(serialized_fields);

	serialized_fields["_player"] = SerializedField(std::string("_player"), std::make_any<GameObject*>(_player), std::string("GameObject*"));

	serialized_fields["_button_back"] = SerializedField(std::string("_button_back"), std::make_any<ComponentButton*>(_button_back), std::string("ComponentButton*"));

	serialized_fields["_teleport_next_pos"] = SerializedField(std::string("_teleport_next_pos"), std::make_any<ComponentButton*>(_teleport_next_pos), std::string("ComponentButton*"));

	serialized_fields["_teleport_prev_pos"] = SerializedField(std::string("_teleport_prev_pos"), std::make_any<ComponentButton*>(_teleport_prev_pos), std::string("ComponentButton*"));

	serialized_fields["_teleport_add_pos"] = SerializedField(std::string("_teleport_add_pos"), std::make_any<ComponentButton*>(_teleport_add_pos), std::string("ComponentButton*"));

	serialized_fields["_add_health"] = SerializedField(std::string("_add_health"), std::make_any<ComponentButton*>(_add_health), std::string("ComponentButton*"));

	serialized_fields["_remove_health"] = SerializedField(std::string("_remove_health"), std::make_any<ComponentButton*>(_remove_health), std::string("ComponentButton*"));

	serialized_fields["_increase_max_hp"] = SerializedField(std::string("_increase_max_hp"), std::make_any<ComponentButton*>(_increase_max_hp), std::string("ComponentButton*"));

	serialized_fields["_decrease_max_hp"] = SerializedField(std::string("_decrease_max_hp"), std::make_any<ComponentButton*>(_decrease_max_hp), std::string("ComponentButton*"));

	serialized_fields["_increase_move_speed"] = SerializedField(std::string("_increase_move_speed"), std::make_any<ComponentButton*>(_increase_move_speed), std::string("ComponentButton*"));

	serialized_fields["_decrease_move_speed"] = SerializedField(std::string("_decrease_move_speed"), std::make_any<ComponentButton*>(_decrease_move_speed), std::string("ComponentButton*"));

	serialized_fields["_increase_attack_cd"] = SerializedField(std::string("_increase_attack_cd"), std::make_any<ComponentButton*>(_increase_attack_cd), std::string("ComponentButton*"));

	serialized_fields["_decrease_attack_cd"] = SerializedField(std::string("_decrease_attack_cd"), std::make_any<ComponentButton*>(_decrease_attack_cd), std::string("ComponentButton*"));

	serialized_fields["_increase_attack_power"] = SerializedField(std::string("_increase_attack_power"), std::make_any<ComponentButton*>(_increase_attack_power), std::string("ComponentButton*"));

	serialized_fields["_decrease_attack_power"] = SerializedField(std::string("_decrease_attack_power"), std::make_any<ComponentButton*>(_decrease_attack_power), std::string("ComponentButton*"));

	serialized_fields["_god_mode"] = SerializedField(std::string("_god_mode"), std::make_any<ComponentButton*>(_god_mode), std::string("ComponentButton*"));

	serialized_fields["_spawn_enemy"] = SerializedField(std::string("_spawn_enemy"), std::make_any<ComponentButton*>(_spawn_enemy), std::string("ComponentButton*"));

	serialized_fields["_unlock_skills"] = SerializedField(std::string("_unlock_skills"), std::make_any<ComponentButton*>(_unlock_skills), std::string("ComponentButton*"));

	serialized_fields["_toggle_performance_output"] = SerializedField(std::string("_toggle_performance_output"), std::make_any<ComponentButton*>(_toggle_performance_output), std::string("ComponentButton*"));

	serialized_fields["_toggle_vsync"] = SerializedField(std::string("_toggle_vsync"), std::make_any<ComponentButton*>(_toggle_vsync), std::string("ComponentButton*"));

	serialized_fields["_toggle_wireframe"] = SerializedField(std::string("_toggle_wireframe"), std::make_any<ComponentButton*>(_toggle_wireframe), std::string("ComponentButton*"));

	serialized_fields["_toggle_show_colliders"] = SerializedField(std::string("_toggle_show_colliders"), std::make_any<ComponentButton*>(_toggle_show_colliders), std::string("ComponentButton*"));

	serialized_fields["_exit_debug"] = SerializedField(std::string("_exit_debug"), std::make_any<ComponentButton*>(_exit_debug), std::string("ComponentButton*"));

	serialized_fields["_text_fps"] = SerializedField(std::string("_text_fps"), std::make_any<ComponentText*>(_text_fps), std::string("ComponentText*"));

	serialized_fields["_text_ms"] = SerializedField(std::string("_text_ms"), std::make_any<ComponentText*>(_text_ms), std::string("ComponentText*"));

	serialized_fields["_tp_pos1"] = SerializedField(std::string("_tp_pos1"), std::make_any<GameObject*>(_tp_pos1), std::string("GameObject*"));

	serialized_fields["_tp_pos2"] = SerializedField(std::string("_tp_pos2"), std::make_any<GameObject*>(_tp_pos2), std::string("GameObject*"));

	serialized_fields["_tp_pos3"] = SerializedField(std::string("_tp_pos3"), std::make_any<GameObject*>(_tp_pos3), std::string("GameObject*"));

	serialized_fields["_performance_menu"] = SerializedField(std::string("_performance_menu"), std::make_any<GameObject*>(_performance_menu), std::string("GameObject*"));
}

void Hachiko::Scripting::MainMenuManager::DeserializeFrom(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::DeserializeFrom(serialized_fields);

	if(serialized_fields.find("_state") != serialized_fields.end())
	{
		const SerializedField& _state_sf = serialized_fields["_state"];
		if (_state_sf.type_name == "State")
		{
			_state = std::any_cast<State>(_state_sf.copy);
		}
	}

	if(serialized_fields.find("_state_changed") != serialized_fields.end())
	{
		const SerializedField& _state_changed_sf = serialized_fields["_state_changed"];
		if (_state_changed_sf.type_name == "bool")
		{
			_state_changed = std::any_cast<bool>(_state_changed_sf.copy);
		}
	}

	if(serialized_fields.find("_main_background") != serialized_fields.end())
	{
		const SerializedField& _main_background_sf = serialized_fields["_main_background"];
		if (_main_background_sf.type_name == "GameObject*")
		{
			_main_background = std::any_cast<GameObject*>(_main_background_sf.copy);
		}
	}

	if(serialized_fields.find("_button_background") != serialized_fields.end())
	{
		const SerializedField& _button_background_sf = serialized_fields["_button_background"];
		if (_button_background_sf.type_name == "GameObject*")
		{
			_button_background = std::any_cast<GameObject*>(_button_background_sf.copy);
		}
	}

	if(serialized_fields.find("_button_play") != serialized_fields.end())
	{
		const SerializedField& _button_play_sf = serialized_fields["_button_play"];
		if (_button_play_sf.type_name == "ComponentButton*")
		{
			_button_play = std::any_cast<ComponentButton*>(_button_play_sf.copy);
		}
	}

	if(serialized_fields.find("_button_quit") != serialized_fields.end())
	{
		const SerializedField& _button_quit_sf = serialized_fields["_button_quit"];
		if (_button_quit_sf.type_name == "ComponentButton*")
		{
			_button_quit = std::any_cast<ComponentButton*>(_button_quit_sf.copy);
		}
	}

	if(serialized_fields.find("_button_settings") != serialized_fields.end())
	{
		const SerializedField& _button_settings_sf = serialized_fields["_button_settings"];
		if (_button_settings_sf.type_name == "ComponentButton*")
		{
			_button_settings = std::any_cast<ComponentButton*>(_button_settings_sf.copy);
		}
	}

	if(serialized_fields.find("_button_credits") != serialized_fields.end())
	{
		const SerializedField& _button_credits_sf = serialized_fields["_button_credits"];
		if (_button_credits_sf.type_name == "ComponentButton*")
		{
			_button_credits = std::any_cast<ComponentButton*>(_button_credits_sf.copy);
		}
	}

	if(serialized_fields.find("_settings") != serialized_fields.end())
	{
		const SerializedField& _settings_sf = serialized_fields["_settings"];
		if (_settings_sf.type_name == "GameObject*")
		{
			_settings = std::any_cast<GameObject*>(_settings_sf.copy);
		}
	}

	if(serialized_fields.find("_credits") != serialized_fields.end())
	{
		const SerializedField& _credits_sf = serialized_fields["_credits"];
		if (_credits_sf.type_name == "GameObject*")
		{
			_credits = std::any_cast<GameObject*>(_credits_sf.copy);
		}
	}

	if(serialized_fields.find("_button_back") != serialized_fields.end())
	{
		const SerializedField& _button_back_sf = serialized_fields["_button_back"];
		if (_button_back_sf.type_name == "ComponentButton*")
		{
			_button_back = std::any_cast<ComponentButton*>(_button_back_sf.copy);
		}
	}
}

void Hachiko::Scripting::MainMenuManager::SerializeTo(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::SerializeTo(serialized_fields);

	serialized_fields["_state"] = SerializedField(std::string("_state"), std::make_any<State>(_state), std::string("State"));

	serialized_fields["_state_changed"] = SerializedField(std::string("_state_changed"), std::make_any<bool>(_state_changed), std::string("bool"));

	serialized_fields["_main_background"] = SerializedField(std::string("_main_background"), std::make_any<GameObject*>(_main_background), std::string("GameObject*"));

	serialized_fields["_button_background"] = SerializedField(std::string("_button_background"), std::make_any<GameObject*>(_button_background), std::string("GameObject*"));

	serialized_fields["_button_play"] = SerializedField(std::string("_button_play"), std::make_any<ComponentButton*>(_button_play), std::string("ComponentButton*"));

	serialized_fields["_button_quit"] = SerializedField(std::string("_button_quit"), std::make_any<ComponentButton*>(_button_quit), std::string("ComponentButton*"));

	serialized_fields["_button_settings"] = SerializedField(std::string("_button_settings"), std::make_any<ComponentButton*>(_button_settings), std::string("ComponentButton*"));

	serialized_fields["_button_credits"] = SerializedField(std::string("_button_credits"), std::make_any<ComponentButton*>(_button_credits), std::string("ComponentButton*"));

	serialized_fields["_settings"] = SerializedField(std::string("_settings"), std::make_any<GameObject*>(_settings), std::string("GameObject*"));

	serialized_fields["_credits"] = SerializedField(std::string("_credits"), std::make_any<GameObject*>(_credits), std::string("GameObject*"));

	serialized_fields["_button_back"] = SerializedField(std::string("_button_back"), std::make_any<ComponentButton*>(_button_back), std::string("ComponentButton*"));
}