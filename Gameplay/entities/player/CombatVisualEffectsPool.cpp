#include "scriptingUtil/gameplaypch.h"
#include "entities/player/CombatVisualEffectsPool.h"
#include "constants/Scenes.h"

Hachiko::Scripting::CombatVisualEffectsPool::CombatVisualEffectsPool(GameObject* game_object)
	: Script(game_object, "CombatVisualEffectsPool")
	, claw_vfxes({})
	, sword_vfxes({})
	, melee_vfxes({})
	, hammer_vfxes({})
	, worm_vfxes({})
	, beetle_vfxes({})
	, current_claw_vfx_index(0)
{
}

void Hachiko::Scripting::CombatVisualEffectsPool::OnAwake()
{
	// 0 - Claw 
	claw_vfxes.clear();
	claw_vfxes.reserve(10);
	claw_vfxes = game_object->children[0]->children;

	// 1 - Sword 
	sword_vfxes.clear();
	sword_vfxes.reserve(10);
	sword_vfxes = game_object->children[1]->children;

	// 2 - Melee 
	melee_vfxes.clear();
	melee_vfxes.reserve(10);
	melee_vfxes = game_object->children[2]->children;

	// 3 - Hammer 
	hammer_vfxes.clear();
	hammer_vfxes.reserve(10);
	hammer_vfxes = game_object->children[3]->children;

	// 4 - Worm 
	worm_vfxes.clear();
	worm_vfxes.reserve(10);
	worm_vfxes = game_object->children[4]->children;

	// 5 - Beetle 
	beetle_vfxes.clear();
	beetle_vfxes.reserve(10);
	beetle_vfxes = game_object->children[5]->children;
}

void Hachiko::Scripting::CombatVisualEffectsPool::PlayPlayerAttackEffect(PlayerController::WeaponUsed weapon_type, int attack_index, float3 position)
{
	ComponentBillboard* current_attack_billboard = nullptr;

	switch (weapon_type)
	{
	case PlayerController::WeaponUsed::MELEE:
	{
		current_attack_billboard = GetCurrentMeleeVfx();
		break;
	}
	case PlayerController::WeaponUsed::CLAW:
	{
		current_attack_billboard = GetCurrentClawVfx();
		break;
	}
	case PlayerController::WeaponUsed::SWORD:
	{
		current_attack_billboard = GetCurrentSwordVfx();
		break;
	}
	case PlayerController::WeaponUsed::HAMMER:
	{
		current_attack_billboard = GetCurrentHammerVfx();
		break;
	}
	default:
		break;
	}

	if (current_attack_billboard == nullptr)
	{
		return;
	}

	float3 main_camera_direction = (Scenes::GetMainCamera()->GetTransform()->GetGlobalPosition() - position).Normalized() * 1.5f;

	current_attack_billboard->GetGameObject()->GetTransform()->SetGlobalPosition(main_camera_direction + position + float3::unitY * 1.5f);

	current_attack_billboard->Restart();
}

void Hachiko::Scripting::CombatVisualEffectsPool::PlayEnemyAttackEffect(EnemyType _enemy_type, float3 position)
{
	ComponentBillboard* current_attack_billboard = nullptr;

	switch (_enemy_type)
	{
	case EnemyType::BEETLE:
	{
		current_attack_billboard = GetCurrentBeetleVfx();
		break;
	}
	case EnemyType::WORM:
	{
		current_attack_billboard = GetCurrentWormVfx();
		break;
	}
	default:
		break;
	}

	if (current_attack_billboard == nullptr)
	{
		return;
	}

	float3 main_camera_direction = (Scenes::GetMainCamera()->GetTransform()->GetGlobalPosition() - position).Normalized() * 1.5f;

	current_attack_billboard->GetGameObject()->GetTransform()->SetGlobalPosition(main_camera_direction + position + float3::unitY * 1.5f);

	current_attack_billboard->Restart();
}

