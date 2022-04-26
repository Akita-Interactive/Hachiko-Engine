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
		CREDITS = 2
	};

public:
	MainMenuManager(GameObject* game_object);
	~MainMenuManager() override = default;

	void OnAwake() override;
	void OnStart() override;
	void OnUpdate() override;

private:
	void OnUpdateMain();
	void OnUpdateSettings();
	void OnUpdateCredits();

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

	// Settings State Content:
	SERIALIZE_FIELD(GameObject*, _settings);

	// Credits State Content:
	SERIALIZE_FIELD(GameObject*, _credits);

	// Credits & Settings Common Content:
	SERIALIZE_FIELD(ComponentButton*, _button_back);

};
} // namespace Scripting
} // namespace Hachiko