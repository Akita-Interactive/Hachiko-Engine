#pragma once

#include "scriptingUtil/framework.h"

extern "C" const GAMEPLAY_API size_t __cdecl SCRIPT_COUNT = 27;

extern "C" const GAMEPLAY_API char* __cdecl SCRIPT_NAMES[SCRIPT_COUNT] =
{
	"TriggerAnim",
	"Stats",
	"CrystalExplosion",
	"BugAnimationManager",
	"EnemyController",
	"CombatManager",
	"CombatVisualEffectsPool",
	"PlayerAnimationManager",
	"PlayerCamera",
	"PlayerController",
	"PlayerSoundManager",
	"RoomTeleporter",
	"AudioManager",
	"BlinkingLight",
	"CameraPosChange",
	"CrystalPlatform",
	"DoorController",
	"DynamicCamera",
	"FancyLights",
	"GauntletManager",
	"LaserController",
	"LevelManager",
	"PillarCheckpoint",
	"Spawner",
	"BackToMainMenu",
	"DebugManager",
	"MainMenuManager"
};
