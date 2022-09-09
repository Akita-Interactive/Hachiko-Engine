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


void Hachiko::Scripting::TriggerAnim::OnEditor()
{
}

void Hachiko::Scripting::Stats::OnEditor()
{
	Editor::Show("Attack Power", _attack_power);
	Editor::Show("Attack Cd", _attack_cd);
	Editor::Show("Attack Range", _attack_range);
	Editor::Show("Move Speed", _move_speed);
	Editor::Show("Max Hp", _max_hp);
	Editor::Show("Current Hp", _current_hp);
}

void Hachiko::Scripting::CrystalExplosion::OnEditor()
{
	Editor::Show("Explosion Crystal", _explosion_crystal);
	Editor::Show("Static Crystal", _static_crystal);
	Editor::Show("Explosion Indicator Helper", _explosion_indicator_helper);
	Editor::Show("Explosion Effect", _explosion_effect);
	Editor::Show("Crashing Index", _crashing_index);
	Editor::Show("Detecting Radius", _detecting_radius);
	Editor::Show("Explosion Radius", _explosion_radius);
	Editor::Show("Timer Explosion", _timer_explosion);
	Editor::Show("Explosive Crystal", _explosive_crystal);
	Editor::Show("Regen Time", _regen_time);
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
	Editor::Show("Worm", _worm);
	Editor::Show("Aggro Range", _aggro_range);
	Editor::Show("Attack Range", _attack_range);
	Editor::Show("Attack Delay", _attack_delay);
	Editor::Show("Idle Cooldown", _idle_cooldown);
	Editor::Show("Spawning Time", _spawning_time);
	Editor::Show("Spawn Pos", _spawn_pos);
	Editor::Show("Spawn Is Initial", _spawn_is_initial);
	Editor::Show("Enemy Body", _enemy_body);
	Editor::Show("Parasite", _parasite);
	Editor::Show("Blood Trail", _blood_trail);
	Editor::Show("Small Dust", _small_dust);
	Editor::Show("Big Dust", _big_dust);
	Editor::Show("Attack Zone", _attack_zone);
	Editor::Show("Inner Indicator", _inner_indicator);
	Editor::Show("Outer Indicator", _outer_indicator);
	Editor::Show("Already In Combat", _already_in_combat);
	Editor::Show("Is From Gautlet", _is_from_gautlet);
	Editor::Show("Will Die", _will_die);
	Editor::Show("Attack Animation Duration", _attack_animation_duration);
	Editor::Show("Attack Animation Timer", _attack_animation_timer);
	Editor::Show("Current Spawning Time", _current_spawning_time);
}

void Hachiko::Scripting::CombatManager::OnEditor()
{
	Editor::Show("Charge Vfx", _charge_vfx);
	Editor::Show("Shot Vfx", _shot_vfx);
}

void Hachiko::Scripting::CombatVisualEffectsPool::OnEditor()
{
	Editor::Show("Current Billboard Index", _current_billboard_index);
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
	Editor::Show("Objective", _objective);
	Editor::Show("Follow Delay", _follow_delay);
}

void Hachiko::Scripting::PlayerController::OnEditor()
{
	Editor::Show("Sword Weapon", _sword_weapon);
	Editor::Show("Sword Upper", _sword_upper);
	Editor::Show("Claw Weapon", _claw_weapon);
	Editor::Show("Attack Indicator", _attack_indicator);
	Editor::Show("Bullet Emitter", _bullet_emitter);
	Editor::Show("Goal", _goal);
	Editor::Show("Player Geometry", _player_geometry);
	Editor::Show("Dash Duration", _dash_duration);
	Editor::Show("Dash Distance", _dash_distance);
	Editor::Show("Dash Cooldown", _dash_cooldown);
	Editor::Show("Invulnerability Time", _invulnerability_time);
	Editor::Show("Dash Scaler", _dash_scaler);
	Editor::Show("Max Dash Charges", _max_dash_charges);
	Editor::Show("Dash Trail", _dash_trail);
	Editor::Show("Trail Enlarger", _trail_enlarger);
	Editor::Show("Falling Dust", _falling_dust);
	Editor::Show("Walking Dust", _walking_dust);
	Editor::Show("Heal Effect", _heal_effect);
	Editor::Show("Rotation Duration", _rotation_duration);
	Editor::Show("Hp Cell 1", _hp_cell_1);
	Editor::Show("Hp Cell 2", _hp_cell_2);
	Editor::Show("Hp Cell 3", _hp_cell_3);
	Editor::Show("Hp Cell 4", _hp_cell_4);
	Editor::Show("Ammo Cell 1", _ammo_cell_1);
	Editor::Show("Ammo Cell 2", _ammo_cell_2);
	Editor::Show("Ammo Cell 3", _ammo_cell_3);
	Editor::Show("Ammo Cell 4", _ammo_cell_4);
	Editor::Show("Weapon Charge Bar Go", _weapon_charge_bar_go);
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

void Hachiko::Scripting::BlinkingLight::OnEditor()
{
	Editor::Show("Use Perlin Noise", _use_perlin_noise);
	Editor::Show("Min Radius", _min_radius);
	Editor::Show("Max Radius", _max_radius);
	Editor::Show("Radius Amplitude", _radius_amplitude);
	Editor::Show("Min Duration", _min_duration);
	Editor::Show("Max Duration", _max_duration);
	Editor::Show("Duration Amplitude", _duration_amplitude);
	Editor::Show("Min Intensity", _min_intensity);
	Editor::Show("Max Intensity", _max_intensity);
	Editor::Show("Intensity Amplitude", _intensity_amplitude);
	Editor::Show("Starting Intensity", _starting_intensity);
	Editor::Show("Starting Radius", _starting_radius);
	Editor::Show("Current Intensity", _current_intensity);
	Editor::Show("Current Duration", _current_duration);
	Editor::Show("Current Radius", _current_radius);
	Editor::Show("Next Duration", _next_duration);
	Editor::Show("Next Intensity", _next_intensity);
	Editor::Show("Next Radius", _next_radius);
}

void Hachiko::Scripting::CameraPosChange::OnEditor()
{
	Editor::Show("Objective", _objective);
	Editor::Show("Do Look Ahead", _do_look_ahead);
	Editor::Show("Speed", _speed);
	Editor::Show("Duration", _duration);
	Editor::Show("Relative Position", _relative_position);
	Editor::Show("Rotation", _rotation);
}

void Hachiko::Scripting::CrystalPlatform::OnEditor()
{
	Editor::Show("Seconds Before Shaking", _seconds_before_shaking);
	Editor::Show("Seconds Shaking", _seconds_shaking);
	Editor::Show("Seconds To Regen", _seconds_to_regen);
	Editor::Show("Shake Intensity", _shake_intensity);
	Editor::Show("Invisible Obstacle", _invisible_obstacle);
	Editor::Show("Crystal Platform", _crystal_platform);
}

void Hachiko::Scripting::DoorController::OnEditor()
{
	Editor::Show("Door Prefab", _door_prefab);
}

void Hachiko::Scripting::DynamicCamera::OnEditor()
{
	Editor::Show("Speed", _speed);
}

void Hachiko::Scripting::FancyLights::OnEditor()
{
	Editor::Show("Rotate On Y", _rotate_on_y);
	Editor::Show("Angle", _angle);
}

void Hachiko::Scripting::GauntletManager::OnEditor()
{
	Editor::Show("Combat Manager Go", _combat_manager_go);
	Editor::Show("Trigger Radius", _trigger_radius);
	Editor::Show("Round Wait Time", _round_wait_time);
	Editor::Show("Complete Wait Time", _complete_wait_time);
	Editor::Show("Door Controller 1", _door_controller_1);
	Editor::Show("Door Controller 2", _door_controller_2);
	Editor::Show("Pack 1", _pack_1);
	Editor::Show("Pack 2", _pack_2);
	Editor::Show("Pack 3", _pack_3);
}

void Hachiko::Scripting::LaserController::OnEditor()
{
	Editor::Show("Laser", _laser);
	Editor::Show("Max Length", _max_length);
	Editor::Show("Max Scale", _max_scale);
	Editor::Show("Activation Time", _activation_time);
	Editor::Show("Damage", _damage);
	Editor::Show("Spin Movement", _spin_movement);
	Editor::Show("Spin Speed", _spin_speed);
	Editor::Show("Horizonal Movement", _horizonal_movement);
	Editor::Show("Movement Speed", _movement_speed);
	Editor::Show("Movement Target", _movement_target);
	Editor::Show("Toggle Activation", _toggle_activation);
	Editor::Show("Toggle Active Time", _toggle_active_time);
	Editor::Show("Toggle Inactive Time", _toggle_inactive_time);
}

void Hachiko::Scripting::LevelManager::OnEditor()
{
	Editor::Show("Respawn Position", _respawn_position);
	Editor::Show("Gauntlet Ui Go", _gauntlet_ui_go);
	Editor::Show("Gauntlet Counter Go", _gauntlet_counter_go);
}

void Hachiko::Scripting::PillarCheckpoint::OnEditor()
{
	Editor::Show("Respawn Go", _respawn_go);
	Editor::Show("Activation Range", _activation_range);
}

void Hachiko::Scripting::Spawner::OnEditor()
{
	Editor::Show("Enemy Pack", _enemy_pack);
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
	Editor::Show("Game Title", _game_title);
	Editor::Show("Settings", _settings);
	Editor::Show("Credits", _credits);
	Editor::Show<ComponentButton>("Button Back", "ComponentButton*", _button_back);
}