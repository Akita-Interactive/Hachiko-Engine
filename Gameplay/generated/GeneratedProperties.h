#pragma once

#include "scriptingUtil/framework.h"

extern "C" const GAMEPLAY_API size_t __cdecl SCRIPT_COUNT = 42;

extern "C" const GAMEPLAY_API char* __cdecl SCRIPT_NAMES[SCRIPT_COUNT] =
{
	"CutsceneManager",
	"PauseMenu",
	"TriggerAnim",
	"BloomAnimator",
	"BounceEffect",
	"HoverEffect",
	"RotationEffect",
	"Stats",
	"CrystalExplosion",
	"Stalagmite",
	"BossController",
	"BugAnimationManager",
	"EnemyController",
	"CombatManager",
	"CombatVisualEffectsPool",
	"PlayerAnimationManager",
	"PlayerCamera",
	"PlayerController",
	"PlayerSoundManager",
	"RoomTeleporter",
	"AssetsObstacle",
	"AudioManager",
	"BlinkingLight",
	"BossLaserController",
	"CameraPosChange",
	"Centipedes",
	"CrystalPlatform",
	"DoorController",
	"DynamicCamera",
	"FancyLights",
	"GauntletManager",
	"LaserController",
	"LevelManager",
	"OrbController",
	"PillarCheckpoint",
	"Spawner",
	"StalagmiteManager",
	"TimeManager",
	"Tutorial",
	"BackToMainMenu",
	"DebugManager",
	"MainMenuManager"
};
