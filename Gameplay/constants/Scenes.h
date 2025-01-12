#pragma once

#include "Gameplay.h"

namespace Hachiko::Scenes
{
	static const UID GAME = 3947210229329588811;
	static const UID WIN = 3083261127926411514;
	static const UID LOSE = 3083261127926411514;
	static const UID MAIN_MENU = 3083261127926411514;
	static const UID LEVEL2 = 8516499815050636676;
	static const UID BOSS_LEVEL = 9597392922211235555;
	static const UID CUTSCENE_INTRO = 16234779812797112625;
	static const UID CUTSCENE_OUTRO = 11440117149198132110;

	static const char* player_go_name = "Player";
	static const char* boss_go_name = "Boss";
	static const char* main_camera_go_name = "Main Camera";
	static const char* enemies_go_name = "Enemies";
	static const char* level_manager_go_name = "LevelManager";
	static const char* terrain_container_go_name = "Level";
	static const char* laser_colliders_go_name = "LaserColliders";
	static const char* audio_manager_go_name = "AudioManager";
	static const char* combat_manager_go_name = "CombatManager";
	static const char* combat_visual_effects_pool_go_name = "CombatVisualEffectsPool";


	static GameObject* GetPlayer()
	{
		return SceneManagement::FindInCurrentScene(player_go_name);
	}

	static GameObject* GetBoss()
	{
		return SceneManagement::FindInCurrentScene(boss_go_name);
	}

	static GameObject* GetEnemiesContainer()
	{
		return SceneManagement::FindInCurrentScene(enemies_go_name);
	}

	static GameObject* GetLevelManager()
	{
		return SceneManagement::FindInCurrentScene(level_manager_go_name);
	}

	static GameObject* GetTerrainContainer()
	{
		return SceneManagement::FindInCurrentScene(terrain_container_go_name);
	}

	static GameObject* GetLaserColliderContainer()
	{
		return SceneManagement::FindInCurrentScene(laser_colliders_go_name);
	}

	static GameObject* GetAudioManager()
	{
		return SceneManagement::FindInCurrentScene(audio_manager_go_name);
	}

	static GameObject* GetCombatManager()
	{
		return SceneManagement::FindInCurrentScene(combat_manager_go_name);
	}

	static GameObject* GetCombatVisualEffectsPool()
	{
		return SceneManagement::FindInCurrentScene(combat_visual_effects_pool_go_name);
	}

	static GameObject* GetMainCamera()
	{
		return SceneManagement::FindInCurrentScene(main_camera_go_name);
	}
}