#pragma once

#include <scripting/Script.h>
#include "entities/Stats.h"

namespace Hachiko
{
	class GameObject;
	class ComponentTransform;
	class ComponentBillboard;


	namespace Scripting
	{
		
		class CombatVisualEffectsPool;
		
		class CrystalExplosion : public Script
		{
			SERIALIZATION_METHODS(false)


		public:
			CrystalExplosion(GameObject* game_object);
			~CrystalExplosion() override = default;


			void OnAwake() override;
			void OnStart() override;
			void OnUpdate() override;

			void StartExplosion();
			void CheckRadiusExplosion();
			void ExplodeCrystal();

			void ShakeCrystal();

			float3 GetShakeOffset();

			void RegisterHit(int damage,  bool is_from_player, bool is_ranged);
			bool IsAlive() { return _stats->IsAlive(); };
			bool IsDestroyed() { return _is_destroyed; };

			void DestroyCrystal();
			void RegenCrystal();

			void DissolveCrystal(bool be_dissolved);

			void SpawnEffect();
		private:
			void SetVisible(bool v);
			void ResetCrystal();
			void StartDomeVFX(float progress);

		public:
			Stats* _stats;

		private:
			ComponentTransform* _transform;

			SERIALIZE_FIELD(GameObject*, _explosion_indicator_helper);
			SERIALIZE_FIELD(GameObject*, _explosion_indicator);
			SERIALIZE_FIELD(GameObject*, _explosion_vfx);
			SERIALIZE_FIELD(GameObject*, _explosion_particles);
			SERIALIZE_FIELD(GameObject*, _explosion_dome);
			SERIALIZE_FIELD(float, _dome_vfx_duration);
			SERIALIZE_FIELD(float, _dome_vfx_size);
			SERIALIZE_FIELD(float, _dome_dissolving_time);
			SERIALIZE_FIELD(float, _shake_intensity);
			SERIALIZE_FIELD(float, _seconds_shaking);

			SERIALIZE_FIELD(unsigned, _crashing_index);
			SERIALIZE_FIELD(float, _detecting_radius);
			SERIALIZE_FIELD(float, _explosion_radius);
			SERIALIZE_FIELD(float, _timer_explosion);
			SERIALIZE_FIELD(bool, _explosive_crystal);
			SERIALIZE_FIELD(float, _regen_time);
			SERIALIZE_FIELD(bool, _should_regen);
			SERIALIZE_FIELD(bool, _for_boss_cocoon);

			SERIALIZE_FIELD(float, damage_effect_duration);
			GameObject* crystal_geometry = nullptr;
			float damage_effect_remaining_time = 0.0f;

			ComponentAudioSource* _audio_source;
			bool _is_destroyed = false;
			bool _is_exploding = false;
			bool _casting_dome_vfx = false;
			bool _visible = false;

			bool _is_dissolving = false;
			float _dissolving_time = 1.5f;
			float _current_dissolving_time = 0.f;
			float _current_regen_time = 0.f;

			bool _is_dome_dissolving = false;
			float _dome_current_dissolving_time = 0.f;

			float _current_explosion_timer = 0.f;
			float explosion_progression = 0.f;

			math::float3 _player_pos;
			

			GameObject* enemies;
			GameObject* boss;

			ComponentAnimation* cp_animation = nullptr;
			ComponentObstacle* obstacle = nullptr;
			ComponentBillboard* spawn_billboard = nullptr;

			float4x4 _initial_transform = float4x4::identity;

			float regen_elapsed = 0.f;
			float shake_magnitude = 1.0f;

			CombatVisualEffectsPool* effects_pool = nullptr;
		};
	}
}