#include "scriptingUtil/gameplaypch.h"
#include "constants/Scenes.h"
#include "constants/Sounds.h"
#include "ui/MainMenuManager.h"

Hachiko::Scripting::MainMenuManager::MainMenuManager(GameObject* new_game_object)
    : Script(new_game_object, "MainMenuManager")
    , _state(State::MAIN)
    , _state_changed(false)
    , _main_background(nullptr)
    , _button_background(nullptr)
    , _button_play(nullptr)
    , _button_quit(nullptr)
    , _button_settings(nullptr)
    , _button_credits(nullptr)
    , _game_title(nullptr)
    , _settings(nullptr)
    , _credits(nullptr)
    , _button_back(nullptr)
    , _audio_source(nullptr)
    , _quit_button_delay_duration(1.0f)
    , _remaining_waiting_time_for_quit(0.0f)
    , _started_to_quit(false) {}

void Hachiko::Scripting::MainMenuManager::OnAwake()
{
	bool any_null = false;

	_audio_source = game_object->GetComponent<ComponentAudioSource>();

	_main_background = game_object->GetFirstChildWithName(
		"button_whitebackground");
	any_null |= (_main_background == nullptr);

	_button_play = game_object->GetFirstChildWithName(
		"button_play")->GetComponent<ComponentButton>();
	any_null |= (_button_play == nullptr);

	_button_quit = game_object->GetFirstChildWithName(
		"button_quit")->GetComponent<ComponentButton>();
	any_null |= (_button_quit == nullptr);

	_button_settings = game_object->GetFirstChildWithName(
		"button_settings")->GetComponent<ComponentButton>();
	any_null |= (_button_settings == nullptr);

	_button_credits = game_object->GetFirstChildWithName(
		"button_credits")->GetComponent<ComponentButton>();
	any_null |= (_button_credits == nullptr);

	_credits = game_object->GetFirstChildWithName(
		"credits");
	any_null |= (_credits == nullptr);

	_settings = game_object->GetFirstChildWithName(
		"settings");
	any_null |= (_settings == nullptr);

	_button_back = game_object->GetFirstChildWithName(
		"button_back")->GetComponent<ComponentButton>();
	any_null |= (_button_back == nullptr);

	if (any_null)
	{
		HE_LOG("If you renamed or deleted any game object in this scene," 
			   " please check this script.");
	}
}

void Hachiko::Scripting::MainMenuManager::OnStart()
{
	_state = State::MAIN;
	_state_changed = true;
}

void Hachiko::Scripting::MainMenuManager::OnUpdate()
{
	switch (_state)
	{
		case State::MAIN:
			OnUpdateMain();
		break;
		
		case State::SETTINGS:
			OnUpdateSettings();
		break;
		
		case State::CREDITS:
			OnUpdateCredits();
		break;

		case State::PLAY:
			SceneManagement::SwitchScene(Scenes::GAME);
		break;
	}
}

void Hachiko::Scripting::MainMenuManager::OnUpdateMain()
{
	// Skip the update if the application is marked as quitting by the quit
	// button:
    if (OnUpdateQuit())
	{
		return;
	}

	if (_state_changed)
	{
		// Common:
		_main_background->SetActive(true);

		// Main:
		_button_play->GetGameObject()->SetActive(true);
		_button_settings->GetGameObject()->SetActive(true);
		_button_quit->GetGameObject()->SetActive(true);
		_button_credits->GetGameObject()->SetActive(true);
		_game_title->SetActive(true);

		// Other:
		_settings->SetActive(false);
		_credits->SetActive(false);
		_button_back->GetGameObject()->SetActive(false);

		_state_changed = false;
	}

	if (_button_quit->IsSelected())
	{
		QuitDelayed();

	    return;
	}

	if (_button_play->IsSelected())
	{
		_audio_source->PostEvent(Sounds::CLICK);
		_state = State::PLAY;
		_state_changed = true;

		return;
	}

	if (_button_settings->IsSelected())
	{
		_audio_source->PostEvent(Sounds::CLICK);
		_state = State::SETTINGS;
		_state_changed = true;

		return;
	}

	if (_button_credits->IsSelected())
	{
		_audio_source->PostEvent(Sounds::CLICK);
		_state = State::CREDITS;
		_state_changed = true;
	}
}

void Hachiko::Scripting::MainMenuManager::OnUpdateSettings()
{
	if (_state_changed)
	{	
		// Common
		_main_background->SetActive(true);

		// Settings:
		_settings->SetActive(true);
		_button_back->GetGameObject()->SetActive(true);

		// Other
		_credits->SetActive(false);
		_button_play->GetGameObject()->SetActive(false);
		_button_settings->GetGameObject()->SetActive(false);
		_button_quit->GetGameObject()->SetActive(false);
		_button_credits->GetGameObject()->SetActive(false);
		_game_title->SetActive(false);

		_state_changed = false;
	}
	
	if (_button_back->IsSelected())
	{
		_audio_source->PostEvent(Sounds::CLICK);
		_state = State::MAIN;
		_state_changed = true;
	}
}

void Hachiko::Scripting::MainMenuManager::OnUpdateCredits()
{
	if (_state_changed)
	{
		// Common
		_main_background->SetActive(true);

		// Credits:
		_credits->SetActive(true);
		_button_back->GetGameObject()->SetActive(true);

		// Other
		_settings->SetActive(false);
		_button_play->GetGameObject()->SetActive(false);
		_button_settings->GetGameObject()->SetActive(false);
		_button_quit->GetGameObject()->SetActive(false);
		_button_credits->GetGameObject()->SetActive(false);
		_game_title->SetActive(false);

		_state_changed = false;
	}

	if (_button_back->IsSelected())
	{
		_audio_source->PostEvent(Sounds::CLICK);
		_state = State::MAIN;
		_state_changed = true;
	}
}

bool Hachiko::Scripting::MainMenuManager::OnUpdateQuit()
{
	if (!_started_to_quit)
	{
		return false;
	}

	// Tick the time for the quit button activation delay:
	_remaining_waiting_time_for_quit -= Time::DeltaTime();
	_remaining_waiting_time_for_quit = _remaining_waiting_time_for_quit < 0.0f
		? 0.0f
		: _remaining_waiting_time_for_quit;

	// Quit when the delay is over:
	if (_remaining_waiting_time_for_quit == 0.0f)
	{
		// Not necessary as the application is quitting but done anyways to
		// ensure a valid state:
		_started_to_quit = false;

		Hachiko::Quit();

	    return true;
	}

	return true;
}

void Hachiko::Scripting::MainMenuManager::QuitDelayed()
{
	_audio_source->PostEvent(Sounds::CLICK);

	_remaining_waiting_time_for_quit = _quit_button_delay_duration;

	_started_to_quit = true;
}
