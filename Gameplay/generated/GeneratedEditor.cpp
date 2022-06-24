#include "scriptingUtil/gameplaypch.h"
#include "entities/Stats.h"
#include "entities/crystals/CrystalExplosion.h"
#include "entities/enemies/BugAnimationManager.h"
#include "entities/enemies/EnemyController.h"
#include "entities/player/CombatManager.h"
#include "entities/player/PlayerAnimationManager.h"
#include "entities/player/PlayerCamera.h"
#include "entities/player/PlayerController.h"
#include "entities/player/PlayerSoundManager.h"
#include "entities/player/RoomTeleporter.h"
#include "misc/AudioManager.h"
#include "misc/DynamicCamera.h"
#include "misc/FancyLights.h"
#include "ui/BackToMainMenu.h"
#include "ui/DebugManager.h"
#include "ui/MainMenuManager.h"


void Hachiko::Scripting::Stats::OnEditor()
{
	Editor::Show("Attack Power", _attack_power);
	Editor::Show("Attack Cd", _attack_cd);
	Editor::Show("Attack Range", _attack_range);
	Editor::Show("Move Speed", _move_speed);
	Editor::Show("Max Hp", _max_hp);
}

void Hachiko::Scripting::CrystalExplosion::OnEditor()
{
	Editor::Show("Player", _player);
	Editor::Show("Explosion Crystal", _explosion_crystal);
	Editor::Show("Static Crystal", _static_crystal);
	Editor::Show("Outer Explosion Indicator", _outer_explosion_indicator);
	Editor::Show("Inner Explosion Indicator", _inner_explosion_indicator);
	Editor::Show("Crashing Index", _crashing_index);
	Editor::Show("Detecting Radius", _detecting_radius);
	Editor::Show("Explosion Radius", _explosion_radius);
	Editor::Show("Timer Explosion", _timer_explosion);
	Editor::Show("Explosive Crystal", _explosive_crystal);
}

void Hachiko::Scripting::BugAnimationManager::OnEditor()
{
	Editor::Show<ComponentAnimation>("Animator", "ComponentAnimation*", _animator);
	Editor::Show("State String", _state_string);
	Editor::Show("Idle Index", _idle_index);
	Editor::Show("Attacking Index", _attacking_index);
}

void Hachiko::Scripting::EnemyController::OnEditor()
{
	Editor::Show("Aggro Range", _aggro_range);
	Editor::Show("Attack Range", _attack_range);
	Editor::Show("Spawn Pos", _spawn_pos);
	Editor::Show("Spawn Is Initial", _spawn_is_initial);
	Editor::Show("Player", _player);
	Editor::Show("Enemy Body", _enemy_body);
	Editor::Show("Parasite", _parasite);
	Editor::Show("Audio Manager Game Object", _audio_manager_game_object);
	Editor::Show("Already In Combat", _already_in_combat);
	Editor::Show("Attack Animation Duration", _attack_animation_duration);
	Editor::Show("Attack Animation Timer", _attack_animation_timer);
	Editor::Show("Is Ranged Attack", _is_ranged_attack);
}

void Hachiko::Scripting::CombatManager::OnEditor()
{
}

void Hachiko::Scripting::PlayerAnimationManager::OnEditor()
{
	Editor::Show<ComponentAnimation>("Animator", "ComponentAnimation*", _animator);
	Editor::Show("State String", _state_string);
	Editor::Show("Idle Index", _idle_index);
	Editor::Show("Walking Index", _walking_index);
	Editor::Show("Dashing Index", _dashing_index);
	Editor::Show("Melee Index", _melee_index);
	Editor::Show("Ranged Index", _ranged_index);
}

void Hachiko::Scripting::PlayerCamera::OnEditor()
{
	Editor::Show("Relative Position To Player", _relative_position_to_player);
	Editor::Show("Player", _player);
	Editor::Show("Follow Delay", _follow_delay);
}

void Hachiko::Scripting::PlayerController::OnEditor()
{
	Editor::Show("Attack Indicator", _attack_indicator);
	Editor::Show("Bullet Emitter", _bullet_emitter);
	Editor::Show("Goal", _goal);
	Editor::Show("Dash Duration", _dash_duration);
	Editor::Show("Dash Distance", _dash_distance);
	Editor::Show("Dash Cooldown", _dash_cooldown);
	Editor::Show("Max Dash Charges", _max_dash_charges);
	Editor::Show("Rotation Duration", _rotation_duration);
	Editor::Show("Hp Cell 1", _hp_cell_1);
	Editor::Show("Hp Cell 2", _hp_cell_2);
	Editor::Show("Hp Cell 3", _hp_cell_3);
	Editor::Show("Hp Cell 4", _hp_cell_4);
	Editor::Show("Camera", _camera);
	Editor::Show("Ui Damage", _ui_damage);
}

void Hachiko::Scripting::PlayerSoundManager::OnEditor()
{
	Editor::Show<ComponentAudioSource>("Audio Source", "ComponentAudioSource*", _audio_source);
	Editor::Show("Step Frequency", _step_frequency);
	Editor::Show("Melee Frequency", _melee_frequency);
	Editor::Show("Ranged Frequency", _ranged_frequency);
	Editor::Show("Timer", _timer);
}

void Hachiko::Scripting::RoomTeleporter::OnEditor()
{
	Editor::Show("Player", _player);
	Editor::Show("Room Portal", _room_portal);
	Editor::Show("Outdoor Portal", _outdoor_portal);
	Editor::Show("Trigger Distance", _trigger_distance);
	Editor::Show<ComponentImage>("Fade Image", "ComponentImage*", _fade_image);
	Editor::Show("Fade Duration", _fade_duration);
	Editor::Show("Blackout Duration", _blackout_duration);
}

void Hachiko::Scripting::AudioManager::OnEditor()
{
	Editor::Show("Enemies In Combat", _enemies_in_combat);
	Editor::Show("Previous In Combat", _previous_in_combat);
}

void Hachiko::Scripting::DynamicCamera::OnEditor()
{
	Editor::Show("Start Point", _start_point);
	Editor::Show("End Point", _end_point);
	Editor::Show("Speed", _speed);
	Editor::Show("Lerp Position", _lerp_position);
}

void Hachiko::Scripting::FancyLights::OnEditor()
{
	Editor::Show("Rotate On Y", _rotate_on_y);
	Editor::Show("Angle", _angle);
}

void Hachiko::Scripting::BackToMainMenu::OnEditor()
{
	Editor::Show<ComponentButton>("Button Back", "ComponentButton*", _button_back);
}

void Hachiko::Scripting::DebugManager::OnEditor()
{
	Editor::Show("Player", _player);
	Editor::Show<ComponentButton>("Button Back", "ComponentButton*", _button_back);
	Editor::Show<ComponentButton>("Teleport Next Pos", "ComponentButton*", _teleport_next_pos);
	Editor::Show<ComponentButton>("Teleport Prev Pos", "ComponentButton*", _teleport_prev_pos);
	Editor::Show<ComponentButton>("Teleport Add Pos", "ComponentButton*", _teleport_add_pos);
	Editor::Show<ComponentButton>("Add Health", "ComponentButton*", _add_health);
	Editor::Show<ComponentButton>("Remove Health", "ComponentButton*", _remove_health);
	Editor::Show<ComponentButton>("Increase Max Hp", "ComponentButton*", _increase_max_hp);
	Editor::Show<ComponentButton>("Decrease Max Hp", "ComponentButton*", _decrease_max_hp);
	Editor::Show<ComponentButton>("Increase Move Speed", "ComponentButton*", _increase_move_speed);
	Editor::Show<ComponentButton>("Decrease Move Speed", "ComponentButton*", _decrease_move_speed);
	Editor::Show<ComponentButton>("Increase Attack Cd", "ComponentButton*", _increase_attack_cd);
	Editor::Show<ComponentButton>("Decrease Attack Cd", "ComponentButton*", _decrease_attack_cd);
	Editor::Show<ComponentButton>("Increase Attack Power", "ComponentButton*", _increase_attack_power);
	Editor::Show<ComponentButton>("Decrease Attack Power", "ComponentButton*", _decrease_attack_power);
	Editor::Show<ComponentButton>("God Mode", "ComponentButton*", _god_mode);
	Editor::Show<ComponentButton>("Spawn Enemy", "ComponentButton*", _spawn_enemy);
	Editor::Show<ComponentButton>("Unlock Skills", "ComponentButton*", _unlock_skills);
	Editor::Show<ComponentButton>("Toggle Performance Output", "ComponentButton*", _toggle_performance_output);
	Editor::Show<ComponentButton>("Toggle Vsync", "ComponentButton*", _toggle_vsync);
	Editor::Show<ComponentButton>("Toggle Wireframe", "ComponentButton*", _toggle_wireframe);
	Editor::Show<ComponentButton>("Toggle Show Colliders", "ComponentButton*", _toggle_show_colliders);
	Editor::Show<ComponentButton>("Exit Debug", "ComponentButton*", _exit_debug);
	Editor::Show<ComponentText>("Text Fps", "ComponentText*", _text_fps);
	Editor::Show<ComponentText>("Text Ms", "ComponentText*", _text_ms);
	Editor::Show("Tp Pos 1", _tp_pos1);
	Editor::Show("Tp Pos 2", _tp_pos2);
	Editor::Show("Tp Pos 3", _tp_pos3);
	Editor::Show("Performance Menu", _performance_menu);
}

void Hachiko::Scripting::MainMenuManager::OnEditor()
{
	Editor::Show("State Changed", _state_changed);
	Editor::Show("Main Background", _main_background);
	Editor::Show("Button Background", _button_background);
	Editor::Show<ComponentButton>("Button Play", "ComponentButton*", _button_play);
	Editor::Show<ComponentButton>("Button Quit", "ComponentButton*", _button_quit);
	Editor::Show<ComponentButton>("Button Settings", "ComponentButton*", _button_settings);
	Editor::Show<ComponentButton>("Button Credits", "ComponentButton*", _button_credits);
	Editor::Show("Settings", _settings);
	Editor::Show("Credits", _credits);
	Editor::Show<ComponentButton>("Button Back", "ComponentButton*", _button_back);
}