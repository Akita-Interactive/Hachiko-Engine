#pragma once
#include <scripting/Script.h>

namespace Hachiko
{
class ComponentButton;
class GameObject;

namespace Scripting
{
class MainMenuManager : public Script
{
	SERIALIZATION_METHODS(false)

public:
	enum class State
	{
		MAIN = 0,
		SETTINGS = 1,
		CREDITS = 2,
		PLAY = 3
	};

public:
	MainMenuManager(GameObject* new_game_object);

	void OnAwake() override;
	void OnStart() override;
	void OnUpdate() override;

private:
	void OnUpdateMain();
	void OnUpdateSettings();
	void OnUpdateCredits();
	// Returns true if quitting that frame, false otherwise.
	bool OnUpdateQuit();
	void QuitDelayed();

private:
	SERIALIZE_FIELD(State, _state);
	SERIALIZE_FIELD(bool, _state_changed);

	// Common Content:
	SERIALIZE_FIELD(GameObject*, _main_background);

	// Main State Content:
	SERIALIZE_FIELD(GameObject*, _button_background);
	SERIALIZE_FIELD(ComponentButton*, _button_play);
	SERIALIZE_FIELD(ComponentButton*, _button_quit);
	SERIALIZE_FIELD(ComponentButton*, _button_settings);
	SERIALIZE_FIELD(ComponentButton*, _button_credits);
	SERIALIZE_FIELD(GameObject*, _game_title);

	// Settings State Content:
	SERIALIZE_FIELD(GameObject*, _settings);

	// Credits State Content:
	SERIALIZE_FIELD(GameObject*, _credits);

	// Credits & Settings Common Content:
	SERIALIZE_FIELD(ComponentButton*, _button_back);

	ComponentAudioSource* _audio_source;

	// The delay for the quit button to actually quit the application. Used for
	// playing the button sound.
	// NOTE: When the button sounds are changed, change the value of this const
	// member on the ctor initializer list.
	const float _quit_button_delay_duration;
	float _remaining_waiting_time_for_quit;
	bool _started_to_quit;

	int _option_selected;
	std::vector<ComponentButton*> _buttons_gamepad;

};
} // namespace Scripting
} // namespace Hachiko
