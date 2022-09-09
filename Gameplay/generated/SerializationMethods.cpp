#include <vector>
#include <string>
#include <unordered_map>
#include "scriptingUtil/gameplaypch.h"
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


void Hachiko::Scripting::TriggerAnim::DeserializeFrom(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::DeserializeFrom(serialized_fields);
}

void Hachiko::Scripting::TriggerAnim::SerializeTo(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::SerializeTo(serialized_fields);
}

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
		if (_attack_cd_sf.type_name == "float")
		{
			_attack_cd = std::any_cast<float>(_attack_cd_sf.copy);
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

	if(serialized_fields.find("_current_hp") != serialized_fields.end())
	{
		const SerializedField& _current_hp_sf = serialized_fields["_current_hp"];
		if (_current_hp_sf.type_name == "int")
		{
			_current_hp = std::any_cast<int>(_current_hp_sf.copy);
		}
	}
}

void Hachiko::Scripting::Stats::SerializeTo(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::SerializeTo(serialized_fields);

	serialized_fields["_attack_power"] = SerializedField(std::string("_attack_power"), std::make_any<int>(_attack_power), std::string("int"));

	serialized_fields["_attack_cd"] = SerializedField(std::string("_attack_cd"), std::make_any<float>(_attack_cd), std::string("float"));

	serialized_fields["_attack_range"] = SerializedField(std::string("_attack_range"), std::make_any<float>(_attack_range), std::string("float"));

	serialized_fields["_move_speed"] = SerializedField(std::string("_move_speed"), std::make_any<float>(_move_speed), std::string("float"));

	serialized_fields["_max_hp"] = SerializedField(std::string("_max_hp"), std::make_any<int>(_max_hp), std::string("int"));

	serialized_fields["_current_hp"] = SerializedField(std::string("_current_hp"), std::make_any<int>(_current_hp), std::string("int"));
}

void Hachiko::Scripting::CrystalExplosion::DeserializeFrom(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::DeserializeFrom(serialized_fields);

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

	if(serialized_fields.find("_explosion_indicator_helper") != serialized_fields.end())
	{
		const SerializedField& _explosion_indicator_helper_sf = serialized_fields["_explosion_indicator_helper"];
		if (_explosion_indicator_helper_sf.type_name == "GameObject*")
		{
			_explosion_indicator_helper = std::any_cast<GameObject*>(_explosion_indicator_helper_sf.copy);
		}
	}

	if(serialized_fields.find("_explosion_effect") != serialized_fields.end())
	{
		const SerializedField& _explosion_effect_sf = serialized_fields["_explosion_effect"];
		if (_explosion_effect_sf.type_name == "GameObject*")
		{
			_explosion_effect = std::any_cast<GameObject*>(_explosion_effect_sf.copy);
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

	if(serialized_fields.find("_timer_explosion") != serialized_fields.end())
	{
		const SerializedField& _timer_explosion_sf = serialized_fields["_timer_explosion"];
		if (_timer_explosion_sf.type_name == "float")
		{
			_timer_explosion = std::any_cast<float>(_timer_explosion_sf.copy);
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

	if(serialized_fields.find("_regen_time") != serialized_fields.end())
	{
		const SerializedField& _regen_time_sf = serialized_fields["_regen_time"];
		if (_regen_time_sf.type_name == "float")
		{
			_regen_time = std::any_cast<float>(_regen_time_sf.copy);
		}
	}
}

void Hachiko::Scripting::CrystalExplosion::SerializeTo(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::SerializeTo(serialized_fields);

	serialized_fields["_explosion_crystal"] = SerializedField(std::string("_explosion_crystal"), std::make_any<GameObject*>(_explosion_crystal), std::string("GameObject*"));

	serialized_fields["_static_crystal"] = SerializedField(std::string("_static_crystal"), std::make_any<GameObject*>(_static_crystal), std::string("GameObject*"));

	serialized_fields["_explosion_indicator_helper"] = SerializedField(std::string("_explosion_indicator_helper"), std::make_any<GameObject*>(_explosion_indicator_helper), std::string("GameObject*"));

	serialized_fields["_explosion_effect"] = SerializedField(std::string("_explosion_effect"), std::make_any<GameObject*>(_explosion_effect), std::string("GameObject*"));

	serialized_fields["_crashing_index"] = SerializedField(std::string("_crashing_index"), std::make_any<unsigned>(_crashing_index), std::string("unsigned"));

	serialized_fields["_detecting_radius"] = SerializedField(std::string("_detecting_radius"), std::make_any<float>(_detecting_radius), std::string("float"));

	serialized_fields["_explosion_radius"] = SerializedField(std::string("_explosion_radius"), std::make_any<float>(_explosion_radius), std::string("float"));

	serialized_fields["_timer_explosion"] = SerializedField(std::string("_timer_explosion"), std::make_any<float>(_timer_explosion), std::string("float"));

	serialized_fields["_explosive_crystal"] = SerializedField(std::string("_explosive_crystal"), std::make_any<bool>(_explosive_crystal), std::string("bool"));

	serialized_fields["_regen_time"] = SerializedField(std::string("_regen_time"), std::make_any<float>(_regen_time), std::string("float"));
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

	if(serialized_fields.find("_worm") != serialized_fields.end())
	{
		const SerializedField& _worm_sf = serialized_fields["_worm"];
		if (_worm_sf.type_name == "bool")
		{
			_worm = std::any_cast<bool>(_worm_sf.copy);
		}
	}

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
		if (_attack_range_sf.type_name == "float")
		{
			_attack_range = std::any_cast<float>(_attack_range_sf.copy);
		}
	}

	if(serialized_fields.find("_attack_delay") != serialized_fields.end())
	{
		const SerializedField& _attack_delay_sf = serialized_fields["_attack_delay"];
		if (_attack_delay_sf.type_name == "float")
		{
			_attack_delay = std::any_cast<float>(_attack_delay_sf.copy);
		}
	}

	if(serialized_fields.find("_idle_cooldown") != serialized_fields.end())
	{
		const SerializedField& _idle_cooldown_sf = serialized_fields["_idle_cooldown"];
		if (_idle_cooldown_sf.type_name == "float")
		{
			_idle_cooldown = std::any_cast<float>(_idle_cooldown_sf.copy);
		}
	}

	if(serialized_fields.find("_spawning_time") != serialized_fields.end())
	{
		const SerializedField& _spawning_time_sf = serialized_fields["_spawning_time"];
		if (_spawning_time_sf.type_name == "float")
		{
			_spawning_time = std::any_cast<float>(_spawning_time_sf.copy);
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

	if(serialized_fields.find("_enemy_body") != serialized_fields.end())
	{
		const SerializedField& _enemy_body_sf = serialized_fields["_enemy_body"];
		if (_enemy_body_sf.type_name == "GameObject*")
		{
			_enemy_body = std::any_cast<GameObject*>(_enemy_body_sf.copy);
		}
	}

	if(serialized_fields.find("_parasite") != serialized_fields.end())
	{
		const SerializedField& _parasite_sf = serialized_fields["_parasite"];
		if (_parasite_sf.type_name == "GameObject*")
		{
			_parasite = std::any_cast<GameObject*>(_parasite_sf.copy);
		}
	}

	if(serialized_fields.find("_blood_trail") != serialized_fields.end())
	{
		const SerializedField& _blood_trail_sf = serialized_fields["_blood_trail"];
		if (_blood_trail_sf.type_name == "GameObject*")
		{
			_blood_trail = std::any_cast<GameObject*>(_blood_trail_sf.copy);
		}
	}

	if(serialized_fields.find("_small_dust") != serialized_fields.end())
	{
		const SerializedField& _small_dust_sf = serialized_fields["_small_dust"];
		if (_small_dust_sf.type_name == "GameObject*")
		{
			_small_dust = std::any_cast<GameObject*>(_small_dust_sf.copy);
		}
	}

	if(serialized_fields.find("_big_dust") != serialized_fields.end())
	{
		const SerializedField& _big_dust_sf = serialized_fields["_big_dust"];
		if (_big_dust_sf.type_name == "GameObject*")
		{
			_big_dust = std::any_cast<GameObject*>(_big_dust_sf.copy);
		}
	}

	if(serialized_fields.find("_attack_zone") != serialized_fields.end())
	{
		const SerializedField& _attack_zone_sf = serialized_fields["_attack_zone"];
		if (_attack_zone_sf.type_name == "GameObject*")
		{
			_attack_zone = std::any_cast<GameObject*>(_attack_zone_sf.copy);
		}
	}

	if(serialized_fields.find("_inner_indicator") != serialized_fields.end())
	{
		const SerializedField& _inner_indicator_sf = serialized_fields["_inner_indicator"];
		if (_inner_indicator_sf.type_name == "GameObject*")
		{
			_inner_indicator = std::any_cast<GameObject*>(_inner_indicator_sf.copy);
		}
	}

	if(serialized_fields.find("_outer_indicator") != serialized_fields.end())
	{
		const SerializedField& _outer_indicator_sf = serialized_fields["_outer_indicator"];
		if (_outer_indicator_sf.type_name == "GameObject*")
		{
			_outer_indicator = std::any_cast<GameObject*>(_outer_indicator_sf.copy);
		}
	}

	if(serialized_fields.find("_already_in_combat") != serialized_fields.end())
	{
		const SerializedField& _already_in_combat_sf = serialized_fields["_already_in_combat"];
		if (_already_in_combat_sf.type_name == "bool")
		{
			_already_in_combat = std::any_cast<bool>(_already_in_combat_sf.copy);
		}
	}

	if(serialized_fields.find("_is_from_gautlet") != serialized_fields.end())
	{
		const SerializedField& _is_from_gautlet_sf = serialized_fields["_is_from_gautlet"];
		if (_is_from_gautlet_sf.type_name == "bool")
		{
			_is_from_gautlet = std::any_cast<bool>(_is_from_gautlet_sf.copy);
		}
	}

	if(serialized_fields.find("_will_die") != serialized_fields.end())
	{
		const SerializedField& _will_die_sf = serialized_fields["_will_die"];
		if (_will_die_sf.type_name == "bool")
		{
			_will_die = std::any_cast<bool>(_will_die_sf.copy);
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

	if(serialized_fields.find("_current_spawning_time") != serialized_fields.end())
	{
		const SerializedField& _current_spawning_time_sf = serialized_fields["_current_spawning_time"];
		if (_current_spawning_time_sf.type_name == "float")
		{
			_current_spawning_time = std::any_cast<float>(_current_spawning_time_sf.copy);
		}
	}
}

void Hachiko::Scripting::EnemyController::SerializeTo(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::SerializeTo(serialized_fields);

	serialized_fields["_worm"] = SerializedField(std::string("_worm"), std::make_any<bool>(_worm), std::string("bool"));

	serialized_fields["_aggro_range"] = SerializedField(std::string("_aggro_range"), std::make_any<int>(_aggro_range), std::string("int"));

	serialized_fields["_attack_range"] = SerializedField(std::string("_attack_range"), std::make_any<float>(_attack_range), std::string("float"));

	serialized_fields["_attack_delay"] = SerializedField(std::string("_attack_delay"), std::make_any<float>(_attack_delay), std::string("float"));

	serialized_fields["_idle_cooldown"] = SerializedField(std::string("_idle_cooldown"), std::make_any<float>(_idle_cooldown), std::string("float"));

	serialized_fields["_spawning_time"] = SerializedField(std::string("_spawning_time"), std::make_any<float>(_spawning_time), std::string("float"));

	serialized_fields["_spawn_pos"] = SerializedField(std::string("_spawn_pos"), std::make_any<float3>(_spawn_pos), std::string("float3"));

	serialized_fields["_spawn_is_initial"] = SerializedField(std::string("_spawn_is_initial"), std::make_any<bool>(_spawn_is_initial), std::string("bool"));

	serialized_fields["_enemy_body"] = SerializedField(std::string("_enemy_body"), std::make_any<GameObject*>(_enemy_body), std::string("GameObject*"));

	serialized_fields["_parasite"] = SerializedField(std::string("_parasite"), std::make_any<GameObject*>(_parasite), std::string("GameObject*"));

	serialized_fields["_blood_trail"] = SerializedField(std::string("_blood_trail"), std::make_any<GameObject*>(_blood_trail), std::string("GameObject*"));

	serialized_fields["_small_dust"] = SerializedField(std::string("_small_dust"), std::make_any<GameObject*>(_small_dust), std::string("GameObject*"));

	serialized_fields["_big_dust"] = SerializedField(std::string("_big_dust"), std::make_any<GameObject*>(_big_dust), std::string("GameObject*"));

	serialized_fields["_attack_zone"] = SerializedField(std::string("_attack_zone"), std::make_any<GameObject*>(_attack_zone), std::string("GameObject*"));

	serialized_fields["_inner_indicator"] = SerializedField(std::string("_inner_indicator"), std::make_any<GameObject*>(_inner_indicator), std::string("GameObject*"));

	serialized_fields["_outer_indicator"] = SerializedField(std::string("_outer_indicator"), std::make_any<GameObject*>(_outer_indicator), std::string("GameObject*"));

	serialized_fields["_already_in_combat"] = SerializedField(std::string("_already_in_combat"), std::make_any<bool>(_already_in_combat), std::string("bool"));

	serialized_fields["_is_from_gautlet"] = SerializedField(std::string("_is_from_gautlet"), std::make_any<bool>(_is_from_gautlet), std::string("bool"));

	serialized_fields["_will_die"] = SerializedField(std::string("_will_die"), std::make_any<bool>(_will_die), std::string("bool"));

	serialized_fields["_attack_animation_duration"] = SerializedField(std::string("_attack_animation_duration"), std::make_any<float>(_attack_animation_duration), std::string("float"));

	serialized_fields["_attack_animation_timer"] = SerializedField(std::string("_attack_animation_timer"), std::make_any<float>(_attack_animation_timer), std::string("float"));

	serialized_fields["_current_spawning_time"] = SerializedField(std::string("_current_spawning_time"), std::make_any<float>(_current_spawning_time), std::string("float"));
}

void Hachiko::Scripting::CombatManager::DeserializeFrom(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::DeserializeFrom(serialized_fields);

	if(serialized_fields.find("_charge_vfx") != serialized_fields.end())
	{
		const SerializedField& _charge_vfx_sf = serialized_fields["_charge_vfx"];
		if (_charge_vfx_sf.type_name == "GameObject*")
		{
			_charge_vfx = std::any_cast<GameObject*>(_charge_vfx_sf.copy);
		}
	}

	if(serialized_fields.find("_shot_vfx") != serialized_fields.end())
	{
		const SerializedField& _shot_vfx_sf = serialized_fields["_shot_vfx"];
		if (_shot_vfx_sf.type_name == "GameObject*")
		{
			_shot_vfx = std::any_cast<GameObject*>(_shot_vfx_sf.copy);
		}
	}
}

void Hachiko::Scripting::CombatManager::SerializeTo(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::SerializeTo(serialized_fields);

	serialized_fields["_charge_vfx"] = SerializedField(std::string("_charge_vfx"), std::make_any<GameObject*>(_charge_vfx), std::string("GameObject*"));

	serialized_fields["_shot_vfx"] = SerializedField(std::string("_shot_vfx"), std::make_any<GameObject*>(_shot_vfx), std::string("GameObject*"));
}

void Hachiko::Scripting::CombatVisualEffectsPool::DeserializeFrom(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::DeserializeFrom(serialized_fields);

	if(serialized_fields.find("_current_billboard_index") != serialized_fields.end())
	{
		const SerializedField& _current_billboard_index_sf = serialized_fields["_current_billboard_index"];
		if (_current_billboard_index_sf.type_name == "int")
		{
			_current_billboard_index = std::any_cast<int>(_current_billboard_index_sf.copy);
		}
	}
}

void Hachiko::Scripting::CombatVisualEffectsPool::SerializeTo(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::SerializeTo(serialized_fields);

	serialized_fields["_current_billboard_index"] = SerializedField(std::string("_current_billboard_index"), std::make_any<int>(_current_billboard_index), std::string("int"));
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

	if(serialized_fields.find("_objective") != serialized_fields.end())
	{
		const SerializedField& _objective_sf = serialized_fields["_objective"];
		if (_objective_sf.type_name == "GameObject*")
		{
			_objective = std::any_cast<GameObject*>(_objective_sf.copy);
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

	serialized_fields["_objective"] = SerializedField(std::string("_objective"), std::make_any<GameObject*>(_objective), std::string("GameObject*"));

	serialized_fields["_follow_delay"] = SerializedField(std::string("_follow_delay"), std::make_any<float>(_follow_delay), std::string("float"));
}

void Hachiko::Scripting::PlayerController::DeserializeFrom(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::DeserializeFrom(serialized_fields);

	if(serialized_fields.find("_state") != serialized_fields.end())
	{
		const SerializedField& _state_sf = serialized_fields["_state"];
		if (_state_sf.type_name == "PlayerState")
		{
			_state = std::any_cast<PlayerState>(_state_sf.copy);
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

	if(serialized_fields.find("_sword_weapon") != serialized_fields.end())
	{
		const SerializedField& _sword_weapon_sf = serialized_fields["_sword_weapon"];
		if (_sword_weapon_sf.type_name == "GameObject*")
		{
			_sword_weapon = std::any_cast<GameObject*>(_sword_weapon_sf.copy);
		}
	}

	if(serialized_fields.find("_sword_upper") != serialized_fields.end())
	{
		const SerializedField& _sword_upper_sf = serialized_fields["_sword_upper"];
		if (_sword_upper_sf.type_name == "GameObject*")
		{
			_sword_upper = std::any_cast<GameObject*>(_sword_upper_sf.copy);
		}
	}

	if(serialized_fields.find("_claw_weapon") != serialized_fields.end())
	{
		const SerializedField& _claw_weapon_sf = serialized_fields["_claw_weapon"];
		if (_claw_weapon_sf.type_name == "GameObject*")
		{
			_claw_weapon = std::any_cast<GameObject*>(_claw_weapon_sf.copy);
		}
	}

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

	if(serialized_fields.find("_player_geometry") != serialized_fields.end())
	{
		const SerializedField& _player_geometry_sf = serialized_fields["_player_geometry"];
		if (_player_geometry_sf.type_name == "GameObject*")
		{
			_player_geometry = std::any_cast<GameObject*>(_player_geometry_sf.copy);
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

	if(serialized_fields.find("_invulnerability_time") != serialized_fields.end())
	{
		const SerializedField& _invulnerability_time_sf = serialized_fields["_invulnerability_time"];
		if (_invulnerability_time_sf.type_name == "float")
		{
			_invulnerability_time = std::any_cast<float>(_invulnerability_time_sf.copy);
		}
	}

	if(serialized_fields.find("_dash_scaler") != serialized_fields.end())
	{
		const SerializedField& _dash_scaler_sf = serialized_fields["_dash_scaler"];
		if (_dash_scaler_sf.type_name == "unsigned")
		{
			_dash_scaler = std::any_cast<unsigned>(_dash_scaler_sf.copy);
		}
	}

	if(serialized_fields.find("_max_dash_charges") != serialized_fields.end())
	{
		const SerializedField& _max_dash_charges_sf = serialized_fields["_max_dash_charges"];
		if (_max_dash_charges_sf.type_name == "unsigned")
		{
			_max_dash_charges = std::any_cast<unsigned>(_max_dash_charges_sf.copy);
		}
	}

	if(serialized_fields.find("_dash_trail") != serialized_fields.end())
	{
		const SerializedField& _dash_trail_sf = serialized_fields["_dash_trail"];
		if (_dash_trail_sf.type_name == "GameObject*")
		{
			_dash_trail = std::any_cast<GameObject*>(_dash_trail_sf.copy);
		}
	}

	if(serialized_fields.find("_trail_enlarger") != serialized_fields.end())
	{
		const SerializedField& _trail_enlarger_sf = serialized_fields["_trail_enlarger"];
		if (_trail_enlarger_sf.type_name == "float")
		{
			_trail_enlarger = std::any_cast<float>(_trail_enlarger_sf.copy);
		}
	}

	if(serialized_fields.find("_falling_dust") != serialized_fields.end())
	{
		const SerializedField& _falling_dust_sf = serialized_fields["_falling_dust"];
		if (_falling_dust_sf.type_name == "GameObject*")
		{
			_falling_dust = std::any_cast<GameObject*>(_falling_dust_sf.copy);
		}
	}

	if(serialized_fields.find("_walking_dust") != serialized_fields.end())
	{
		const SerializedField& _walking_dust_sf = serialized_fields["_walking_dust"];
		if (_walking_dust_sf.type_name == "GameObject*")
		{
			_walking_dust = std::any_cast<GameObject*>(_walking_dust_sf.copy);
		}
	}

	if(serialized_fields.find("_heal_effect") != serialized_fields.end())
	{
		const SerializedField& _heal_effect_sf = serialized_fields["_heal_effect"];
		if (_heal_effect_sf.type_name == "GameObject*")
		{
			_heal_effect = std::any_cast<GameObject*>(_heal_effect_sf.copy);
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

	if(serialized_fields.find("_ammo_cell_1") != serialized_fields.end())
	{
		const SerializedField& _ammo_cell_1_sf = serialized_fields["_ammo_cell_1"];
		if (_ammo_cell_1_sf.type_name == "GameObject*")
		{
			_ammo_cell_1 = std::any_cast<GameObject*>(_ammo_cell_1_sf.copy);
		}
	}

	if(serialized_fields.find("_ammo_cell_2") != serialized_fields.end())
	{
		const SerializedField& _ammo_cell_2_sf = serialized_fields["_ammo_cell_2"];
		if (_ammo_cell_2_sf.type_name == "GameObject*")
		{
			_ammo_cell_2 = std::any_cast<GameObject*>(_ammo_cell_2_sf.copy);
		}
	}

	if(serialized_fields.find("_ammo_cell_3") != serialized_fields.end())
	{
		const SerializedField& _ammo_cell_3_sf = serialized_fields["_ammo_cell_3"];
		if (_ammo_cell_3_sf.type_name == "GameObject*")
		{
			_ammo_cell_3 = std::any_cast<GameObject*>(_ammo_cell_3_sf.copy);
		}
	}

	if(serialized_fields.find("_ammo_cell_4") != serialized_fields.end())
	{
		const SerializedField& _ammo_cell_4_sf = serialized_fields["_ammo_cell_4"];
		if (_ammo_cell_4_sf.type_name == "GameObject*")
		{
			_ammo_cell_4 = std::any_cast<GameObject*>(_ammo_cell_4_sf.copy);
		}
	}

	if(serialized_fields.find("_weapon_charge_bar_go") != serialized_fields.end())
	{
		const SerializedField& _weapon_charge_bar_go_sf = serialized_fields["_weapon_charge_bar_go"];
		if (_weapon_charge_bar_go_sf.type_name == "GameObject*")
		{
			_weapon_charge_bar_go = std::any_cast<GameObject*>(_weapon_charge_bar_go_sf.copy);
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
}

void Hachiko::Scripting::PlayerController::SerializeTo(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::SerializeTo(serialized_fields);

	serialized_fields["_state"] = SerializedField(std::string("_state"), std::make_any<PlayerState>(_state), std::string("PlayerState"));

	serialized_fields["_previous_state"] = SerializedField(std::string("_previous_state"), std::make_any<PlayerState>(_previous_state), std::string("PlayerState"));

	serialized_fields["_sword_weapon"] = SerializedField(std::string("_sword_weapon"), std::make_any<GameObject*>(_sword_weapon), std::string("GameObject*"));

	serialized_fields["_sword_upper"] = SerializedField(std::string("_sword_upper"), std::make_any<GameObject*>(_sword_upper), std::string("GameObject*"));

	serialized_fields["_claw_weapon"] = SerializedField(std::string("_claw_weapon"), std::make_any<GameObject*>(_claw_weapon), std::string("GameObject*"));

	serialized_fields["_attack_indicator"] = SerializedField(std::string("_attack_indicator"), std::make_any<GameObject*>(_attack_indicator), std::string("GameObject*"));

	serialized_fields["_bullet_emitter"] = SerializedField(std::string("_bullet_emitter"), std::make_any<GameObject*>(_bullet_emitter), std::string("GameObject*"));

	serialized_fields["_goal"] = SerializedField(std::string("_goal"), std::make_any<GameObject*>(_goal), std::string("GameObject*"));

	serialized_fields["_player_geometry"] = SerializedField(std::string("_player_geometry"), std::make_any<GameObject*>(_player_geometry), std::string("GameObject*"));

	serialized_fields["_dash_duration"] = SerializedField(std::string("_dash_duration"), std::make_any<float>(_dash_duration), std::string("float"));

	serialized_fields["_dash_distance"] = SerializedField(std::string("_dash_distance"), std::make_any<float>(_dash_distance), std::string("float"));

	serialized_fields["_dash_cooldown"] = SerializedField(std::string("_dash_cooldown"), std::make_any<float>(_dash_cooldown), std::string("float"));

	serialized_fields["_invulnerability_time"] = SerializedField(std::string("_invulnerability_time"), std::make_any<float>(_invulnerability_time), std::string("float"));

	serialized_fields["_dash_scaler"] = SerializedField(std::string("_dash_scaler"), std::make_any<unsigned>(_dash_scaler), std::string("unsigned"));

	serialized_fields["_max_dash_charges"] = SerializedField(std::string("_max_dash_charges"), std::make_any<unsigned>(_max_dash_charges), std::string("unsigned"));

	serialized_fields["_dash_trail"] = SerializedField(std::string("_dash_trail"), std::make_any<GameObject*>(_dash_trail), std::string("GameObject*"));

	serialized_fields["_trail_enlarger"] = SerializedField(std::string("_trail_enlarger"), std::make_any<float>(_trail_enlarger), std::string("float"));

	serialized_fields["_falling_dust"] = SerializedField(std::string("_falling_dust"), std::make_any<GameObject*>(_falling_dust), std::string("GameObject*"));

	serialized_fields["_walking_dust"] = SerializedField(std::string("_walking_dust"), std::make_any<GameObject*>(_walking_dust), std::string("GameObject*"));

	serialized_fields["_heal_effect"] = SerializedField(std::string("_heal_effect"), std::make_any<GameObject*>(_heal_effect), std::string("GameObject*"));

	serialized_fields["_rotation_duration"] = SerializedField(std::string("_rotation_duration"), std::make_any<float>(_rotation_duration), std::string("float"));

	serialized_fields["_hp_cell_1"] = SerializedField(std::string("_hp_cell_1"), std::make_any<GameObject*>(_hp_cell_1), std::string("GameObject*"));

	serialized_fields["_hp_cell_2"] = SerializedField(std::string("_hp_cell_2"), std::make_any<GameObject*>(_hp_cell_2), std::string("GameObject*"));

	serialized_fields["_hp_cell_3"] = SerializedField(std::string("_hp_cell_3"), std::make_any<GameObject*>(_hp_cell_3), std::string("GameObject*"));

	serialized_fields["_hp_cell_4"] = SerializedField(std::string("_hp_cell_4"), std::make_any<GameObject*>(_hp_cell_4), std::string("GameObject*"));

	serialized_fields["_ammo_cell_1"] = SerializedField(std::string("_ammo_cell_1"), std::make_any<GameObject*>(_ammo_cell_1), std::string("GameObject*"));

	serialized_fields["_ammo_cell_2"] = SerializedField(std::string("_ammo_cell_2"), std::make_any<GameObject*>(_ammo_cell_2), std::string("GameObject*"));

	serialized_fields["_ammo_cell_3"] = SerializedField(std::string("_ammo_cell_3"), std::make_any<GameObject*>(_ammo_cell_3), std::string("GameObject*"));

	serialized_fields["_ammo_cell_4"] = SerializedField(std::string("_ammo_cell_4"), std::make_any<GameObject*>(_ammo_cell_4), std::string("GameObject*"));

	serialized_fields["_weapon_charge_bar_go"] = SerializedField(std::string("_weapon_charge_bar_go"), std::make_any<GameObject*>(_weapon_charge_bar_go), std::string("GameObject*"));

	serialized_fields["_camera"] = SerializedField(std::string("_camera"), std::make_any<GameObject*>(_camera), std::string("GameObject*"));

	serialized_fields["_ui_damage"] = SerializedField(std::string("_ui_damage"), std::make_any<GameObject*>(_ui_damage), std::string("GameObject*"));
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

void Hachiko::Scripting::AudioManager::DeserializeFrom(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::DeserializeFrom(serialized_fields);

	if(serialized_fields.find("_enemies_in_combat") != serialized_fields.end())
	{
		const SerializedField& _enemies_in_combat_sf = serialized_fields["_enemies_in_combat"];
		if (_enemies_in_combat_sf.type_name == "int")
		{
			_enemies_in_combat = std::any_cast<int>(_enemies_in_combat_sf.copy);
		}
	}

	if(serialized_fields.find("_previous_in_combat") != serialized_fields.end())
	{
		const SerializedField& _previous_in_combat_sf = serialized_fields["_previous_in_combat"];
		if (_previous_in_combat_sf.type_name == "bool")
		{
			_previous_in_combat = std::any_cast<bool>(_previous_in_combat_sf.copy);
		}
	}

	if(serialized_fields.find("enemics") != serialized_fields.end())
	{
		const SerializedField& enemics_sf = serialized_fields["enemics"];
		if (enemics_sf.type_name == "std::vector<GameObject*>")
		{
			enemics = std::any_cast<std::vector<GameObject*>>(enemics_sf.copy);
		}
	}
}

void Hachiko::Scripting::AudioManager::SerializeTo(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::SerializeTo(serialized_fields);

	serialized_fields["_enemies_in_combat"] = SerializedField(std::string("_enemies_in_combat"), std::make_any<int>(_enemies_in_combat), std::string("int"));

	serialized_fields["_previous_in_combat"] = SerializedField(std::string("_previous_in_combat"), std::make_any<bool>(_previous_in_combat), std::string("bool"));

	serialized_fields["enemics"] = SerializedField(std::string("enemics"), std::make_any<std::vector<GameObject*>>(enemics), std::string("std::vector<GameObject*>"));
}

void Hachiko::Scripting::BlinkingLight::DeserializeFrom(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::DeserializeFrom(serialized_fields);

	if(serialized_fields.find("_use_perlin_noise") != serialized_fields.end())
	{
		const SerializedField& _use_perlin_noise_sf = serialized_fields["_use_perlin_noise"];
		if (_use_perlin_noise_sf.type_name == "bool")
		{
			_use_perlin_noise = std::any_cast<bool>(_use_perlin_noise_sf.copy);
		}
	}

	if(serialized_fields.find("_min_radius") != serialized_fields.end())
	{
		const SerializedField& _min_radius_sf = serialized_fields["_min_radius"];
		if (_min_radius_sf.type_name == "float")
		{
			_min_radius = std::any_cast<float>(_min_radius_sf.copy);
		}
	}

	if(serialized_fields.find("_max_radius") != serialized_fields.end())
	{
		const SerializedField& _max_radius_sf = serialized_fields["_max_radius"];
		if (_max_radius_sf.type_name == "float")
		{
			_max_radius = std::any_cast<float>(_max_radius_sf.copy);
		}
	}

	if(serialized_fields.find("_radius_amplitude") != serialized_fields.end())
	{
		const SerializedField& _radius_amplitude_sf = serialized_fields["_radius_amplitude"];
		if (_radius_amplitude_sf.type_name == "float")
		{
			_radius_amplitude = std::any_cast<float>(_radius_amplitude_sf.copy);
		}
	}

	if(serialized_fields.find("_min_duration") != serialized_fields.end())
	{
		const SerializedField& _min_duration_sf = serialized_fields["_min_duration"];
		if (_min_duration_sf.type_name == "float")
		{
			_min_duration = std::any_cast<float>(_min_duration_sf.copy);
		}
	}

	if(serialized_fields.find("_max_duration") != serialized_fields.end())
	{
		const SerializedField& _max_duration_sf = serialized_fields["_max_duration"];
		if (_max_duration_sf.type_name == "float")
		{
			_max_duration = std::any_cast<float>(_max_duration_sf.copy);
		}
	}

	if(serialized_fields.find("_duration_amplitude") != serialized_fields.end())
	{
		const SerializedField& _duration_amplitude_sf = serialized_fields["_duration_amplitude"];
		if (_duration_amplitude_sf.type_name == "float")
		{
			_duration_amplitude = std::any_cast<float>(_duration_amplitude_sf.copy);
		}
	}

	if(serialized_fields.find("_min_intensity") != serialized_fields.end())
	{
		const SerializedField& _min_intensity_sf = serialized_fields["_min_intensity"];
		if (_min_intensity_sf.type_name == "float")
		{
			_min_intensity = std::any_cast<float>(_min_intensity_sf.copy);
		}
	}

	if(serialized_fields.find("_max_intensity") != serialized_fields.end())
	{
		const SerializedField& _max_intensity_sf = serialized_fields["_max_intensity"];
		if (_max_intensity_sf.type_name == "float")
		{
			_max_intensity = std::any_cast<float>(_max_intensity_sf.copy);
		}
	}

	if(serialized_fields.find("_intensity_amplitude") != serialized_fields.end())
	{
		const SerializedField& _intensity_amplitude_sf = serialized_fields["_intensity_amplitude"];
		if (_intensity_amplitude_sf.type_name == "float")
		{
			_intensity_amplitude = std::any_cast<float>(_intensity_amplitude_sf.copy);
		}
	}

	if(serialized_fields.find("_starting_intensity") != serialized_fields.end())
	{
		const SerializedField& _starting_intensity_sf = serialized_fields["_starting_intensity"];
		if (_starting_intensity_sf.type_name == "float")
		{
			_starting_intensity = std::any_cast<float>(_starting_intensity_sf.copy);
		}
	}

	if(serialized_fields.find("_starting_radius") != serialized_fields.end())
	{
		const SerializedField& _starting_radius_sf = serialized_fields["_starting_radius"];
		if (_starting_radius_sf.type_name == "float")
		{
			_starting_radius = std::any_cast<float>(_starting_radius_sf.copy);
		}
	}

	if(serialized_fields.find("_current_intensity") != serialized_fields.end())
	{
		const SerializedField& _current_intensity_sf = serialized_fields["_current_intensity"];
		if (_current_intensity_sf.type_name == "float")
		{
			_current_intensity = std::any_cast<float>(_current_intensity_sf.copy);
		}
	}

	if(serialized_fields.find("_current_duration") != serialized_fields.end())
	{
		const SerializedField& _current_duration_sf = serialized_fields["_current_duration"];
		if (_current_duration_sf.type_name == "float")
		{
			_current_duration = std::any_cast<float>(_current_duration_sf.copy);
		}
	}

	if(serialized_fields.find("_current_radius") != serialized_fields.end())
	{
		const SerializedField& _current_radius_sf = serialized_fields["_current_radius"];
		if (_current_radius_sf.type_name == "float")
		{
			_current_radius = std::any_cast<float>(_current_radius_sf.copy);
		}
	}

	if(serialized_fields.find("_next_duration") != serialized_fields.end())
	{
		const SerializedField& _next_duration_sf = serialized_fields["_next_duration"];
		if (_next_duration_sf.type_name == "float")
		{
			_next_duration = std::any_cast<float>(_next_duration_sf.copy);
		}
	}

	if(serialized_fields.find("_next_intensity") != serialized_fields.end())
	{
		const SerializedField& _next_intensity_sf = serialized_fields["_next_intensity"];
		if (_next_intensity_sf.type_name == "float")
		{
			_next_intensity = std::any_cast<float>(_next_intensity_sf.copy);
		}
	}

	if(serialized_fields.find("_next_radius") != serialized_fields.end())
	{
		const SerializedField& _next_radius_sf = serialized_fields["_next_radius"];
		if (_next_radius_sf.type_name == "float")
		{
			_next_radius = std::any_cast<float>(_next_radius_sf.copy);
		}
	}
}

void Hachiko::Scripting::BlinkingLight::SerializeTo(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::SerializeTo(serialized_fields);

	serialized_fields["_use_perlin_noise"] = SerializedField(std::string("_use_perlin_noise"), std::make_any<bool>(_use_perlin_noise), std::string("bool"));

	serialized_fields["_min_radius"] = SerializedField(std::string("_min_radius"), std::make_any<float>(_min_radius), std::string("float"));

	serialized_fields["_max_radius"] = SerializedField(std::string("_max_radius"), std::make_any<float>(_max_radius), std::string("float"));

	serialized_fields["_radius_amplitude"] = SerializedField(std::string("_radius_amplitude"), std::make_any<float>(_radius_amplitude), std::string("float"));

	serialized_fields["_min_duration"] = SerializedField(std::string("_min_duration"), std::make_any<float>(_min_duration), std::string("float"));

	serialized_fields["_max_duration"] = SerializedField(std::string("_max_duration"), std::make_any<float>(_max_duration), std::string("float"));

	serialized_fields["_duration_amplitude"] = SerializedField(std::string("_duration_amplitude"), std::make_any<float>(_duration_amplitude), std::string("float"));

	serialized_fields["_min_intensity"] = SerializedField(std::string("_min_intensity"), std::make_any<float>(_min_intensity), std::string("float"));

	serialized_fields["_max_intensity"] = SerializedField(std::string("_max_intensity"), std::make_any<float>(_max_intensity), std::string("float"));

	serialized_fields["_intensity_amplitude"] = SerializedField(std::string("_intensity_amplitude"), std::make_any<float>(_intensity_amplitude), std::string("float"));

	serialized_fields["_starting_intensity"] = SerializedField(std::string("_starting_intensity"), std::make_any<float>(_starting_intensity), std::string("float"));

	serialized_fields["_starting_radius"] = SerializedField(std::string("_starting_radius"), std::make_any<float>(_starting_radius), std::string("float"));

	serialized_fields["_current_intensity"] = SerializedField(std::string("_current_intensity"), std::make_any<float>(_current_intensity), std::string("float"));

	serialized_fields["_current_duration"] = SerializedField(std::string("_current_duration"), std::make_any<float>(_current_duration), std::string("float"));

	serialized_fields["_current_radius"] = SerializedField(std::string("_current_radius"), std::make_any<float>(_current_radius), std::string("float"));

	serialized_fields["_next_duration"] = SerializedField(std::string("_next_duration"), std::make_any<float>(_next_duration), std::string("float"));

	serialized_fields["_next_intensity"] = SerializedField(std::string("_next_intensity"), std::make_any<float>(_next_intensity), std::string("float"));

	serialized_fields["_next_radius"] = SerializedField(std::string("_next_radius"), std::make_any<float>(_next_radius), std::string("float"));
}

void Hachiko::Scripting::CameraPosChange::DeserializeFrom(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::DeserializeFrom(serialized_fields);

	if(serialized_fields.find("_objective") != serialized_fields.end())
	{
		const SerializedField& _objective_sf = serialized_fields["_objective"];
		if (_objective_sf.type_name == "GameObject*")
		{
			_objective = std::any_cast<GameObject*>(_objective_sf.copy);
		}
	}

	if(serialized_fields.find("_do_look_ahead") != serialized_fields.end())
	{
		const SerializedField& _do_look_ahead_sf = serialized_fields["_do_look_ahead"];
		if (_do_look_ahead_sf.type_name == "bool")
		{
			_do_look_ahead = std::any_cast<bool>(_do_look_ahead_sf.copy);
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

	if(serialized_fields.find("_duration") != serialized_fields.end())
	{
		const SerializedField& _duration_sf = serialized_fields["_duration"];
		if (_duration_sf.type_name == "float")
		{
			_duration = std::any_cast<float>(_duration_sf.copy);
		}
	}

	if(serialized_fields.find("_relative_position") != serialized_fields.end())
	{
		const SerializedField& _relative_position_sf = serialized_fields["_relative_position"];
		if (_relative_position_sf.type_name == "float3")
		{
			_relative_position = std::any_cast<float3>(_relative_position_sf.copy);
		}
	}

	if(serialized_fields.find("_rotation") != serialized_fields.end())
	{
		const SerializedField& _rotation_sf = serialized_fields["_rotation"];
		if (_rotation_sf.type_name == "float3")
		{
			_rotation = std::any_cast<float3>(_rotation_sf.copy);
		}
	}
}

void Hachiko::Scripting::CameraPosChange::SerializeTo(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::SerializeTo(serialized_fields);

	serialized_fields["_objective"] = SerializedField(std::string("_objective"), std::make_any<GameObject*>(_objective), std::string("GameObject*"));

	serialized_fields["_do_look_ahead"] = SerializedField(std::string("_do_look_ahead"), std::make_any<bool>(_do_look_ahead), std::string("bool"));

	serialized_fields["_speed"] = SerializedField(std::string("_speed"), std::make_any<float>(_speed), std::string("float"));

	serialized_fields["_duration"] = SerializedField(std::string("_duration"), std::make_any<float>(_duration), std::string("float"));

	serialized_fields["_relative_position"] = SerializedField(std::string("_relative_position"), std::make_any<float3>(_relative_position), std::string("float3"));

	serialized_fields["_rotation"] = SerializedField(std::string("_rotation"), std::make_any<float3>(_rotation), std::string("float3"));
}

void Hachiko::Scripting::CrystalPlatform::DeserializeFrom(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::DeserializeFrom(serialized_fields);

	if(serialized_fields.find("_seconds_before_shaking") != serialized_fields.end())
	{
		const SerializedField& _seconds_before_shaking_sf = serialized_fields["_seconds_before_shaking"];
		if (_seconds_before_shaking_sf.type_name == "float")
		{
			_seconds_before_shaking = std::any_cast<float>(_seconds_before_shaking_sf.copy);
		}
	}

	if(serialized_fields.find("_seconds_shaking") != serialized_fields.end())
	{
		const SerializedField& _seconds_shaking_sf = serialized_fields["_seconds_shaking"];
		if (_seconds_shaking_sf.type_name == "float")
		{
			_seconds_shaking = std::any_cast<float>(_seconds_shaking_sf.copy);
		}
	}

	if(serialized_fields.find("_seconds_to_regen") != serialized_fields.end())
	{
		const SerializedField& _seconds_to_regen_sf = serialized_fields["_seconds_to_regen"];
		if (_seconds_to_regen_sf.type_name == "float")
		{
			_seconds_to_regen = std::any_cast<float>(_seconds_to_regen_sf.copy);
		}
	}

	if(serialized_fields.find("_shake_intensity") != serialized_fields.end())
	{
		const SerializedField& _shake_intensity_sf = serialized_fields["_shake_intensity"];
		if (_shake_intensity_sf.type_name == "float")
		{
			_shake_intensity = std::any_cast<float>(_shake_intensity_sf.copy);
		}
	}

	if(serialized_fields.find("_invisible_obstacle") != serialized_fields.end())
	{
		const SerializedField& _invisible_obstacle_sf = serialized_fields["_invisible_obstacle"];
		if (_invisible_obstacle_sf.type_name == "GameObject*")
		{
			_invisible_obstacle = std::any_cast<GameObject*>(_invisible_obstacle_sf.copy);
		}
	}

	if(serialized_fields.find("_crystal_platform") != serialized_fields.end())
	{
		const SerializedField& _crystal_platform_sf = serialized_fields["_crystal_platform"];
		if (_crystal_platform_sf.type_name == "GameObject*")
		{
			_crystal_platform = std::any_cast<GameObject*>(_crystal_platform_sf.copy);
		}
	}
}

void Hachiko::Scripting::CrystalPlatform::SerializeTo(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::SerializeTo(serialized_fields);

	serialized_fields["_seconds_before_shaking"] = SerializedField(std::string("_seconds_before_shaking"), std::make_any<float>(_seconds_before_shaking), std::string("float"));

	serialized_fields["_seconds_shaking"] = SerializedField(std::string("_seconds_shaking"), std::make_any<float>(_seconds_shaking), std::string("float"));

	serialized_fields["_seconds_to_regen"] = SerializedField(std::string("_seconds_to_regen"), std::make_any<float>(_seconds_to_regen), std::string("float"));

	serialized_fields["_shake_intensity"] = SerializedField(std::string("_shake_intensity"), std::make_any<float>(_shake_intensity), std::string("float"));

	serialized_fields["_invisible_obstacle"] = SerializedField(std::string("_invisible_obstacle"), std::make_any<GameObject*>(_invisible_obstacle), std::string("GameObject*"));

	serialized_fields["_crystal_platform"] = SerializedField(std::string("_crystal_platform"), std::make_any<GameObject*>(_crystal_platform), std::string("GameObject*"));
}

void Hachiko::Scripting::DoorController::DeserializeFrom(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::DeserializeFrom(serialized_fields);

	if(serialized_fields.find("_door_prefab") != serialized_fields.end())
	{
		const SerializedField& _door_prefab_sf = serialized_fields["_door_prefab"];
		if (_door_prefab_sf.type_name == "GameObject*")
		{
			_door_prefab = std::any_cast<GameObject*>(_door_prefab_sf.copy);
		}
	}
}

void Hachiko::Scripting::DoorController::SerializeTo(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::SerializeTo(serialized_fields);

	serialized_fields["_door_prefab"] = SerializedField(std::string("_door_prefab"), std::make_any<GameObject*>(_door_prefab), std::string("GameObject*"));
}

void Hachiko::Scripting::DynamicCamera::DeserializeFrom(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::DeserializeFrom(serialized_fields);

	if(serialized_fields.find("_speed") != serialized_fields.end())
	{
		const SerializedField& _speed_sf = serialized_fields["_speed"];
		if (_speed_sf.type_name == "float")
		{
			_speed = std::any_cast<float>(_speed_sf.copy);
		}
	}
}

void Hachiko::Scripting::DynamicCamera::SerializeTo(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::SerializeTo(serialized_fields);

	serialized_fields["_speed"] = SerializedField(std::string("_speed"), std::make_any<float>(_speed), std::string("float"));
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

void Hachiko::Scripting::GauntletManager::DeserializeFrom(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::DeserializeFrom(serialized_fields);

	if(serialized_fields.find("_combat_manager_go") != serialized_fields.end())
	{
		const SerializedField& _combat_manager_go_sf = serialized_fields["_combat_manager_go"];
		if (_combat_manager_go_sf.type_name == "GameObject*")
		{
			_combat_manager_go = std::any_cast<GameObject*>(_combat_manager_go_sf.copy);
		}
	}

	if(serialized_fields.find("_trigger_radius") != serialized_fields.end())
	{
		const SerializedField& _trigger_radius_sf = serialized_fields["_trigger_radius"];
		if (_trigger_radius_sf.type_name == "float")
		{
			_trigger_radius = std::any_cast<float>(_trigger_radius_sf.copy);
		}
	}

	if(serialized_fields.find("_round_wait_time") != serialized_fields.end())
	{
		const SerializedField& _round_wait_time_sf = serialized_fields["_round_wait_time"];
		if (_round_wait_time_sf.type_name == "float")
		{
			_round_wait_time = std::any_cast<float>(_round_wait_time_sf.copy);
		}
	}

	if(serialized_fields.find("_complete_wait_time") != serialized_fields.end())
	{
		const SerializedField& _complete_wait_time_sf = serialized_fields["_complete_wait_time"];
		if (_complete_wait_time_sf.type_name == "float")
		{
			_complete_wait_time = std::any_cast<float>(_complete_wait_time_sf.copy);
		}
	}

	if(serialized_fields.find("_door_controller_1") != serialized_fields.end())
	{
		const SerializedField& _door_controller_1_sf = serialized_fields["_door_controller_1"];
		if (_door_controller_1_sf.type_name == "GameObject*")
		{
			_door_controller_1 = std::any_cast<GameObject*>(_door_controller_1_sf.copy);
		}
	}

	if(serialized_fields.find("_door_controller_2") != serialized_fields.end())
	{
		const SerializedField& _door_controller_2_sf = serialized_fields["_door_controller_2"];
		if (_door_controller_2_sf.type_name == "GameObject*")
		{
			_door_controller_2 = std::any_cast<GameObject*>(_door_controller_2_sf.copy);
		}
	}

	if(serialized_fields.find("_pack_1") != serialized_fields.end())
	{
		const SerializedField& _pack_1_sf = serialized_fields["_pack_1"];
		if (_pack_1_sf.type_name == "GameObject*")
		{
			_pack_1 = std::any_cast<GameObject*>(_pack_1_sf.copy);
		}
	}

	if(serialized_fields.find("_pack_2") != serialized_fields.end())
	{
		const SerializedField& _pack_2_sf = serialized_fields["_pack_2"];
		if (_pack_2_sf.type_name == "GameObject*")
		{
			_pack_2 = std::any_cast<GameObject*>(_pack_2_sf.copy);
		}
	}

	if(serialized_fields.find("_pack_3") != serialized_fields.end())
	{
		const SerializedField& _pack_3_sf = serialized_fields["_pack_3"];
		if (_pack_3_sf.type_name == "GameObject*")
		{
			_pack_3 = std::any_cast<GameObject*>(_pack_3_sf.copy);
		}
	}
}

void Hachiko::Scripting::GauntletManager::SerializeTo(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::SerializeTo(serialized_fields);

	serialized_fields["_combat_manager_go"] = SerializedField(std::string("_combat_manager_go"), std::make_any<GameObject*>(_combat_manager_go), std::string("GameObject*"));

	serialized_fields["_trigger_radius"] = SerializedField(std::string("_trigger_radius"), std::make_any<float>(_trigger_radius), std::string("float"));

	serialized_fields["_round_wait_time"] = SerializedField(std::string("_round_wait_time"), std::make_any<float>(_round_wait_time), std::string("float"));

	serialized_fields["_complete_wait_time"] = SerializedField(std::string("_complete_wait_time"), std::make_any<float>(_complete_wait_time), std::string("float"));

	serialized_fields["_door_controller_1"] = SerializedField(std::string("_door_controller_1"), std::make_any<GameObject*>(_door_controller_1), std::string("GameObject*"));

	serialized_fields["_door_controller_2"] = SerializedField(std::string("_door_controller_2"), std::make_any<GameObject*>(_door_controller_2), std::string("GameObject*"));

	serialized_fields["_pack_1"] = SerializedField(std::string("_pack_1"), std::make_any<GameObject*>(_pack_1), std::string("GameObject*"));

	serialized_fields["_pack_2"] = SerializedField(std::string("_pack_2"), std::make_any<GameObject*>(_pack_2), std::string("GameObject*"));

	serialized_fields["_pack_3"] = SerializedField(std::string("_pack_3"), std::make_any<GameObject*>(_pack_3), std::string("GameObject*"));
}

void Hachiko::Scripting::LaserController::DeserializeFrom(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::DeserializeFrom(serialized_fields);

	if(serialized_fields.find("_laser") != serialized_fields.end())
	{
		const SerializedField& _laser_sf = serialized_fields["_laser"];
		if (_laser_sf.type_name == "GameObject*")
		{
			_laser = std::any_cast<GameObject*>(_laser_sf.copy);
		}
	}

	if(serialized_fields.find("_max_length") != serialized_fields.end())
	{
		const SerializedField& _max_length_sf = serialized_fields["_max_length"];
		if (_max_length_sf.type_name == "float")
		{
			_max_length = std::any_cast<float>(_max_length_sf.copy);
		}
	}

	if(serialized_fields.find("_max_scale") != serialized_fields.end())
	{
		const SerializedField& _max_scale_sf = serialized_fields["_max_scale"];
		if (_max_scale_sf.type_name == "float")
		{
			_max_scale = std::any_cast<float>(_max_scale_sf.copy);
		}
	}

	if(serialized_fields.find("_activation_time") != serialized_fields.end())
	{
		const SerializedField& _activation_time_sf = serialized_fields["_activation_time"];
		if (_activation_time_sf.type_name == "float")
		{
			_activation_time = std::any_cast<float>(_activation_time_sf.copy);
		}
	}

	if(serialized_fields.find("_damage") != serialized_fields.end())
	{
		const SerializedField& _damage_sf = serialized_fields["_damage"];
		if (_damage_sf.type_name == "float")
		{
			_damage = std::any_cast<float>(_damage_sf.copy);
		}
	}

	if(serialized_fields.find("_spin_movement") != serialized_fields.end())
	{
		const SerializedField& _spin_movement_sf = serialized_fields["_spin_movement"];
		if (_spin_movement_sf.type_name == "bool")
		{
			_spin_movement = std::any_cast<bool>(_spin_movement_sf.copy);
		}
	}

	if(serialized_fields.find("_spin_speed") != serialized_fields.end())
	{
		const SerializedField& _spin_speed_sf = serialized_fields["_spin_speed"];
		if (_spin_speed_sf.type_name == "float")
		{
			_spin_speed = std::any_cast<float>(_spin_speed_sf.copy);
		}
	}

	if(serialized_fields.find("_horizonal_movement") != serialized_fields.end())
	{
		const SerializedField& _horizonal_movement_sf = serialized_fields["_horizonal_movement"];
		if (_horizonal_movement_sf.type_name == "bool")
		{
			_horizonal_movement = std::any_cast<bool>(_horizonal_movement_sf.copy);
		}
	}

	if(serialized_fields.find("_movement_speed") != serialized_fields.end())
	{
		const SerializedField& _movement_speed_sf = serialized_fields["_movement_speed"];
		if (_movement_speed_sf.type_name == "float")
		{
			_movement_speed = std::any_cast<float>(_movement_speed_sf.copy);
		}
	}

	if(serialized_fields.find("_movement_target") != serialized_fields.end())
	{
		const SerializedField& _movement_target_sf = serialized_fields["_movement_target"];
		if (_movement_target_sf.type_name == "GameObject*")
		{
			_movement_target = std::any_cast<GameObject*>(_movement_target_sf.copy);
		}
	}

	if(serialized_fields.find("_toggle_activation") != serialized_fields.end())
	{
		const SerializedField& _toggle_activation_sf = serialized_fields["_toggle_activation"];
		if (_toggle_activation_sf.type_name == "bool")
		{
			_toggle_activation = std::any_cast<bool>(_toggle_activation_sf.copy);
		}
	}

	if(serialized_fields.find("_toggle_active_time") != serialized_fields.end())
	{
		const SerializedField& _toggle_active_time_sf = serialized_fields["_toggle_active_time"];
		if (_toggle_active_time_sf.type_name == "float")
		{
			_toggle_active_time = std::any_cast<float>(_toggle_active_time_sf.copy);
		}
	}

	if(serialized_fields.find("_toggle_inactive_time") != serialized_fields.end())
	{
		const SerializedField& _toggle_inactive_time_sf = serialized_fields["_toggle_inactive_time"];
		if (_toggle_inactive_time_sf.type_name == "float")
		{
			_toggle_inactive_time = std::any_cast<float>(_toggle_inactive_time_sf.copy);
		}
	}
}

void Hachiko::Scripting::LaserController::SerializeTo(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::SerializeTo(serialized_fields);

	serialized_fields["_laser"] = SerializedField(std::string("_laser"), std::make_any<GameObject*>(_laser), std::string("GameObject*"));

	serialized_fields["_max_length"] = SerializedField(std::string("_max_length"), std::make_any<float>(_max_length), std::string("float"));

	serialized_fields["_max_scale"] = SerializedField(std::string("_max_scale"), std::make_any<float>(_max_scale), std::string("float"));

	serialized_fields["_activation_time"] = SerializedField(std::string("_activation_time"), std::make_any<float>(_activation_time), std::string("float"));

	serialized_fields["_damage"] = SerializedField(std::string("_damage"), std::make_any<float>(_damage), std::string("float"));

	serialized_fields["_spin_movement"] = SerializedField(std::string("_spin_movement"), std::make_any<bool>(_spin_movement), std::string("bool"));

	serialized_fields["_spin_speed"] = SerializedField(std::string("_spin_speed"), std::make_any<float>(_spin_speed), std::string("float"));

	serialized_fields["_horizonal_movement"] = SerializedField(std::string("_horizonal_movement"), std::make_any<bool>(_horizonal_movement), std::string("bool"));

	serialized_fields["_movement_speed"] = SerializedField(std::string("_movement_speed"), std::make_any<float>(_movement_speed), std::string("float"));

	serialized_fields["_movement_target"] = SerializedField(std::string("_movement_target"), std::make_any<GameObject*>(_movement_target), std::string("GameObject*"));

	serialized_fields["_toggle_activation"] = SerializedField(std::string("_toggle_activation"), std::make_any<bool>(_toggle_activation), std::string("bool"));

	serialized_fields["_toggle_active_time"] = SerializedField(std::string("_toggle_active_time"), std::make_any<float>(_toggle_active_time), std::string("float"));

	serialized_fields["_toggle_inactive_time"] = SerializedField(std::string("_toggle_inactive_time"), std::make_any<float>(_toggle_inactive_time), std::string("float"));
}

void Hachiko::Scripting::LevelManager::DeserializeFrom(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::DeserializeFrom(serialized_fields);

	if(serialized_fields.find("_respawn_position") != serialized_fields.end())
	{
		const SerializedField& _respawn_position_sf = serialized_fields["_respawn_position"];
		if (_respawn_position_sf.type_name == "float3")
		{
			_respawn_position = std::any_cast<float3>(_respawn_position_sf.copy);
		}
	}

	if(serialized_fields.find("_gauntlet_ui_go") != serialized_fields.end())
	{
		const SerializedField& _gauntlet_ui_go_sf = serialized_fields["_gauntlet_ui_go"];
		if (_gauntlet_ui_go_sf.type_name == "GameObject*")
		{
			_gauntlet_ui_go = std::any_cast<GameObject*>(_gauntlet_ui_go_sf.copy);
		}
	}

	if(serialized_fields.find("_gauntlet_counter_go") != serialized_fields.end())
	{
		const SerializedField& _gauntlet_counter_go_sf = serialized_fields["_gauntlet_counter_go"];
		if (_gauntlet_counter_go_sf.type_name == "GameObject*")
		{
			_gauntlet_counter_go = std::any_cast<GameObject*>(_gauntlet_counter_go_sf.copy);
		}
	}
}

void Hachiko::Scripting::LevelManager::SerializeTo(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::SerializeTo(serialized_fields);

	serialized_fields["_respawn_position"] = SerializedField(std::string("_respawn_position"), std::make_any<float3>(_respawn_position), std::string("float3"));

	serialized_fields["_gauntlet_ui_go"] = SerializedField(std::string("_gauntlet_ui_go"), std::make_any<GameObject*>(_gauntlet_ui_go), std::string("GameObject*"));

	serialized_fields["_gauntlet_counter_go"] = SerializedField(std::string("_gauntlet_counter_go"), std::make_any<GameObject*>(_gauntlet_counter_go), std::string("GameObject*"));
}

void Hachiko::Scripting::PillarCheckpoint::DeserializeFrom(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::DeserializeFrom(serialized_fields);

	if(serialized_fields.find("_respawn_go") != serialized_fields.end())
	{
		const SerializedField& _respawn_go_sf = serialized_fields["_respawn_go"];
		if (_respawn_go_sf.type_name == "GameObject*")
		{
			_respawn_go = std::any_cast<GameObject*>(_respawn_go_sf.copy);
		}
	}

	if(serialized_fields.find("_activation_range") != serialized_fields.end())
	{
		const SerializedField& _activation_range_sf = serialized_fields["_activation_range"];
		if (_activation_range_sf.type_name == "float")
		{
			_activation_range = std::any_cast<float>(_activation_range_sf.copy);
		}
	}
}

void Hachiko::Scripting::PillarCheckpoint::SerializeTo(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::SerializeTo(serialized_fields);

	serialized_fields["_respawn_go"] = SerializedField(std::string("_respawn_go"), std::make_any<GameObject*>(_respawn_go), std::string("GameObject*"));

	serialized_fields["_activation_range"] = SerializedField(std::string("_activation_range"), std::make_any<float>(_activation_range), std::string("float"));
}

void Hachiko::Scripting::Spawner::DeserializeFrom(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::DeserializeFrom(serialized_fields);

	if(serialized_fields.find("_enemy_pack") != serialized_fields.end())
	{
		const SerializedField& _enemy_pack_sf = serialized_fields["_enemy_pack"];
		if (_enemy_pack_sf.type_name == "GameObject*")
		{
			_enemy_pack = std::any_cast<GameObject*>(_enemy_pack_sf.copy);
		}
	}
}

void Hachiko::Scripting::Spawner::SerializeTo(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::SerializeTo(serialized_fields);

	serialized_fields["_enemy_pack"] = SerializedField(std::string("_enemy_pack"), std::make_any<GameObject*>(_enemy_pack), std::string("GameObject*"));
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

	if(serialized_fields.find("_game_title") != serialized_fields.end())
	{
		const SerializedField& _game_title_sf = serialized_fields["_game_title"];
		if (_game_title_sf.type_name == "GameObject*")
		{
			_game_title = std::any_cast<GameObject*>(_game_title_sf.copy);
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

	serialized_fields["_game_title"] = SerializedField(std::string("_game_title"), std::make_any<GameObject*>(_game_title), std::string("GameObject*"));

	serialized_fields["_settings"] = SerializedField(std::string("_settings"), std::make_any<GameObject*>(_settings), std::string("GameObject*"));

	serialized_fields["_credits"] = SerializedField(std::string("_credits"), std::make_any<GameObject*>(_credits), std::string("GameObject*"));

	serialized_fields["_button_back"] = SerializedField(std::string("_button_back"), std::make_any<ComponentButton*>(_button_back), std::string("ComponentButton*"));
}