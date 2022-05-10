#include "scriptingUtil/gameplaypch.h"
#include "BackToMainMenu.h"
#include "DynamicCamera.h"
#include "MainMenuManager.h"
#include "PlayerCamera.h"
#include "PlayerController.h"


void Hachiko::Scripting::BackToMainMenu::OnEditor()
{
	Editor::Show<ComponentButton>("_button_back", "ComponentButton*", _button_back);
}

void Hachiko::Scripting::DynamicCamera::OnEditor()
{
	Editor::Show("_start_point", _start_point);
	Editor::Show("_end_point", _end_point);
	Editor::Show("_speed", _speed);
	Editor::Show("_lerp_position", _lerp_position);
}

void Hachiko::Scripting::MainMenuManager::OnEditor()
{
	Editor::Show("_state_changed", _state_changed);
	Editor::Show("_main_background", _main_background);
	Editor::Show("_button_background", _button_background);
	Editor::Show<ComponentButton>("_button_play", "ComponentButton*", _button_play);
	Editor::Show<ComponentButton>("_button_quit", "ComponentButton*", _button_quit);
	Editor::Show<ComponentButton>("_button_settings", "ComponentButton*", _button_settings);
	Editor::Show<ComponentButton>("_button_credits", "ComponentButton*", _button_credits);
	Editor::Show("_settings", _settings);
	Editor::Show("_credits", _credits);
	Editor::Show<ComponentButton>("_button_back", "ComponentButton*", _button_back);
}

void Hachiko::Scripting::PlayerCamera::OnEditor()
{
	Editor::Show("_relative_position_to_player", _relative_position_to_player);
	Editor::Show("_player", _player);
	Editor::Show("_follow_delay", _follow_delay);
}

void Hachiko::Scripting::PlayerController::OnEditor()
{
	Editor::Show("_movement_speed", _movement_speed);
	Editor::Show("_dash_indicator", _dash_indicator);
	Editor::Show("_dash_duration", _dash_duration);
	Editor::Show("_dash_distance", _dash_distance);
	Editor::Show("_dash_progress", _dash_progress);
	Editor::Show("_dash_cooldown", _dash_cooldown);
	Editor::Show("_dash_timer", _dash_timer);
	Editor::Show("_dash_count", _dash_count);
	Editor::Show("_max_dash_count", _max_dash_count);
	Editor::Show("_is_dashing", _is_dashing);
	Editor::Show("_has_cooldown", _has_cooldown);
	Editor::Show("_dash_start", _dash_start);
	Editor::Show("_dash_direction", _dash_direction);
	Editor::Show("_should_rotate", _should_rotate);
	Editor::Show("_rotation_progress", _rotation_progress);
	Editor::Show("_rotation_duration", _rotation_duration);
}