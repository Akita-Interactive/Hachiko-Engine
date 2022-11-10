#include "scriptingUtil/gameplaypch.h"
#include "CutsceneManager.h"
#include "components/ComponentVideo.h"
#include "components/ComponentAudioSource.h"
#include "constants/Scenes.h"
#include "constants/Sounds.h"

Hachiko::Scripting::CutsceneManager::CutsceneManager(GameObject* new_game_object)
    : Script(new_game_object, "CutsceneManager")
	, _type(CutsceneType::INTRO)
{}

void Hachiko::Scripting::CutsceneManager::OnAwake()
{
    if (_cutscene != nullptr)
    {
        _cutscene_video = _cutscene->GetComponent<ComponentVideo>();
    }

    _audio_source = game_object->GetComponent<ComponentAudioSource>();
}

void Hachiko::Scripting::CutsceneManager::OnStart()
{
    if (_cutscene_video != nullptr)
    {
        _cutscene_video->Play();
    }

    if (_audio_source != nullptr)
    {
	    switch (_type)
	    {

		case CutsceneType::INTRO:
			_audio_source->PostEvent(Sounds::INTRO_CINEMATIC);
            break;
		case CutsceneType::OUTRO:
            _audio_source->PostEvent(Sounds::OUTRO_CINEMATIC);
            break;
		default:
            break;
	    }
    }
}

void Hachiko::Scripting::CutsceneManager::OnUpdate()
{
    if (_cutscene_video != nullptr && !_cutscene_video->IsPlaying())
    {
        switch (_next_level)
        {
        case 0:
            SceneManagement::SwitchScene(Scenes::MAIN_MENU);
            break;
        case 1:
            SceneManagement::SwitchScene(Scenes::GAME);
            break;
        case 2:
            SceneManagement::SwitchScene(Scenes::LEVEL2);
            break;
        case 3:
            SceneManagement::SwitchScene(Scenes::BOSS_LEVEL);
            break;
        }
    }
}
