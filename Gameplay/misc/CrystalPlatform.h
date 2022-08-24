#pragma once

#include <scripting/Script.h>
#include "entities/Stats.h"

namespace Hachiko
{
	class GameObject;
	class ComponentTransform;

	namespace Scripting
	{
		enum class PlatformState
		{
			INVALID,
			HIDDEN,
			PLATFORM,
			TRIGGERED,
			SHAKING,
		};

		class CrystalPlatform : public Script
		{
			SERIALIZATION_METHODS(false)

		public:
			CrystalPlatform(GameObject* game_object);
			~CrystalPlatform() override = default;

			void OnStart() override;
			void OnUpdate() override;

			PlatformState GetState() {
				return _state;
			}

		private:
			void HidePlatform();
			void RegeneratePlatform();
			void ResetTimers();
			void ShakePlatform();
			bool IsTriggered();

			void UpdatePlatformStatus();
			

			float3 GetShakeOffset();

		public:
			PlatformState _state = PlatformState::PLATFORM;
			PlatformState _previous_state = PlatformState::INVALID;

			bool is_platform_active = false;
			bool is_shaking = false;

			GameObject* player = nullptr;
			ComponentAnimation* exploding_platform = nullptr;
			ComponentObstacle* obstacle = nullptr;
			

			SERIALIZE_FIELD(float, _seconds_before_shaking);
			SERIALIZE_FIELD(float, _seconds_shaking);
			SERIALIZE_FIELD(float, _seconds_to_regen);
			SERIALIZE_FIELD(float, _shake_intensity);

			SERIALIZE_FIELD(GameObject*, _invisible_obstacle);
			SERIALIZE_FIELD(GameObject*, _crystal_platform);

			float4x4 _initial_transform = float4x4::identity;

			float before_shake_elapsed = 0.0f;
			float shake_elapsed = 0.0f;
			float regen_elapsed = 0.f;

			float shake_magnitude = 1.0f;
			const float obstacle_radius_trigger_percent = 1.f;
		};
	}
}