#pragma once

#include <scripting/Script.h>
#include <components/ComponentAnimation.h>

namespace Hachiko
{
	class GameObject;

	namespace Scripting
	{
		class OrbController : public Script
		{
			SERIALIZATION_METHODS(false)

		public:
			OrbController(GameObject* game_object);
			~OrbController() override = default;

			void OnAwake() override;
			void OnUpdate() override;

			void DestroyOrb();

			bool IsPicked() {
				return picked;
			}

			SERIALIZE_FIELD(GameObject*, _orb);
			ComponentAnimation* cp_animation = nullptr;

			float _parasite_dissolve_time = 2.2f;
			float _parasite_dissolving_time_progress = 0.f;

			bool picked = false;
		};
	}
}