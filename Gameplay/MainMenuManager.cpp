#include "scriptingUtil/gameplaypch.h"
#include "MainMenuManager.h"

Hachiko::Scripting::MainMenuManager::MainMenuManager(GameObject* game_object)
	: Script(game_object, "MainMenuManager")
	, _state(State::MAIN)
	, _state_changed(false)
	, _main_background(nullptr)
	, _button_background(nullptr)
	, _button_play(nullptr)
	, _button_quit(nullptr)
	, _button_settings(nullptr)
	, _button_credits(nullptr)
	, _credits(nullptr)
	, _settings(nullptr)
	, _button_back(nullptr)
{}

void Hachiko::Scripting::MainMenuManager::OnAwake()
{
	HE_LOG("ON AWAKE");

	bool any_null = false;

	_main_background = game_object->GetFirstChildWithName(
		"button_whitebackground");
	any_null |= (_main_background == nullptr);
	
	_button_background = game_object->GetFirstChildWithName(
		"button_background");
	any_null |= (_button_background == nullptr);

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
		HE_LOG("If you renamed any game object in this scene," 
			   " please check this script.");
	}
}

void Hachiko::Scripting::MainMenuManager::OnStart()
{
	HE_LOG("ON START");
	_state = State::MAIN;
	_state_changed = true;
}

void Hachiko::Scripting::MainMenuManager::OnUpdate()
{
	switch (_state)
	{
		case Hachiko::Scripting::MainMenuManager::State::MAIN:
			OnUpdateMain();
		break;
		
		case Hachiko::Scripting::MainMenuManager::State::SETTINGS:
			OnUpdateSettings();
		break;
		
		case Hachiko::Scripting::MainMenuManager::State::CREDITS:
			OnUpdateCredits();
		break;
		
		default:
			HE_LOG("This should never happen.");
		break;
	}
}

void Hachiko::Scripting::MainMenuManager::OnUpdateMain()
{
	if (_state_changed)
	{
		// Common:
		_main_background->SetActive(true);

		// Main:
		_button_background->SetActive(true);
		_button_play->GetGameObject()->SetActive(true);
		_button_settings->GetGameObject()->SetActive(true);
		_button_quit->GetGameObject()->SetActive(true);
		_button_credits->GetGameObject()->SetActive(true);

		// Other:
		_settings->SetActive(false);
		_credits->SetActive(false);
		_button_back->GetGameObject()->SetActive(false);

		_state_changed = false;
	}

	if (_button_play->IsSelected())
	{
		SceneManagement::SwitchScene("Assets/Scenes/untitled.scene");

		return;
	}

	if (_button_settings->IsSelected())
	{
		_state = State::SETTINGS;
		_state_changed = true;

		return;
	}

	if (_button_credits->IsSelected())
	{
		_state = State::CREDITS;
		_state_changed = true;

		return;
	}

	if (_button_quit->IsSelected())
	{
		HE_LOG("This should quit the game");
		
		return;
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
		_button_background->SetActive(false);
		_button_play->GetGameObject()->SetActive(false);
		_button_settings->GetGameObject()->SetActive(false);
		_button_quit->GetGameObject()->SetActive(false);
		_button_credits->GetGameObject()->SetActive(false);

		_state_changed = false;
	}
	
	if (_button_back->IsSelected())
	{
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
		_button_background->SetActive(false);
		_button_play->GetGameObject()->SetActive(false);
		_button_settings->GetGameObject()->SetActive(false);
		_button_quit->GetGameObject()->SetActive(false);
		_button_credits->GetGameObject()->SetActive(false);

		_state_changed = false;
	}

	if (_button_back->IsSelected())
	{
		_state = State::MAIN;
		_state_changed = true;
	}
}