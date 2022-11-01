#include "scriptingUtil/gameplaypch.h"
#include "constants/Scenes.h"
#include "entities/player/PlayerController.h"
#include "entities/player/PlayerCamera.h"
#include "entities/enemies/EnemyController.h"

#include "StalagmiteManager.h"


Hachiko::Scripting::StalagmiteManager::StalagmiteManager(GameObject* game_object)
	: Script(game_object, "StalagmiteManager")
	, _falling_cooldown(0.5f)
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
    std::vector<GameObject*>& stalagmites_children = game_object->children;
    _stalagmites.clear();
    _stalagmites.reserve(stalagmites_children.size());

    for (GameObject* child : stalagmites_children)
    {
        Stalagmite* stalagmite = child->GetComponent<Stalagmite>();

        if (!stalagmite)
        {
            continue;
        }

        _stalagmites.push_back(stalagmite);
		stalagmite->Dissolved();
    }

	cooldown_elapsed = 0.f;
}

void Hachiko::Scripting::StalagmiteManager::OnUpdate()
{
	if (!_should_update_stalagmites)
	{
		return;
	}

	_player_pos = _player_controller->GetGameObject()->GetTransform()->GetGlobalPosition();

	size_t updated_stalagmites_count = 0;

	if (_stalactites_timer < 0.0f)
	{
		DestroyAllStalagmites();
	}
	else
	{
		_stalactites_timer -= Time::DeltaTimeScaled();
	}

	for (unsigned i = 0; i < _stalagmites.size(); i++)
	{
		if (!_should_fall_stalagmites)
		{
			if (_stalagmites[i]->GetState() == StalagmiteState::DISSOLVING)
			{
				_stalagmites[i]->_dissolving_time -= Time::DeltaTimeScaled();
				_stalagmites[i]->GetGameObject()->ChangeDissolveProgress(_stalagmites[i]->_dissolving_time/_total_dissolving_time, true);
				if (_stalagmites[i]->_dissolving_time <= 0.0f)
				{
					_stalagmites[i]->SetNewState(StalagmiteState::INVALID);
					_stalagmites[i]->Dissolved();
					++updated_stalagmites_count;
				}
			}
			if (updated_stalagmites_count >= _stalagmites.size())
			{
				_should_update_stalagmites = false;
				return;
			}

			continue;
		}


		if (_stalagmites[i]->GetState() == StalagmiteState::INVALID && CheckPreviousStalagmite(i))
		{
			// MOVE STALAGMITE TO THE PLAYER

			cooldown_elapsed += Time::DeltaTimeScaled();
			if (cooldown_elapsed >= _falling_cooldown)
			{
				_stalagmites[i]->SetNewState(StalagmiteState::FALLING);
				cooldown_elapsed = 0.f;
			}
			else
			{
				_player_camera->Shake(0.15f, 0.1f);
			}
		}
		else {
			if (_stalagmites[i]->GetState() == StalagmiteState::FALLING)
			{
				falling_elapsed += Time::DeltaTimeScaled();
				if (falling_elapsed >= _falling_time)
				{
					FallingStalagmite(_stalagmites[i], 1.0f);

					// AOE PLAYER DAMAGE

					_player_camera->Shake(1.5f, 2.f);
					 
					CombatManager::AttackStats attack_stats;
					attack_stats.damage = 1;
					attack_stats.knockback_distance = 2.0f;
					attack_stats.width = 3.5f;
					attack_stats.range = 3.5;
					attack_stats.type = CombatManager::AttackType::CIRCLE;
					_combat_manager->EnemyMeleeAttack(_stalagmites[i]->GetExplosionArea()
						->GetTransform()->GetGlobalMatrix(), attack_stats);

					KnockbackOnEnemies(_stalagmites[i]->GetExplosionArea()
						->GetTransform()->GetGlobalPosition());

					_stalagmites[i]->SetNewState(StalagmiteState::COLLAPSED);

				    ++updated_stalagmites_count;

				    falling_elapsed = -1.f;
				}
				else if (falling_elapsed > 0.0f)
				{
					float fall_progress = falling_elapsed / _falling_time;
					FallingStalagmite(_stalagmites[i], fall_progress);
				}
			}

			UpdateStalagmiteState(_stalagmites[i]);
		}
	}

	if (updated_stalagmites_count >= _stalagmites.size())
	{
		_should_fall_stalagmites = false;
		_should_update_stalagmites = true;
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
	case StalagmiteState::DISSOLVING:
		break;
	default:
		break;
	}
}

void Hachiko::Scripting::StalagmiteManager::FallingStalagmite(Stalagmite* stalagmite, float fall_progress)
{
	stalagmite->Falling(fall_progress, _player);
}

void Hachiko::Scripting::StalagmiteManager::TriggerStalagmites()
{
	_should_fall_stalagmites = true;
	_should_update_stalagmites = true;

	_stalactites_timer = _stalactites_life;

	GenerateStalagmites();
}

bool Hachiko::Scripting::StalagmiteManager::CheckPreviousStalagmite(int idx)
{
    return (idx == 0) ? true : _stalagmites[static_cast<size_t>(idx - 1)]->IsStalagmiteCollapsed();
}

void Hachiko::Scripting::StalagmiteManager::DestroyAllStalagmites(bool force)
{
	_should_fall_stalagmites = false;

	for (unsigned i = 0; i < _stalagmites.size(); ++i)
	{
		_stalagmites[i]->SetNewState(StalagmiteState::DISSOLVING);
		// Set a random offset for the dissolving
		const float offset = force ? 0 : RandomUtil::RandomBetween(0, 3);
		_stalagmites[i]->_dissolving_time = _total_dissolving_time + offset;
	}
}

void Hachiko::Scripting::StalagmiteManager::KnockbackOnEnemies(float3 position)
{
	std::vector<GameObject*> check_hit = {};
	float explosion_radius = 3.5f;

	GameObject* enemies = Scenes::GetEnemiesContainer();

	// Get all the enemies
	if (enemies != nullptr)
	{
		for (GameObject* enemy_pack : enemies->children)
		{
			check_hit.insert(check_hit.end(), enemy_pack->children.begin(), enemy_pack->children.end());
		}
	}

	std::vector<GameObject*> elements_hit = {};

	//Check if the enemies are hit
	for (int i = 0; i < check_hit.size(); ++i)
	{
		if (check_hit[i]->active &&
			explosion_radius >= position.Distance(check_hit[i]->GetTransform()->GetGlobalPosition()))
		{
			elements_hit.push_back(check_hit[i]);
		}
	}

	for (Hachiko::GameObject* element : elements_hit)
	{
		EnemyController* enemy_controller = element->GetComponent<EnemyController>();

		float3 relative_dir = element->GetTransform()->GetGlobalPosition() - position;
		relative_dir.y = 0.0f;

		// If it hits an enemy perfectly in the center displace it to positive Z
		if (relative_dir.Equals(float3::zero))
		{
			relative_dir = float3(0.0f, 0.0f, 1.0f);
		}

		if (enemy_controller != nullptr)
		{
			enemy_controller->RegisterHit(0, relative_dir.Normalized(), 2.0f, false, false);
		}
	}
};

bool Hachiko::Scripting::StalagmiteManager::AllStalactitesCollapsed()
{
	int collapsed = 0;
	for (unsigned i = 0; i < _stalagmites.size(); ++i)
	{
		if (_stalagmites[i]->GetState() == StalagmiteState::COLLAPSED)
		{
			++collapsed;
		}
	}
	return (collapsed == _stalagmites.size());
}