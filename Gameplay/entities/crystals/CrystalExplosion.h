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

			void RegisterHit(int damage);
			bool IsAlive() { return _stats->IsAlive(); };
			bool IsDestroyed() { return _is_destroyed; };

			void DestroyCrystal();
			void RegenCrystal();

			void SpawnEffect();
		private:
			void SetVisible(bool v);
			void ResetCrystal();


		public:
			Stats* _stats;

		private:
			ComponentTransform* _transform;

			SERIALIZE_FIELD(GameObject*, _explosion_indicator_helper);
			SERIALIZE_FIELD(GameObject*, _explosion_effect);
			SERIALIZE_FIELD(float, _shake_intensity);
			SERIALIZE_FIELD(float, _seconds_shaking);

			SERIALIZE_FIELD(unsigned, _crashing_index);
			SERIALIZE_FIELD(float, _detecting_radius);
			SERIALIZE_FIELD(float, _explosion_radius);
			SERIALIZE_FIELD(float, _timer_explosion);
			SERIALIZE_FIELD(bool, _explosive_crystal);
			SERIALIZE_FIELD(float, _regen_time);
			SERIALIZE_FIELD(bool, _should_regen);

			SERIALIZE_FIELD(float, damage_effect_duration);
			GameObject* crystal_geometry = nullptr;
			float damage_effect_remaining_time = 0.0f;

			ComponentAudioSource* _audio_source;
			bool _is_destroyed = false;
			bool _is_exploding = false;
			bool _visible = false;
			float _current_regen_time = 0.f;
			float _current_explosion_timer = 0.f;

			math::float3 _player_pos;
			

			GameObject* enemies;
			GameObject* boss;

			ComponentAnimation* cp_animation = nullptr;
			ComponentObstacle* obstacle = nullptr;
			ComponentBillboard* spawn_billboard = nullptr;

			float4x4 _initial_transform = float4x4::identity;

			float regen_elapsed = 0.f;
			float shake_magnitude = 1.0f;
		};
	}
}