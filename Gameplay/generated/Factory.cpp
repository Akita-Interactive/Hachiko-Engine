#include "scriptingUtil/gameplaypch.h"
#include "generated/Factory.h"
#include "CutsceneManager.h"
#include "PauseMenu.h"
#include "TriggerAnim.h"
#include "effects/BloomAnimator.h"
#include "effects/BounceEffect.h"
#include "effects/HoverEffect.h"
#include "effects/RotationEffect.h"
#include "entities/Stats.h"
#include "entities/crystals/CrystalExplosion.h"
#include "entities/crystals/Stalagmite.h"
#include "entities/enemies/BossController.h"
#include "entities/enemies/BugAnimationManager.h"
#include "entities/enemies/EnemyController.h"
#include "entities/player/CombatManager.h"
#include "entities/player/CombatVisualEffectsPool.h"
#include "entities/player/PlayerAnimationManager.h"
#include "entities/player/PlayerCamera.h"
#include "entities/player/PlayerController.h"
#include "entities/player/PlayerSoundManager.h"
#include "entities/player/RoomTeleporter.h"
#include "misc/AssetsObstacle.h"
#include "misc/AudioManager.h"
#include "misc/BlinkingLight.h"
#include "misc/BossLaserController.h"
#include "misc/CameraPosChange.h"
#include "misc/Centipedes.h"
#include "misc/CrystalPlatform.h"
#include "misc/DoorController.h"
#include "misc/DynamicCamera.h"
#include "misc/FancyLights.h"
#include "misc/GauntletManager.h"
#include "misc/LaserController.h"
#include "misc/LevelManager.h"
#include "misc/OrbController.h"
#include "misc/PillarCheckpoint.h"
#include "misc/Spawner.h"
#include "misc/StalagmiteManager.h"
#include "misc/TimeManager.h"
#include "misc/Tutorial.h"
#include "ui/BackToMainMenu.h"
#include "ui/DebugManager.h"
#include "ui/MainMenuManager.h"


Hachiko::Scripting::Script* InstantiateScript(Hachiko::GameObject* script_owner, const std::string& script_name)
{
	if (script_name == "CutsceneManager")
	{
		return new Hachiko::Scripting::CutsceneManager(script_owner);
	}

	if (script_name == "PauseMenu")
	{
		return new Hachiko::Scripting::PauseMenu(script_owner);
	}

	if (script_name == "TriggerAnim")
	{
		return new Hachiko::Scripting::TriggerAnim(script_owner);
	}

	if (script_name == "BloomAnimator")
	{
		return new Hachiko::Scripting::BloomAnimator(script_owner);
	}

	if (script_name == "BounceEffect")
	{
		return new Hachiko::Scripting::BounceEffect(script_owner);
	}

	if (script_name == "HoverEffect")
	{
		return new Hachiko::Scripting::HoverEffect(script_owner);
	}

	if (script_name == "RotationEffect")
	{
		return new Hachiko::Scripting::RotationEffect(script_owner);
	}

	if (script_name == "Stats")
	{
		return new Hachiko::Scripting::Stats(script_owner);
	}

	if (script_name == "CrystalExplosion")
	{
		return new Hachiko::Scripting::CrystalExplosion(script_owner);
	}

	if (script_name == "Stalagmite")
	{
		return new Hachiko::Scripting::Stalagmite(script_owner);
	}

	if (script_name == "BossController")
	{
		return new Hachiko::Scripting::BossController(script_owner);
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

	if (script_name == "CombatVisualEffectsPool")
	{
		return new Hachiko::Scripting::CombatVisualEffectsPool(script_owner);
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

	if (script_name == "AssetsObstacle")
	{
		return new Hachiko::Scripting::AssetsObstacle(script_owner);
	}

	if (script_name == "AudioManager")
	{
		return new Hachiko::Scripting::AudioManager(script_owner);
	}

	if (script_name == "BlinkingLight")
	{
		return new Hachiko::Scripting::BlinkingLight(script_owner);
	}

	if (script_name == "BossLaserController")
	{
		return new Hachiko::Scripting::BossLaserController(script_owner);
	}

	if (script_name == "CameraPosChange")
	{
		return new Hachiko::Scripting::CameraPosChange(script_owner);
	}

	if (script_name == "Centipedes")
	{
		return new Hachiko::Scripting::Centipedes(script_owner);
	}

	if (script_name == "CrystalPlatform")
	{
		return new Hachiko::Scripting::CrystalPlatform(script_owner);
	}

	if (script_name == "DoorController")
	{
		return new Hachiko::Scripting::DoorController(script_owner);
	}

	if (script_name == "DynamicCamera")
	{
		return new Hachiko::Scripting::DynamicCamera(script_owner);
	}

	if (script_name == "FancyLights")
	{
		return new Hachiko::Scripting::FancyLights(script_owner);
	}

	if (script_name == "GauntletManager")
	{
		return new Hachiko::Scripting::GauntletManager(script_owner);
	}

	if (script_name == "LaserController")
	{
		return new Hachiko::Scripting::LaserController(script_owner);
	}

	if (script_name == "LevelManager")
	{
		return new Hachiko::Scripting::LevelManager(script_owner);
	}

	if (script_name == "OrbController")
	{
		return new Hachiko::Scripting::OrbController(script_owner);
	}

	if (script_name == "PillarCheckpoint")
	{
		return new Hachiko::Scripting::PillarCheckpoint(script_owner);
	}

	if (script_name == "Spawner")
	{
		return new Hachiko::Scripting::Spawner(script_owner);
	}

	if (script_name == "StalagmiteManager")
	{
		return new Hachiko::Scripting::StalagmiteManager(script_owner);
	}

	if (script_name == "TimeManager")
	{
		return new Hachiko::Scripting::TimeManager(script_owner);
	}

	if (script_name == "Tutorial")
	{
		return new Hachiko::Scripting::Tutorial(script_owner);
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