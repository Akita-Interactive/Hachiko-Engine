#include "scriptingUtil/gameplaypch.h"
#include "constants/Scenes.h"
#include "entities/player/PlayerController.h"
#include "entities/player/PlayerCamera.h"

#include "StalagmiteManager.h"


Hachiko::Scripting::StalagmiteManager::StalagmiteManager(GameObject* game_object)
	: Script(game_object, "StalagmiteManager")
	, _falling_time(0.4f)
{
}

void Hachiko::Scripting::StalagmiteManager::OnAwake()
{
	_player = Scenes::GetPlayer();
	_player_controller = _player->GetComponent<PlayerController>();
	_combat_manager = Scenes::GetCombatManager()->GetComponent<CombatManager>();
	_player_camera = Scenes::GetMainCamera()->GetComponent<PlayerCamera>();
}

void Hachiko::Scripting::StalagmiteManager::OnStart()
{
	GenerateStalagmites();
}

void Hachiko::Scripting::StalagmiteManager::GenerateStalagmites()
{
	std::vector<GameObject*> stalagmites_children = game_object->children;

	for (int i = 0; i < stalagmites_children.size(); i++)
	{
		Stalagmite* stalagmite = stalagmites_children[i]->GetComponent<Stalagmite>();
		_stalagmites.push_back(stalagmite);
	}
}

void Hachiko::Scripting::StalagmiteManager::OnUpdate()
{
	_player_pos = _player_controller->GetGameObject()->GetTransform()->GetGlobalPosition();

	for (unsigned i = 0; i < _stalagmites.size(); i++)
	{
		if (_stalagmites[i]->GetState() == StalagmiteState::INVALID && CheckPreviousStalagmite(i))
		{
			// MOVE STALAGMITE TO THE PLAYER

			cooldown_elapsed += Time::DeltaTime();
			if (cooldown_elapsed >= _falling_cooldown)
			{
				_stalagmites[i]->SetNewState(StalagmiteState::FALLING);
				cooldown_elapsed = 0;
			}
			else {
				_player_camera->Shake(0.3f, 0.2f);
			}
		}
		else {
			if (_stalagmites[i]->GetState() == StalagmiteState::FALLING)
			{
				falling_elapsed += Time::DeltaTime();
				if (falling_elapsed >= _falling_time)
				{
					// AOE PLAYER DAMAGE
					 
					CombatManager::AttackStats attack_stats;
					attack_stats.damage = 1;
					attack_stats.knockback_distance = 0.0f;
					attack_stats.width = 3.5f;
					attack_stats.range = 4.5; // a bit bigger than its attack activation range
					attack_stats.type = CombatManager::AttackType::CIRCLE;
					_combat_manager->EnemyMeleeAttack(_stalagmites[i]->GetExplosionArea()
						->GetTransform()->GetGlobalMatrix(), attack_stats);
					

					_stalagmites[i]->SetNewState(StalagmiteState::COLLAPSED);
					falling_elapsed = 0;
				}
				else
				{
					FallingStalagmite(_stalagmites[i]);
				}

			}

			UpdateStalagmiteState(_stalagmites[i]);
		}
	}
}

void Hachiko::Scripting::StalagmiteManager::UpdateStalagmiteState(Stalagmite* stalagmite)
{
	StalagmiteState platform_state = stalagmite->GetState();
	bool state_changed = platform_state != stalagmite->GetPreviousState();

	if (!state_changed)
	{
		return;
	}

	stalagmite->SetPreviousState(platform_state);

	switch (platform_state)
	{
	case StalagmiteState::INVALID:
		return;
	case StalagmiteState::FALLING:
		_player_pos.y += 1.2f; // PLAYER HEIGHT
		stalagmite->GetGameObject()->GetTransform()->SetGlobalPosition(_player_pos);
		stalagmite->ActiveStalagmite();
		stalagmite->ActiveEffects();
		break;
	case StalagmiteState::COLLAPSED:
		stalagmite->SetStalagmiteOnGround();
		break;
	default:
		break;
	}
}

void Hachiko::Scripting::StalagmiteManager::FallingStalagmite(Stalagmite* stalagmite)
{
	stalagmite->Falling();
}

bool Hachiko::Scripting::StalagmiteManager::CheckPreviousStalagmite(int idx)
{
	if (idx == 0) // First stalagmite
	{
		return true;
	}
	else {
		return _stalagmites[(int)idx - 1]->IsStalagmiteCollapsed();
	}
}



