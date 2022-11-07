#pragma once
#include <scripting/Script.h>
namespace Hachiko
{
	class ComponentVideo;
	class ComponentAudioSource;

	namespace Scripting
	{
		class CutsceneManager : public Script
		{


		public:
			SERIALIZATION_METHODS(false)
			CutsceneManager(GameObject* game_object);

			void OnAwake() override;
			void OnStart() override;
			void OnUpdate() override;
		private:
			SERIALIZE_FIELD(GameObject*, _cutscene);
			SERIALIZE_FIELD(unsigned, _next_level);
			SERIALIZE_FIELD(unsigned, _type);
			ComponentVideo* _cutscene_video = nullptr;
			ComponentAudioSource* _audio_source = nullptr;
		};


		namespace CutsceneType
		{
			constexpr unsigned INTRO = 0;
			constexpr unsigned OUTRO = 1;
		}
	}
}

