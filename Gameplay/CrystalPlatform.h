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
			IDLE,
			PLATFORM,
			SHAKING
		};

		class CrystalPlatform : public Script
		{
			SERIALIZATION_METHODS(false)

		public:
			CrystalPlatform(GameObject* game_object);
			~CrystalPlatform() override = default;

			void OnAwake() override;
			void OnStart() override;
			void OnUpdate() override;

			PlatformState GetState() {
				return _state;
			}

		private:
			void ShowPlatform();
			void RegenerateCrystal();
			void UpdatePlatformStatus();

			float3 Shake();

		public:
			PlatformState _state = PlatformState::INVALID;

			bool is_platform_active = false;
			bool is_shaking = false;

			ComponentAnimation* exploding_platform;
			ComponentObstacle* obstacle;
			Stats* stats;

			SERIALIZE_FIELD(float, _seconds_before_shaking);
			SERIALIZE_FIELD(float, _seconds_shaking);
			SERIALIZE_FIELD(float, _shake_intensity);

			SERIALIZE_FIELD(PlatformState, _previous_state);

			SERIALIZE_FIELD(GameObject*, _invisible_obstacle);
			SERIALIZE_FIELD(GameObject*, _crystal);
			SERIALIZE_FIELD(GameObject*, _crystal_platform);

			float shake_elapsed = 0.0f;
			float shake_magnitude = 1.0f;
		};
	}
}