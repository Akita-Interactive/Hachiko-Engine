#pragma once

#include "scriptingUtil/framework.h"

extern "C" const GAMEPLAY_API size_t __cdecl SCRIPT_COUNT = 23;

extern "C" const GAMEPLAY_API char* __cdecl SCRIPT_NAMES[SCRIPT_COUNT] =
{
	"CrystalPlatform",
	"Stats",
	"CrystalExplosion",
	"BugAnimationManager",
	"EnemyController",
	"CombatManager",
	"PlayerAnimationManager",
	"PlayerCamera",
	"PlayerController",
	"PlayerSoundManager",
	"RoomTeleporter",
	"AudioManager",
	"BlinkingLight",
	"DoorController",
	"DynamicCamera",
	"FancyLights",
	"GauntletManager",
	"LevelManager",
	"PillarCheckpoint",
	"Spawner",
	"BackToMainMenu",
	"DebugManager",
	"MainMenuManager"
};
