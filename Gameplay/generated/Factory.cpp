#include "scriptingUtil/gameplaypch.h"
#include "generated/Factory.h"
#include "CrystalPlatform.h"
#include "entities/Stats.h"
#include "entities/crystals/CrystalExplosion.h"
#include "entities/enemies/BugAnimationManager.h"
#include "entities/enemies/EnemyController.h"
#include "entities/player/CombatManager.h"
#include "entities/player/PlayerAnimationManager.h"
#include "entities/player/PlayerCamera.h"
#include "entities/player/PlayerController.h"
#include "entities/player/PlayerSoundManager.h"
#include "entities/player/RoomTeleporter.h"
#include "misc/AudioManager.h"
#include "misc/DynamicCamera.h"
#include "misc/FancyLights.h"
#include "ui/BackToMainMenu.h"
#include "ui/DebugManager.h"
#include "ui/MainMenuManager.h"


Hachiko::Scripting::Script* InstantiateScript(Hachiko::GameObject* script_owner, const std::string& script_name)
{
	if (script_name == "CrystalPlatform")
	{
		return new Hachiko::Scripting::CrystalPlatform(script_owner);
	}

	if (script_name == "Stats")
	{
		return new Hachiko::Scripting::Stats(script_owner);
	}

	if (script_name == "CrystalExplosion")
	{
		return new Hachiko::Scripting::CrystalExplosion(script_owner);
	}

	if (script_name == "BugAnimationManager")
	{
		return new Hachiko::Scripting::BugAnimationManager(script_owner);
	}

	if (script_name == "EnemyController")
	{
		return new Hachiko::Scripting::EnemyController(script_owner);
	}

	if (script_name == "CombatManager")
	{
		return new Hachiko::Scripting::CombatManager(script_owner);
	}

	if (script_name == "PlayerAnimationManager")
	{
		return new Hachiko::Scripting::PlayerAnimationManager(script_owner);
	}

	if (script_name == "PlayerCamera")
	{
		return new Hachiko::Scripting::PlayerCamera(script_owner);
	}

	if (script_name == "PlayerController")
	{
		return new Hachiko::Scripting::PlayerController(script_owner);
	}

	if (script_name == "PlayerSoundManager")
	{
		return new Hachiko::Scripting::PlayerSoundManager(script_owner);
	}

	if (script_name == "RoomTeleporter")
	{
		return new Hachiko::Scripting::RoomTeleporter(script_owner);
	}

	if (script_name == "AudioManager")
	{
		return new Hachiko::Scripting::AudioManager(script_owner);
	}

	if (script_name == "DynamicCamera")
	{
		return new Hachiko::Scripting::DynamicCamera(script_owner);
	}

	if (script_name == "FancyLights")
	{
		return new Hachiko::Scripting::FancyLights(script_owner);
	}

	if (script_name == "BackToMainMenu")
	{
		return new Hachiko::Scripting::BackToMainMenu(script_owner);
	}

	if (script_name == "DebugManager")
	{
		return new Hachiko::Scripting::DebugManager(script_owner);
	}

	if (script_name == "MainMenuManager")
	{
		return new Hachiko::Scripting::MainMenuManager(script_owner);
	}

	return nullptr;
}