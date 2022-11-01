#include "scriptingUtil/gameplaypch.h"
#include "constants/Scenes.h"

#include "entities/Stats.h"
#include "entities/crystals/CrystalExplosion.h"
#include "entities/enemies/EnemyController.h"
#include "entities/player/PlayerController.h"
#include "entities/enemies/BossController.h"

#include "constants/Sounds.h"
#include "constants/Scenes.h"

// TODO: These two includes must go:
#include <modules/ModuleSceneManager.h>


// TODO: Joel make this class delta time scaled as well.

Hachiko::Scripting::CrystalExplosion::CrystalExplosion(GameObject* game_object)
	: Script(game_object, "CrystalExplosion")
	, _stats()
	, _explosion_radius(10.0f)
	, _detecting_radius(1.0f)
	, _explosive_crystal(false)
	, _explosion_indicator_helper(nullptr)
	, _timer_explosion(0.0f)
	, _explosion_effect(nullptr)
	, _regen_time(5.f)
	, _shake_intensity(0.1f)
	, _seconds_shaking(0.8f)
	, _should_regen(true)
	, damage_effect_duration(0.5f)
{
}

void Hachiko::Scripting::CrystalExplosion::OnAwake()
{
	enemies = Scenes::GetEnemiesContainer();
	boss = Scenes::GetBoss();
	_stats = game_object->GetComponent<Stats>();
	_audio_source = game_object->GetComponent<ComponentAudioSource>();
	_transform = game_object->GetTransform();
	cp_animation = game_object->GetComponent<ComponentAnimation>();
	obstacle = game_object->GetComponent<ComponentObstacle>();

	_initial_transform = game_object->GetTransform()->GetGlobalMatrix();
	crystal_geometry = game_object->FindDescendantWithName("Geo");
}

void Hachiko::Scripting::CrystalExplosion::OnStart()
{
	ResetCrystal();
}

void Hachiko::Scripting::CrystalExplosion::OnUpdate()
{
	if (!_stats)
	{
		return;
	}

	if (damage_effect_remaining_time >= 0.0f)
	{
		damage_effect_remaining_time -= Time::DeltaTime();
	}

	if (damage_effect_remaining_time >= 0.0f)
	{
		float progress = damage_effect_remaining_time / damage_effect_duration;
		crystal_geometry->ChangeEmissiveColor(float4(1.0f, 1.0f, 1.0f, progress), true);
	}
	else
	{
		crystal_geometry->ResetEmissive(true);
	}


	if (!_stats->IsAlive())
	{
		if (cp_animation->IsAnimationStopped())
		{
			_current_regen_time += Time::DeltaTime();

			if (_current_regen_time >= _regen_time)
			{
				ResetCrystal();
			}

			ShakeCrystal();

		}
	}

	if (_is_exploding)
	{
		if (_current_explosion_timer >= _timer_explosion)
		{
			ExplodeCrystal();
			DestroyCrystal();
			return;
		}
		else
		{
			_current_explosion_timer += Time::DeltaTime();
			return;
		}
	}

	if (_explosive_crystal && _stats->IsAlive() && !_is_exploding)
	{
		CheckRadiusExplosion();
	}
}

void Hachiko::Scripting::CrystalExplosion::StartExplosion()
{
	_is_exploding = true;

	for (GameObject* child : _explosion_effect->children)
	{
		child->SetActive(true);
		child->GetComponent<ComponentBillboard>()->Restart();
	}
}

void Hachiko::Scripting::CrystalExplosion::CheckRadiusExplosion()
{
	GameObject* player = Scenes::GetPlayer();

	float3 position = game_object->GetTransform()->GetGlobalPosition();

	if (_detecting_radius >= position.Distance(player->GetTransform()->GetGlobalPosition())
		&& player->GetComponent<PlayerController>()->IsAlive())
	{
		StartExplosion();
	}
}

void Hachiko::Scripting::CrystalExplosion::ExplodeCrystal()
{
	_is_exploding = false;
	_stats->ReceiveDamage(_stats->_max_hp);

	// Desable billboards
	for (GameObject* child : _explosion_effect->children)
	{
		child->SetActive(false);
	}

	std::vector<GameObject*> check_hit = {};

	if (enemies != nullptr)
	{
		for (GameObject* enemy_pack : enemies->children)
		{
			check_hit.insert(check_hit.end(), enemy_pack->children.begin(), enemy_pack->children.end());
		}
	}

	if (boss != nullptr)
	{
		check_hit.push_back(boss);
	}

	check_hit.push_back(Scenes::GetPlayer());

	std::vector<GameObject*> elements_hit = {};

	for (int i = 0; i < check_hit.size(); ++i)
	{
		if (check_hit[i]->active &&
			_explosion_radius >= _transform->GetGlobalPosition().Distance(check_hit[i]->GetTransform()->GetGlobalPosition()))
		{
			elements_hit.push_back(check_hit[i]);
		}
	}

	for (Hachiko::GameObject* element : elements_hit)
	{
		EnemyController* enemy_controller = element->GetComponent<EnemyController>();
		PlayerController* player_controller = element->GetComponent<PlayerController>();
		BossController* boss_controller = element->GetComponent<BossController>();

		float3 relative_dir = element->GetTransform()->GetGlobalPosition() - _transform->GetGlobalPosition();
		relative_dir.y = 0.0f;

		if (enemy_controller != nullptr)
		{
			enemy_controller->RegisterHit(_stats->_attack_power, relative_dir.Normalized(), 0.7f, false, false);
		}

		if (player_controller != nullptr)
		{
			player_controller->RegisterHit(_stats->_attack_power, true, relative_dir.Normalized(), false, PlayerController::DamageType::CRYSTAL);
		}

		if (boss_controller != nullptr)
		{
			boss_controller->RegisterHit(_stats->_attack_power);
		}
	}
}

void Hachiko::Scripting::CrystalExplosion::ShakeCrystal()
{
	ComponentTransform* transform = game_object->GetTransform();
	math::float3 current_position = transform->GetGlobalPosition();

	float3 shake_offset = float3::zero;

	shake_offset = GetShakeOffset();

	if (_should_regen)
	{
		// We are not doing this on Boss level because it causes moving the crystal to strange positions
		transform->SetGlobalPosition(_initial_transform.Col3(3) + shake_offset);
	}
}

float3 Hachiko::Scripting::CrystalExplosion::GetShakeOffset()
{
	if (_current_regen_time < _seconds_shaking)
	{
		float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float x = (r - 0.5f) * shake_magnitude * _shake_intensity;
		r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float z = (r - 0.5f) * shake_magnitude * _shake_intensity;

		_current_regen_time += Time::DeltaTime();
		shake_magnitude = (1 - (_current_regen_time / _seconds_shaking)) * (1 - (_current_regen_time / _seconds_shaking));
		return float3(x, 0, z);
	}
	else
	{
		return float3::zero;
	}
}

void Hachiko::Scripting::CrystalExplosion::RegisterHit(int damage)
{
	if (!_stats) return;

	damage_effect_remaining_time = damage_effect_duration;

	_stats->ReceiveDamage(damage);

	if (!_stats->IsAlive() && !_is_destroyed)
	{
		if (_explosive_crystal)
		{
			//StartExplosion();
			ExplodeCrystal();
			DestroyCrystal();
		}
		else
		{
			DestroyCrystal();
		}
	}
}

void Hachiko::Scripting::CrystalExplosion::SetVisible(bool v)
{
	_visible = v;
	game_object->SetVisible(v, true);
}

void Hachiko::Scripting::CrystalExplosion::ResetCrystal()
{
	_is_exploding = false;
	_is_destroyed = false;
	_stats->SetHealth(1);
	SetVisible(true);
	_current_explosion_timer = 0.f;
	_current_regen_time = 0.f;

	if (_explosion_indicator_helper)
	{
		_explosion_indicator_helper->SetActive(false);
	}

	if (obstacle)
	{
		obstacle->Enable();
		obstacle->AddObstacle();
	}

	cp_animation = game_object->GetComponent<ComponentAnimation>();
	if (cp_animation)
	{
		cp_animation->SendTrigger("isRegenerating");
	}

	if (!_should_regen)
	{
		// Is boss scene and crystal shouldn't regen so we move it far away
		float3 front = _transform->GetFront().Normalized();
		float3 move_away = _transform->GetGlobalPosition() + front * 50;
		_transform->SetGlobalPosition(move_away);
	}
}

void Hachiko::Scripting::CrystalExplosion::DestroyCrystal()
{
	_audio_source->PostEvent(Sounds::CRYSTAL);
	if (obstacle)
	{
		obstacle->RemoveObstacle();
		obstacle->Disable();
	}
	if (cp_animation)
	{
		_is_destroyed = true;
		cp_animation->SendTrigger("isExploding");
	}
}

void Hachiko::Scripting::CrystalExplosion::RegenCrystal()
{
	_current_regen_time = 0.f;
	if (cp_animation)
	{
		cp_animation->SendTrigger("isRegenerating");
	}
}