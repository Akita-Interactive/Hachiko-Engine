#include <vector>
#include <string>
#include <unordered_map>
#include "scriptingUtil/gameplaypch.h"
#include "BackToMainMenu.h"
#include "DynamicCamera.h"
#include "MainMenuManager.h"
#include "PlayerCamera.h"
#include "PlayerController.h"


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

void Hachiko::Scripting::PlayerCamera::DeserializeFrom(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::DeserializeFrom(serialized_fields);

	if(serialized_fields.find("_distance_to_player") != serialized_fields.end())
	{
		const SerializedField& _distance_to_player_sf = serialized_fields["_distance_to_player"];
		if (_distance_to_player_sf.type_name == "math::float3")
		{
			_distance_to_player = std::any_cast<math::float3>(_distance_to_player_sf.copy);
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
}

void Hachiko::Scripting::PlayerCamera::SerializeTo(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::SerializeTo(serialized_fields);

	serialized_fields["_distance_to_player"] = SerializedField(std::string("_distance_to_player"), std::make_any<math::float3>(_distance_to_player), std::string("math::float3"));

	serialized_fields["_player"] = SerializedField(std::string("_player"), std::make_any<GameObject*>(_player), std::string("GameObject*"));
}

void Hachiko::Scripting::PlayerController::DeserializeFrom(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::DeserializeFrom(serialized_fields);

	if(serialized_fields.find("_movement_speed") != serialized_fields.end())
	{
		const SerializedField& _movement_speed_sf = serialized_fields["_movement_speed"];
		if (_movement_speed_sf.type_name == "float")
		{
			_movement_speed = std::any_cast<float>(_movement_speed_sf.copy);
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

	if(serialized_fields.find("_is_dashing") != serialized_fields.end())
	{
		const SerializedField& _is_dashing_sf = serialized_fields["_is_dashing"];
		if (_is_dashing_sf.type_name == "bool")
		{
			_is_dashing = std::any_cast<bool>(_is_dashing_sf.copy);
		}
	}

	if(serialized_fields.find("_dash_progress") != serialized_fields.end())
	{
		const SerializedField& _dash_progress_sf = serialized_fields["_dash_progress"];
		if (_dash_progress_sf.type_name == "float")
		{
			_dash_progress = std::any_cast<float>(_dash_progress_sf.copy);
		}
	}

	if(serialized_fields.find("_dash_start") != serialized_fields.end())
	{
		const SerializedField& _dash_start_sf = serialized_fields["_dash_start"];
		if (_dash_start_sf.type_name == "math::float3")
		{
			_dash_start = std::any_cast<math::float3>(_dash_start_sf.copy);
		}
	}

	if(serialized_fields.find("_rotation_speed") != serialized_fields.end())
	{
		const SerializedField& _rotation_speed_sf = serialized_fields["_rotation_speed"];
		if (_rotation_speed_sf.type_name == "float")
		{
			_rotation_speed = std::any_cast<float>(_rotation_speed_sf.copy);
		}
	}

	if(serialized_fields.find("_is_falling") != serialized_fields.end())
	{
		const SerializedField& _is_falling_sf = serialized_fields["_is_falling"];
		if (_is_falling_sf.type_name == "bool")
		{
			_is_falling = std::any_cast<bool>(_is_falling_sf.copy);
		}
	}

	if(serialized_fields.find("_original_y") != serialized_fields.end())
	{
		const SerializedField& _original_y_sf = serialized_fields["_original_y"];
		if (_original_y_sf.type_name == "float")
		{
			_original_y = std::any_cast<float>(_original_y_sf.copy);
		}
	}

	if(serialized_fields.find("_speed_y") != serialized_fields.end())
	{
		const SerializedField& _speed_y_sf = serialized_fields["_speed_y"];
		if (_speed_y_sf.type_name == "float")
		{
			_speed_y = std::any_cast<float>(_speed_y_sf.copy);
		}
	}

	if(serialized_fields.find("_starting_position") != serialized_fields.end())
	{
		const SerializedField& _starting_position_sf = serialized_fields["_starting_position"];
		if (_starting_position_sf.type_name == "math::float3")
		{
			_starting_position = std::any_cast<math::float3>(_starting_position_sf.copy);
		}
	}
}

void Hachiko::Scripting::PlayerController::SerializeTo(std::unordered_map<std::string, SerializedField>& serialized_fields)
{
	Hachiko::Scripting::Script::SerializeTo(serialized_fields);

	serialized_fields["_movement_speed"] = SerializedField(std::string("_movement_speed"), std::make_any<float>(_movement_speed), std::string("float"));

	serialized_fields["_dash_duration"] = SerializedField(std::string("_dash_duration"), std::make_any<float>(_dash_duration), std::string("float"));

	serialized_fields["_dash_distance"] = SerializedField(std::string("_dash_distance"), std::make_any<float>(_dash_distance), std::string("float"));

	serialized_fields["_is_dashing"] = SerializedField(std::string("_is_dashing"), std::make_any<bool>(_is_dashing), std::string("bool"));

	serialized_fields["_dash_progress"] = SerializedField(std::string("_dash_progress"), std::make_any<float>(_dash_progress), std::string("float"));

	serialized_fields["_dash_start"] = SerializedField(std::string("_dash_start"), std::make_any<math::float3>(_dash_start), std::string("math::float3"));

	serialized_fields["_rotation_speed"] = SerializedField(std::string("_rotation_speed"), std::make_any<float>(_rotation_speed), std::string("float"));

	serialized_fields["_is_falling"] = SerializedField(std::string("_is_falling"), std::make_any<bool>(_is_falling), std::string("bool"));

	serialized_fields["_original_y"] = SerializedField(std::string("_original_y"), std::make_any<float>(_original_y), std::string("float"));

	serialized_fields["_speed_y"] = SerializedField(std::string("_speed_y"), std::make_any<float>(_speed_y), std::string("float"));

	serialized_fields["_starting_position"] = SerializedField(std::string("_starting_position"), std::make_any<math::float3>(_starting_position), std::string("math::float3"));
}