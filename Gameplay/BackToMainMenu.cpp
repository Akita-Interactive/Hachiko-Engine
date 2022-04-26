#include "scriptingUtil/gameplaypch.h"
#include "BackToMainMenu.h"
#include "Scenes.h"

Hachiko::Scripting::BackToMainMenu::BackToMainMenu(GameObject* game_object)
	: Script(game_object, "BackToMainMenu")
	, _button_back(nullptr)
{
}

void Hachiko::Scripting::BackToMainMenu::OnAwake()
{
	_button_back = game_object->GetComponent<ComponentButton>();

	if (_button_back == nullptr)
	{
		HE_LOG("Owner of this script must have ComponentButton.");
	}
}

void Hachiko::Scripting::BackToMainMenu::OnStart()
{
}

void Hachiko::Scripting::BackToMainMenu::OnUpdate()
{
	// TODO: Uncomment this in the next PR after adding the new scenes with
	// YAML based serialization.
	/*if (_button_back->IsSelected())
	{
		SceneManagement::SwitchScene(Scenes::MAIN_MENU);
	}*/
}