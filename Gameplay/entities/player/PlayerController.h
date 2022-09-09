#pragma once

#include <scripting/Script.h>
#include "entities/Stats.h"
#include "entities/player/CombatManager.h"
#include "misc/LevelManager.h"
#include "Gameplay.h"

#include <queue>

namespace Hachiko
{
	class GameObject;
	class ComponentMeshRenderer;
	class ComponentProgressBar;
	namespace Scripting
	{
		class PlayerCamera;

		enum class PlayerState
		{
			INVALID,
			IDLE,
			WALKING,
			PICK_UP,
			DASHING,
			RANGED_ATTACKING,
			MELEE_ATTACKING,
			FALLING,
			STUNNED,
			DIE,
			READY_TO_RESPAWN
		};

		class PlayerController : public Script
		{
			SERIALIZATION_METHODS(false)

				friend class LevelManager;

			enum class WeaponUsed
			{
				RED = 0,
				GREEN,
				BLUE,
				SIZE
			};

			enum class AttackType
			{
				COMMON_1 = 0,
				COMMON_2,
				COMMON_3,
				QUICK_1,
				QUICK_2,
				QUICK_3,
				HEAVY_1,
				HEAVY_2,
				HEAVY_3
			};

			struct PlayerAttack
			{
				float hit_delay = 0.f;
				float duration = 0.f;
				float dash_distance = 0.f;
				float cooldown = 0.2f;
				CombatManager::AttackStats stats;
			};

			struct Weapon
			{
				std::string name = "Undefined Weapon";
				CombatManager::BulletStats bullet;
				// Here we define the combo of attacks		
				float4 color = float4::zero;
				bool unlimited = true;
				unsigned charges = 0;
				GameObject* weapon_go = nullptr;
				std::vector<PlayerAttack> attacks;
			};

		public:
			PlayerController(GameObject* game_object);
			~PlayerController() override = default;

			void OnAwake() override;
			void OnStart() override;
			void OnUpdate() override;

			PlayerState GetState() const;

			void CheckGoal(const float3& current_position);
			bool RegisterHit(int damage, float knockback = 0, math::float3 direction = float3::zero, bool force_dmg = false);
			void UpdateHealthBar();
			void UpdateAmmoUI();
			void UpdateWeaponChargeUI();
			void ToggleGodMode();

			bool IsAlive() const { return _combat_stats->_current_hp > 0; }
			int GetCurrentHp() const { return _combat_stats->_current_hp; }
			bool _isInDebug = false;

			int GetAttackIndex() const
			{
				return _attack_idx;
			}

			WeaponUsed GetCurrentWeaponType() const
			{
				return static_cast<WeaponUsed>(_current_weapon);
			}

		private:
			math::float3 GetRaycastPosition(
				const math::float3& current_position) const;
			float3 GetCorrectedPosition(const float3& target_pos) const;

			PlayerAttack GetAttackType(AttackType attack_type);

			void SpawnGameObject() const;

			// Status check
			bool IsAttacking() const;
			bool IsDashing() const;
			bool IsWalking() const;
			bool IsStunned() const;
			bool IsFalling() const;
			bool IsPickUp() const;
			bool IsDying() const;

			bool IsActionLocked() const;
			bool IsAttackOnCooldown() const;
			bool IsInComboWindow() const;

			Weapon& GetCurrentWeapon();
			const PlayerAttack& GetNextAttack();
			const PlayerAttack& GetCurrentAttack();

			// Input and status management
			void HandleInputAndStatus();
			void HandleInputBuffering();
			bool HasBufferedClick();
			float3 GetBufferedClick();
			void ResetClickBuffer();

			// Actions called by handle input
			void Dash();
			void CorrectDashDestination(const float3& dash_source, float3& dash_destination);
			void StoreDashOrigin(const float3& dash_origin);
			float3 GetLastValidDashOrigin();
			void MeleeAttack();
			void ChangeWeapon(unsigned weapon_idx);
			void RangedAttack();
			void CancelAttack();
			float4x4 GetMeleeAttackOrigin(float attack_range) const;

			bool GetTerrainCollision(const float3& start, const float3& end, float3& collision_point) const;

			// Player simulation
			void MovementController();
			void DashController();
			void DashChargesManager();
			void DashTrailManager(float dash_progress);
			void WalkingOrientationController();
			void AttackController();

			void PickupParasite(const math::float3& current_position);
			void RecieveKnockback(const math::float3 direction);

			void CheckState();
			void CheckComboAnimation();
			void ResetPlayer(float3 spawn_pos);

		public:
			SERIALIZE_FIELD(PlayerState, _state);
			PlayerState _previous_state = PlayerState::INVALID;
			float3 _initial_pos = float3::zero;

			SERIALIZE_FIELD(GameObject*, _sword_weapon);
			SERIALIZE_FIELD(GameObject*, _sword_upper);
			SERIALIZE_FIELD(GameObject*, _claw_weapon);

			Stats* _combat_stats;
			bool _god_mode = false;
			bool _god_mode_trigger = false;

		private:
			SERIALIZE_FIELD(GameObject*, _attack_indicator);
			SERIALIZE_FIELD(GameObject*, _bullet_emitter);
			SERIALIZE_FIELD(GameObject*, _goal);
			SERIALIZE_FIELD(GameObject*, _player_geometry);

			SERIALIZE_FIELD(float, _dash_duration);
			SERIALIZE_FIELD(float, _dash_distance);
			SERIALIZE_FIELD(float, _dash_cooldown);
			SERIALIZE_FIELD(float, _invulnerability_time);
			SERIALIZE_FIELD(unsigned, _dash_scaler);
			SERIALIZE_FIELD(unsigned, _max_dash_charges);
			SERIALIZE_FIELD(GameObject*, _dash_trail);
			SERIALIZE_FIELD(float, _trail_enlarger);
			SERIALIZE_FIELD(GameObject*, _falling_dust);
			SERIALIZE_FIELD(GameObject*, _walking_dust);
			SERIALIZE_FIELD(GameObject*, _heal_effect);

			const float _ranged_attack_cooldown = 0.2f;
			const float _combo_grace_period = 0.4f;

			SERIALIZE_FIELD(float, _rotation_duration);

			SERIALIZE_FIELD(GameObject*, _death_screen);

			SERIALIZE_FIELD(GameObject*, _hp_cell_1);
			SERIALIZE_FIELD(GameObject*, _hp_cell_2);
			SERIALIZE_FIELD(GameObject*, _hp_cell_3);
			SERIALIZE_FIELD(GameObject*, _hp_cell_4);
			std::vector<GameObject*> hp_cells;

			SERIALIZE_FIELD(GameObject*, _ammo_cell_1);
			SERIALIZE_FIELD(GameObject*, _ammo_cell_2);
			SERIALIZE_FIELD(GameObject*, _ammo_cell_3);
			SERIALIZE_FIELD(GameObject*, _ammo_cell_4);
			std::vector<GameObject*> ammo_cells;
			int _ammo_count;

			SERIALIZE_FIELD(GameObject*, _sword_ui_addon);
			SERIALIZE_FIELD(GameObject*, _maze_ui_addon);
			SERIALIZE_FIELD(GameObject*, _claw_ui_addon);

			SERIALIZE_FIELD(GameObject*, _weapon_charge_bar_go);
			ComponentProgressBar* _weapon_charge_bar = nullptr;

			SERIALIZE_FIELD(GameObject*, _camera);
			SERIALIZE_FIELD(GameObject*, _ui_damage);

			ComponentTransform* _player_transform = nullptr;
			ComponentAnimation* animation = nullptr;
			ComponentParticleSystem* _falling_dust_particles = nullptr;
			ComponentParticleSystem* _walking_dust_particles = nullptr;
			ComponentParticleSystem* _heal_effect_particles_1 = nullptr;
			ComponentParticleSystem* _heal_effect_particles_2 = nullptr;
			
			std::vector<Weapon> weapons{};

			// Internal state variables

			// Input buffer
			// Use for combo for now, reset when combo ends
			std::queue<float3> click_buffer{};
			bool dash_buffer = false;

			// Dash management
			unsigned _dash_charges = 2;
			float _current_dash_duration = 0.f;
			float _dash_progress = 0.0f;
			float _dash_charging_time = 0.0f;
			float3 _dash_start = float3::zero;
			float3 _dash_end = float3::zero;
			float3 _dash_direction = float3::zero;
			const unsigned max_dashed_from_positions = 5;
			std::deque<float3> dashed_from_positions;

			float3 _trail_start_pos = float3::zero;
			float3 _trail_start_scale = float3::zero;
			float3 _trail_end_pos = float3::zero;
			float3 _trail_end_scale = float3::zero;
			bool _show_dashtrail = false;

			//Attack management
			float _attack_current_cd = 0.0f;
			float _attack_current_duration = 0.0f;
			float _current_attack_cooldown = 0.f;
			float _attack_current_delay = 0.0f;
			float _after_attack_timer = 0.f;;
			int _current_bullet = -1;
			unsigned _attack_idx = 0;
			unsigned _current_weapon = 0;
			unsigned _attack_charges = 0;
			float _invulnerability_time_remaining = 0.0f;
			float _pickUp_duration = 0.5f;
			float pickUp_time = _pickUp_duration;

			// Movement management
			float _stun_time = 0.0f;
			float _stun_duration = 0.5f;
			float _rotation_progress = 0.0f;
			float _falling_distance = 10.0f;
			bool _should_rotate = false;
			bool _is_falling = false;

			// Camera management
			int _current_cam_setting = 0;
			std::vector<float3> _cam_positions = {};
			std::vector<float3> _cam_rotations = {};

			float3 _player_position = float3::zero;
			float3 _movement_direction = float3::zero;
			float3 _knock_start = float3::zero;
			float3 _knock_end = float3::zero;
			float3 _start_fall_pos = float3::zero;
			Quat _rotation_start = Quat::identity;
			Quat _rotation_target = Quat::identity;

			GameObject* _enemies;
			GameObject* _terrain;

			LevelManager* _level_manager;

		};
	} // namespace Scripting
} // namespace Hachiko
