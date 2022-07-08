#pragma once

#include <scripting/Script.h>

namespace Hachiko
{
	namespace Scripting
	{
		class TriggerAnim : public Script
		{
			SERIALIZATION_METHODS(false)

		public:

			TriggerAnim(GameObject* game_object);
			~TriggerAnim() override = default;


			void OnAwake() override;
			void OnStart() override;
			void OnUpdate() override;

			ComponentAnimation* animation;
		};
	}
}


