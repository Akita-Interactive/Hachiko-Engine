#include "scriptingUtil/gameplaypch.h"
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


void Hachiko::Scripting::CutsceneManager::OnEditor()
{
	Editor::Show("Cutscene", _cutscene);
	Editor::Show("Next Level", _next_level);
}

void Hachiko::Scripting::PauseMenu::OnEditor()
{
	Editor::Show("Pause Ui", _pause_ui);
	Editor::Show("Player Hud", _player_hud);
	Editor::Show<ComponentButton>("Resume Button", "ComponentButton*", _resume_button);
	Editor::Show<ComponentButton>("Quit Button", "ComponentButton*", _quit_button);
}

void Hachiko::Scripting::TriggerAnim::OnEditor()
{
}

void Hachiko::Scripting::BloomAnimator::OnEditor()
{
	Editor::Show("Bloom Target", _bloom_target);
	Editor::Show("Is Automatic", _is_automatic);
	Editor::Show("Is Randomized", _is_randomized);
	Editor::Show("Randomized Duration Min", _randomized_duration_min);
	Editor::Show("Randomized Duration Max", _randomized_duration_max);
	Editor::Show("Automatic Lerp Duration", _automatic_lerp_duration);
	Editor::Show("Initial Emissive Color", _initial_emissive_color);
}

void Hachiko::Scripting::BounceEffect::OnEditor()
{
	Editor::Show("Altitude", _altitude);
	Editor::Show("Magnitude", _magnitude);
	Editor::Show("Speed", _speed);
}

void Hachiko::Scripting::HoverEffect::OnEditor()
{
	Editor::Show("Altitude", _altitude);
	Editor::Show("Magnitude", _magnitude);
	Editor::Show("Speed", _speed);
}

void Hachiko::Scripting::RotationEffect::OnEditor()
{
	Editor::Show("Rotate X", _rotate_x);
	Editor::Show("Rotate Y", _rotate_y);
	Editor::Show("Rotate Z", _rotate_z);
	Editor::Show("Rotation Speed", _rotation_speed);
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
	Editor::Show("Explosion Indicator Helper", _explosion_indicator_helper);
	Editor::Show("Explosion Effect", _explosion_effect);
	Editor::Show("Shake Intensity", _shake_intensity);
	Editor::Show("Seconds Shaking", _seconds_shaking);
	Editor::Show("Crashing Index", _crashing_index);
	Editor::Show("Detecting Radius", _detecting_radius);
	Editor::Show("Explosion Radius", _explosion_radius);
	Editor::Show("Timer Explosion", _timer_explosion);
	Editor::Show("Explosive Crystal", _explosive_crystal);
	Editor::Show("Regen Time", _regen_time);
	Editor::Show("Should Regen", _should_regen);
	Editor::Show("Damage Effect Duration", damage_effect_duration);
}

void Hachiko::Scripting::Stalagmite::OnEditor()
{
	Editor::Show("Explosion Effect", _explosion_effect);
	Editor::Show("G E O", GEO);
	Editor::Show("Obstacle", _obstacle);
}

void Hachiko::Scripting::BossController::OnEditor()
{
	Editor::Show("State Value", state_value);
	Editor::Show("Combat State Value", combat_state_value);
	Editor::Show("Second Phase", second_phase);
	Editor::Show("Hp Bar Go", hp_bar_go);
	Editor::Show("Crystal Target Go", crystal_target_go);
	Editor::Show("Cocoons Parent", cocoons_parent);
	Editor::Show("Gauntlet Go", gauntlet_go);
	Editor::Show("Current Index Crystals", _current_index_crystals);
	Editor::Show("Crystal Pool", crystal_pool);
	Editor::Show("Attack Delay", attack_delay);
	Editor::Show("After Attack Wait", after_attack_wait);
	Editor::Show("Camera Transition Speed", camera_transition_speed);
	Editor::Show("Encounter Start Duration", encounter_start_duration);
	Editor::Show("Pre Combat Camera Offset", pre_combat_camera_offset);
	Editor::Show("Jump Start Delay", _jump_start_delay);
	Editor::Show("Jump Ascending Duration", _jump_ascending_duration);
	Editor::Show("Jump Post Ascending Delay", _jump_post_ascending_delay);
	Editor::Show("Jump On Highest Point Duration", _jump_on_highest_point_duration);
	Editor::Show("Jump Post On Highest Point Delay", _jump_post_on_highest_point_delay);
	Editor::Show("Jump Descending Duration", _jump_descending_duration);
	Editor::Show("Jump Post Descending Delay", _jump_post_descending_delay);
	Editor::Show("Jump Getting Up Duration", _jump_getting_up_duration);
	Editor::Show("Jump Skill Delay", _jump_skill_delay);
	Editor::Show("Jump Skill Duration", _jump_skill_duration);
	Editor::Show("Jump Post Skill Delay", _jump_post_skill_delay);
	Editor::Show("Jump Height", _jump_height);
	Editor::Show("Jump Offset", _jump_offset);
	Editor::Show("Damage Aoe", _damage_AOE);
	Editor::Show("Crystal Jump Position", _crystal_jump_position);
	Editor::Show("Stalagmite Manager Go", _stalagmite_manager_go);
	Editor::Show<ComponentText>("Boss State Text Ui", "ComponentText*", _boss_state_text_ui);
	Editor::Show("Time Between Enemies", time_between_enemies);
	Editor::Show("Enemy Pool", enemy_pool);
	Editor::Show("Damage Effect Duration", damage_effect_duration);
	Editor::Show("Chasing Time Limit", chasing_time_limit);
	Editor::Show("Rotating Lasers", _rotating_lasers);
	Editor::Show("Rotating Lasers Speed", _rotating_lasers_speed);
	Editor::Show("Laser Wall", _laser_wall);
	Editor::Show("Laser Wall Duration", _laser_wall_duration);
	Editor::Show("Laser Jump Height", _laser_jump_height);
	Editor::Show("Melee Trail Right", _melee_trail_right);
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
	Editor::Show("State Num", _state_num);
	Editor::Show("Worm", _worm);
	Editor::Show("Aggro Range", _aggro_range);
	Editor::Show("Attack Range", _attack_range);
	Editor::Show("Attack Delay", _attack_delay);
	Editor::Show("Idle Cooldown", _idle_cooldown);
	Editor::Show("Patrol Cooldown", _patrol_cooldown);
	Editor::Show("Spawning Time", _spawning_time);
	Editor::Show("Chase Cooldown", _chase_cooldown);
	Editor::Show("Spawn Pos", _spawn_pos);
	Editor::Show("Enemy Body", _enemy_body);
	Editor::Show("Parasite", _parasite);
	Editor::Show("Small Dust", _small_dust);
	Editor::Show("Big Dust", _big_dust);
	Editor::Show("Attack Zone", _attack_zone);
	Editor::Show("Inner Indicator", _inner_indicator);
	Editor::Show("Outer Indicator", _outer_indicator);
	Editor::Show("Projectile Particles", _projectile_particles);
	Editor::Show("Explosion Particles", _explosion_particles);
	Editor::Show("Already In Combat", _already_in_combat);
	Editor::Show("Is From Gautlet", _is_from_gautlet);
	Editor::Show("Is From Boss", _is_from_boss);
	Editor::Show("Will Die", _will_die);
	Editor::Show("Attack Animation Duration", _attack_animation_duration);
	Editor::Show("Attack Animation Timer", _attack_animation_timer);
	Editor::Show("Damage Effect Duration", damage_effect_duration);
}

void Hachiko::Scripting::CombatManager::OnEditor()
{
	Editor::Show("Charge Vfx", _charge_vfx);
	Editor::Show("Shot Vfx", _shot_vfx);
}

void Hachiko::Scripting::CombatVisualEffectsPool::OnEditor()
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
	Editor::Show("Objective", _objective);
	Editor::Show("Follow Delay", _follow_delay);
}

void Hachiko::Scripting::PlayerController::OnEditor()
{
	Editor::Show("Sword Weapon", _sword_weapon);
	Editor::Show("Sword Upper", _sword_upper);
	Editor::Show("Claw Weapon", _claw_weapon);
	Editor::Show("Hammer Weapon", _hammer_weapon);
	Editor::Show("Attack Indicator", _attack_indicator);
	Editor::Show("Aim Indicator", _aim_indicator);
	Editor::Show("Combat Manager", _combat_manager);
	Editor::Show("Goal", _goal);
	Editor::Show("Player Geometry", _player_geometry);
	Editor::Show("Dash Duration", _dash_duration);
	Editor::Show("Dash Distance", _dash_distance);
	Editor::Show("Dash Cooldown", _dash_cooldown);
	Editor::Show("Invulnerability Time", _invulnerability_time);
	Editor::Show("Dash Scaler", _dash_scaler);
	Editor::Show("Max Dash Charges", _max_dash_charges);
	Editor::Show("Dash Trail", _dash_trail);
	Editor::Show("Dash Trail Vfx", _dash_trail_vfx);
	Editor::Show("Trail Enlarger", _trail_enlarger);
	Editor::Show("Falling Dust", _falling_dust);
	Editor::Show("Walking Dust", _walking_dust);
	Editor::Show("Heal Effect", _heal_effect);
	Editor::Show("Damage Effect", _damage_effect);
	Editor::Show("Parasite Pickup Effect", _parasite_pickup_effect);
	Editor::Show("Melee Trail Right", _melee_trail_right);
	Editor::Show("Melee Trail Left", _melee_trail_left);
	Editor::Show("Melee Trail Center", _melee_trail_center);
	Editor::Show("Claws Trail Right", _claws_trail_right);
	Editor::Show("Claws Trail Left", _claws_trail_left);
	Editor::Show("Claws Trail Center", _claws_trail_center);
	Editor::Show("Sword Trail Right", _sword_trail_right);
	Editor::Show("Sword Trail Left", _sword_trail_left);
	Editor::Show("Sword Trail Center", _sword_trail_center);
	Editor::Show("Hammer Trail Right", _hammer_trail_right);
	Editor::Show("Hammer Trail Left", _hammer_trail_left);
	Editor::Show("Hammer Trail Center", _hammer_trail_center);
	Editor::Show("Rotation Duration", _rotation_duration);
	Editor::Show("Death Screen", _death_screen);
	Editor::Show("Hp Cell 1", _hp_cell_1);
	Editor::Show("Hp Cell 2", _hp_cell_2);
	Editor::Show("Hp Cell 3", _hp_cell_3);
	Editor::Show("Hp Cell 4", _hp_cell_4);
	Editor::Show("Hp Cell Extra", _hp_cell_extra);
	Editor::Show("Magic Parasyte", _magic_parasyte);
	Editor::Show("Ammo Cell 1", _ammo_cell_1);
	Editor::Show("Ammo Cell 2", _ammo_cell_2);
	Editor::Show("Ammo Cell 3", _ammo_cell_3);
	Editor::Show("Ammo Cell 4", _ammo_cell_4);
	Editor::Show("Sword Ui Addon", _sword_ui_addon);
	Editor::Show("Maze Ui Addon", _maze_ui_addon);
	Editor::Show("Claw Ui Addon", _claw_ui_addon);
	Editor::Show("Weapon Charge Bar Go", _weapon_charge_bar_go);
	Editor::Show("Keyboard Tooltip Display", _keyboard_tooltip_display);
	Editor::Show("Controller Tooltip Display", _controller_tooltip_display);
	Editor::Show("Tooltip Y Offset", tooltip_y_offset);
	Editor::Show("Camera", _camera);
	Editor::Show("Ui Damage", _ui_damage);
	Editor::Show("Heal Effect Fade Duration", _heal_effect_fade_duration);
	Editor::Show("Damage Effect Duration", damage_effect_duration);
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

void Hachiko::Scripting::AssetsObstacle::OnEditor()
{
}

void Hachiko::Scripting::AudioManager::OnEditor()
{
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

void Hachiko::Scripting::BossLaserController::OnEditor()
{
	Editor::Show("Laser", _laser);
	Editor::Show("Max Length", _max_length);
	Editor::Show("Max Scale", _max_scale);
	Editor::Show("Activation Time", _activation_time);
	Editor::Show("Damage", _damage);
	Editor::Show("Track If Active", _track_if_active);
	Editor::Show("Tracking Speed", _tracking_speed);
	Editor::Show("Toggle Activation", _toggle_activation);
	Editor::Show("Toggle Active Time", _toggle_active_time);
	Editor::Show("Toggle Inactive Time", _toggle_inactive_time);
}

void Hachiko::Scripting::CameraPosChange::OnEditor()
{
	Editor::Show("Objective", _objective);
	Editor::Show("Do Look Ahead", _do_look_ahead);
	Editor::Show("Do Mouse Movement", _do_mouse_movement);
	Editor::Show("Speed", _speed);
	Editor::Show("Duration", _duration);
	Editor::Show("Relative Position", _relative_position);
	Editor::Show("Rotation", _rotation);
	Editor::Show("Check Box", _check_box);
}

void Hachiko::Scripting::Centipedes::OnEditor()
{
	Editor::Show("Damage Effect", _damage_effect);
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
	Editor::Show("Enable After", _enable_after);
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
	Editor::Show("Closing Door", _closing_door);
	Editor::Show("Pack 1", _pack_1);
	Editor::Show("Pack 2", _pack_2);
	Editor::Show("Pack 3", _pack_3);
	Editor::Show("Camera Anchor", _camera_anchor);
	Editor::Show("Central Anchor", _central_anchor);
	Editor::Show("Relative Position", _relative_position);
	Editor::Show("Camera Follows Player", _camera_follows_player);
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
	Editor::Show("Dissolving Time", _dissolving_time);
	Editor::Show("Sparks", _sparks);
}

void Hachiko::Scripting::LevelManager::OnEditor()
{
	Editor::Show("Level", _level);
	Editor::Show("Respawn Position", _respawn_position);
	Editor::Show("Gauntlet Ui Go", _gauntlet_ui_go);
	Editor::Show("Gauntlet Counter Go", _gauntlet_counter_go);
	Editor::Show("Gauntlets Easy Mode", _gauntlets_easy_mode);
	Editor::Show("Modify Fog", _modify_fog);
	Editor::Show("Fog Frequency", _fog_frequency);
	Editor::Show("Fog Max Density", _fog_max_density);
	Editor::Show("Fog Min Density", _fog_min_density);
	Editor::Show("Audio Manager Go", _audio_manager_go);
	Editor::Show("Player Sound Manager Go", _player_sound_manager_go);
	Editor::Show("Victory Screen", _victory_screen);
}

void Hachiko::Scripting::PillarCheckpoint::OnEditor()
{
	Editor::Show("Respawn Go", _respawn_go);
	Editor::Show("Activation Range", _activation_range);
	Editor::Show("Light Go", _light_go);
}

void Hachiko::Scripting::Spawner::OnEditor()
{
	Editor::Show("Enemy Pack", _enemy_pack);
}

void Hachiko::Scripting::StalagmiteManager::OnEditor()
{
	Editor::Show("Falling Cooldown", _falling_cooldown);
}

void Hachiko::Scripting::TimeManager::OnEditor()
{
}

void Hachiko::Scripting::Tutorial::OnEditor()
{
	Editor::Show("Tutorial Keyboard Movement", _tutorial_keyboard_movement);
	Editor::Show("Tutorial Controller Movement", _tutorial_controller_movement);
	Editor::Show("Tutorial Keyboard Attacks", _tutorial_keyboard_attacks);
	Editor::Show("Tutorial Controller Attacks", _tutorial_controller_attacks);
	Editor::Show("Tutorial Keyboard Pickup", _tutorial_keyboard_pickup);
	Editor::Show("Tutorial Controller Pickup", _tutorial_controller_pickup);
	Editor::Show("First Enemy", _first_enemy);
	Editor::Show("Tutorials Screen Shown", _tutorials_screen_shown);
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
	Editor::Show<ComponentButton>("Increase Move Speed", "ComponentButton*", _increase_move_speed);
	Editor::Show<ComponentButton>("Decrease Move Speed", "ComponentButton*", _decrease_move_speed);
	Editor::Show<ComponentButton>("Increase Attack Power", "ComponentButton*", _increase_attack_power);
	Editor::Show<ComponentButton>("Decrease Attack Power", "ComponentButton*", _decrease_attack_power);
	Editor::Show<ComponentButton>("God Mode", "ComponentButton*", _god_mode);
	Editor::Show<ComponentButton>("Spawn Enemy", "ComponentButton*", _spawn_enemy);
	Editor::Show<ComponentButton>("Weapon Claws", "ComponentButton*", _weapon_claws);
	Editor::Show<ComponentButton>("Weapon Sword", "ComponentButton*", _weapon_sword);
	Editor::Show<ComponentButton>("Weapon Hammer", "ComponentButton*", _weapon_hammer);
	Editor::Show<ComponentButton>("Reload Ammo", "ComponentButton*", _reload_ammo);
	Editor::Show<ComponentButton>("Toggle Performance Output", "ComponentButton*", _toggle_performance_output);
	Editor::Show<ComponentButton>("Toggle Vsync", "ComponentButton*", _toggle_vsync);
	Editor::Show<ComponentButton>("Toggle Wireframe", "ComponentButton*", _toggle_wireframe);
	Editor::Show<ComponentButton>("Toggle Show Colliders", "ComponentButton*", _toggle_show_colliders);
	Editor::Show<ComponentButton>("Exit Debug", "ComponentButton*", _exit_debug);
	Editor::Show<ComponentText>("Text Fps", "ComponentText*", _text_fps);
	Editor::Show<ComponentText>("Text Ms", "ComponentText*", _text_ms);
	Editor::Show<ComponentText>("Text Atck", "ComponentText*", _text_atck);
	Editor::Show<ComponentText>("Text Mvspd", "ComponentText*", _text_mvspd);
	Editor::Show("Tp Pos 1", _tp_pos1);
	Editor::Show("Tp Pos 2", _tp_pos2);
	Editor::Show("Tp Pos 3", _tp_pos3);
	Editor::Show("Performance Menu", _performance_menu);
	Editor::Show("Player Stats", _player_stats);
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