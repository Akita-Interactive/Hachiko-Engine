#include "scriptingUtil/gameplaypch.h"
#include "constants/Scenes.h"

#include "entities/Stats.h"
#include "entities/crystals/CrystalExplosion.h"
#include "entities/enemies/EnemyController.h"
#include "entities/player/PlayerController.h"
#include "constants/Sounds.h"
#include "constants/Scenes.h"

// TODO: These two includes must go:
#include <modules/ModuleSceneManager.h>


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
{
}

void Hachiko::Scripting::CrystalExplosion::OnAwake()
{
	if (_explosion_crystal->IsActive()) 
	{
		_explosion_crystal->SetActive(false);
	}
	enemies = Scenes::GetEnemiesContainer();
	_stats = game_object->GetComponent<Stats>();
	_audio_source = game_object->GetComponent<ComponentAudioSource>();
	transform = game_object->GetTransform();
	ResetCrystal();
}

void Hachiko::Scripting::CrystalExplosion::OnUpdate()
{
	if (!_stats)
	{
		return;
	}
	
	if (!_stats->IsAlive())
	{
		ComponentAnimation* explosion_anim = _explosion_crystal->GetComponent<ComponentAnimation>();
		if (explosion_anim && explosion_anim->IsAnimationStopped() && visible)
		{
			SetVisible(false);
			return;
		}


		if (!visible && _current_regen_time == 0.f)
		{
			RegenCrystal();
		}
		_current_regen_time += Time::DeltaTime();

		if (_current_regen_time >= _regen_time)
		{
			ResetCrystal();
		}
	}

	if (is_exploding)
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

	if (_explosive_crystal)
	{
		CheckRadiusExplosion();
	}
}

void Hachiko::Scripting::CrystalExplosion::StartExplosion()
{
	is_exploding = true;

	for (GameObject* child : _explosion_effect->children)
	{
		child->GetComponent<ComponentBillboard>()->Restart();
		child->SetActive(true);
	}
}

void Hachiko::Scripting::CrystalExplosion::CheckRadiusExplosion()
{
	if (_detecting_radius >= game_object->GetTransform()->GetGlobalPosition().Distance(Scenes::GetPlayer()->GetTransform()->GetGlobalPosition()))
	{
		RegisterHit(_stats->_max_hp);
	}
}

void Hachiko::Scripting::CrystalExplosion::ExplodeCrystal()
{
	is_exploding = false;
	
	std::vector<GameObject*> check_hit = {};

	if (enemies != nullptr)
	{
		for (GameObject* enemy_pack : enemies->children)
		{
			check_hit.insert(check_hit.end(), enemy_pack->children.begin(), enemy_pack->children.end());
		}
	}

	check_hit.push_back(Scenes::GetPlayer());

	std::vector<GameObject*> elements_hit = {};

	for (int i = 0; i < check_hit.size(); ++i)
	{
		if (check_hit[i]->active &&
			_explosion_radius >= transform->GetGlobalPosition().Distance(check_hit[i]->GetTransform()->GetGlobalPosition()))
		{
			elements_hit.push_back(check_hit[i]);
		}
	}

	for (Hachiko::GameObject* element : elements_hit)
	{
		EnemyController* enemy_controller = element->GetComponent<EnemyController>();
		PlayerController* player_controller = element->GetComponent<PlayerController>();

		float3 relative_dir = element->GetTransform()->GetGlobalPosition() - transform->GetGlobalPosition();
		relative_dir.y = 0.0f;

		if (enemy_controller != nullptr)
		{
			enemy_controller->RegisterHit(_stats->_attack_power, relative_dir.Normalized(), 0.7f, false, false);
		}

		if (player_controller != nullptr)
		{
			player_controller->RegisterHit(_stats->_attack_power, true, relative_dir.Normalized());
		}
	}
	

	for (GameObject* child : _explosion_effect->children)
	{
		child->SetActive(false);
	}
}

void Hachiko::Scripting::CrystalExplosion::RegisterHit(int damage)
{
	if (!_stats)	return;

	_stats->ReceiveDamage(damage);


	if (!_stats->IsAlive() && !is_destroyed)
	{
		if (_explosive_crystal)
		{
			StartExplosion();
		}
		else
		{
			DestroyCrystal();
		}
	}
}

void Hachiko::Scripting::CrystalExplosion::SetVisible(bool v)
{
	visible = v;
	game_object->SetVisible(v, true);
}

void Hachiko::Scripting::CrystalExplosion::ResetCrystal()
{
	is_exploding = false;
	is_destroyed = false;
	_stats->SetHealth(1);
	_static_crystal->SetActive(true);
	_explosion_crystal->SetActive(false);
	SetVisible(true);
	_current_explosion_timer = 0.f;
	_current_regen_time = 0.f;

	if (_explosion_indicator_helper)
	{
		_explosion_indicator_helper->SetActive(false);
	}

	if (_explosion_effect)
	{
		for (GameObject* child : _explosion_effect->children)
		{
			child->SetActive(false);
		}
	}

	ComponentObstacle* obstacle = game_object->GetComponent<ComponentObstacle>();
	if (obstacle)
	{
		obstacle->AddObstacle();
	}

	ComponentAnimation* exploding_animation = _explosion_crystal->GetComponent<ComponentAnimation>();
	if (exploding_animation)
	{
		exploding_animation->StopAnimating();
		exploding_animation->ResetState();
	}
}

void Hachiko::Scripting::CrystalExplosion::DestroyCrystal()
{
	_audio_source->PostEvent(Sounds::CRYSTAL);
	_static_crystal->SetActive(false);
	_explosion_crystal->SetActive(true);
	ComponentObstacle* obstacle = game_object->GetComponent<ComponentObstacle>();
	if (obstacle)
	{
		obstacle->RemoveObstacle();
	}
	ComponentAnimation* exploding_animation = _explosion_crystal->GetComponent<ComponentAnimation>();
	if (exploding_animation)
	{
		is_destroyed = true;
		exploding_animation->StartAnimating();
	}
}

void Hachiko::Scripting::CrystalExplosion::RegenCrystal()
{
	_current_regen_time = 0.f;
	_static_crystal->SetActive(true);
	_static_crystal->SetVisible(true, false);
	ComponentAnimation* regen_animation = _static_crystal->GetComponent<ComponentAnimation>();
	if (regen_animation)
	{
		regen_animation->ResetState();
		regen_animation->StartAnimating();
	}
}
