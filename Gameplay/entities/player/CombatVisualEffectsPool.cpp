#include "scriptingUtil/gameplaypch.h"
#include "entities/player/CombatVisualEffectsPool.h"
#include "constants/Scenes.h"

Hachiko::Scripting::CombatVisualEffectsPool::CombatVisualEffectsPool(GameObject* game_object)
	: Script(game_object, "CombatVisualEffectsPool")
	, _billboards({})
	, _current_billboard_index(0)
{
}

void Hachiko::Scripting::CombatVisualEffectsPool::OnAwake()
{
	_billboards.clear();
	_billboards.reserve(15);

	for (ComponentBillboard* billboard : game_object->GetComponentsInDescendants<ComponentBillboard>())
	{
		_billboards.push_back(billboard);
	}
}

void Hachiko::Scripting::CombatVisualEffectsPool::OnUpdate()
{
}

void Hachiko::Scripting::CombatVisualEffectsPool::PlayPlayerAttackEffect(PlayerController::WeaponUsed weapon_type, int attack_index, float3 position)
{
	if (_billboards.size() <= 0)
	{
		return;
	}

	_current_billboard_index = _current_billboard_index % _billboards.size();

	ComponentBillboard* current_attack_billboard = _billboards[_current_billboard_index];

	if (current_attack_billboard == nullptr)
	{
		return;
	}

	float3 main_camera_direction = (Scenes::GetMainCamera()->GetTransform()->GetGlobalPosition() - position).Normalized() * 1.5f;

	current_attack_billboard->GetGameObject()->GetTransform()->SetGlobalPosition(main_camera_direction + position + float3::unitY * 1.5f);

	switch (weapon_type)
	{
	case PlayerController::WeaponUsed::RED:
	{
		
		break;
	}
	case PlayerController::WeaponUsed::BLUE:
	{
		std::string texture_file_name = FileUtility::GetWorkingDirectory() + "/assets/textures/vfx/slash_sprite.png";

		current_attack_billboard->GetTextureProperties().SetTexture(texture_file_name);
		current_attack_billboard->GetTextureProperties().SetTiles(float2(3.0f, 4.0f));
		
		break;
	}
	case PlayerController::WeaponUsed::GREEN:
	{
		std::string texture_file_name = FileUtility::GetWorkingDirectory() + "/assets/textures/vfx/slash_vfx.png";

		current_attack_billboard->GetTextureProperties().SetTexture(texture_file_name);
		current_attack_billboard->GetTextureProperties().SetTiles(float2(5.0f, 2.0f));

		break;
	}
	default:
		break;
	}

	switch (attack_index)
	{
	case 0:
	{
		current_attack_billboard->GetTextureProperties().SetFlip(Hachiko::bool2(false, true));

		break;
	}

	case 1:
	{
		current_attack_billboard->GetTextureProperties().SetFlip(Hachiko::bool2(true, false));

		break;
	}

	case 2:
	{
		current_attack_billboard->GetTextureProperties().SetFlip(Hachiko::bool2(false, false));

		break;
	}

	default:
		break;
	}

	current_attack_billboard->Restart();

	_current_billboard_index = (_current_billboard_index + 1) % _billboards.size();
}

Hachiko::ComponentBillboard* Hachiko::Scripting::CombatVisualEffectsPool::GetCurrentBillboard() const
{
	return _billboards[_current_billboard_index];
}
