#include "scriptingUtil/gameplaypch.h"


#include "constants/Scenes.h"
#include "entities/crystals/CrystalExplosion.h"
#include "entities/enemies/EnemyController.h"
#include "entities/player/PlayerController.h"
#include "entities/player/CombatManager.h"

// TODO: Delete this include:
#include <modules/ModuleSceneManager.h>

Hachiko::Scripting::CombatManager::CombatManager(GameObject* game_object)
	: Script(game_object, "CombatManager")
{
}

Hachiko::Scripting::CombatManager::~CombatManager()
{
}

void Hachiko::Scripting::CombatManager::OnAwake()
{
	// Spawn bullet (Passing the prefab can be improved)
	//UID bullet_uid = 14999767472668584259;
	UID bullet_uid = 329775617723033719;
	_bullets = SceneManagement::Instantiate(bullet_uid, game_object, _max_bullets);

	_bullet_stats.reserve(_bullets.size());

	for (unsigned i = 0; i < _bullets.size(); i++)
	{
		GameObject* bullet = _bullets[i];
		bullet->SetActive(false);
		_bullet_stats.push_back(BulletStats());
	}

	_player = Scenes::GetPlayer();
	_enemy_packs_container = Scenes::GetEnemiesContainer();

	_charge_particles = _charge_vfx->GetComponent<ComponentParticleSystem>();
	_shot_particles = _shot_vfx->GetComponent<ComponentParticleSystem>();

	if (_player)
	{
		_player_controller = _player->GetComponent<PlayerController>();
	}

	SerializeEnemyPacks();

}

void Hachiko::Scripting::CombatManager::OnUpdate()
{
	if (!game_object->IsActive())
	{
		return;
	}

	RunBulletSimulation();
}

int Hachiko::Scripting::CombatManager::PlayerConeAttack(const float4x4& origin, const AttackStats& attack_stats)
{
	float3 attack_dir = origin.WorldZ().Normalized();
	float min_dot_product = std::cos(math::DegToRad(attack_stats.width));
	
	int hit = 0;
	hit += ProcessAgentsCone(origin.Col3(3), attack_dir, min_dot_product, attack_stats.range, attack_stats, true);
	hit += ProcessObstaclesCone(origin.Col3(3), attack_dir, min_dot_product, attack_stats.range, attack_stats);

	return hit;
}

int Hachiko::Scripting::CombatManager::PlayerRectangleAttack(const float4x4& origin, const AttackStats& attack_stats)
{
	OBB hitbox = CreateAttackHitbox(origin, attack_stats);
	int hit = 0;

	hit +=  ProcessAgentsOBB(hitbox, attack_stats, origin.Col3(3), true);
	hit +=  ProcessObstaclesOBB(hitbox, attack_stats);
	
	return hit;
}

int Hachiko::Scripting::CombatManager::PlayerCircleAttack(const float4x4& origin, const AttackStats& attack_stats)
{
	int hit = 0;

	hit +=  ProcessAgentsCircle(origin.Col3(3), attack_stats, true);
	hit +=  ProcessObstaclesCircle(origin.Col3(3), attack_stats);

	return hit;
}

int Hachiko::Scripting::CombatManager::PlayerMeleeAttack(const float4x4& origin, const AttackStats& attack_stats)
{
	switch (attack_stats.type)
	{
	case AttackType::RECTANGLE:
		return PlayerRectangleAttack(origin, attack_stats);
	case AttackType::CONE:
		return PlayerConeAttack(origin, attack_stats);
	default:
		return false;
	}
}

int Hachiko::Scripting::CombatManager::EnemyConeAttack(const float4x4& origin, const AttackStats& attack_stats)
{
	float3 attack_dir = origin.WorldZ().Normalized();
	float min_dot_product = std::cos(math::DegToRad(attack_stats.width));

	int hit = 0;
	hit += ProcessPlayerCone(origin.Col3(3), attack_dir, min_dot_product, attack_stats.range, attack_stats);
	
	return hit;
}

int Hachiko::Scripting::CombatManager::EnemyRectangleAttack(const float4x4& origin, const AttackStats& attack_stats)
{
	OBB hitbox = CreateAttackHitbox(origin, attack_stats);
	int hit = 0;

	hit += ProcessPlayerOBB(hitbox, attack_stats, origin.Col3(3));

	return hit;
}

int Hachiko::Scripting::CombatManager::EnemyCircleAttack(const float4x4& origin, const AttackStats& attack_stats)
{
	OBB hitbox = CreateAttackHitbox(origin, attack_stats);
	int hit = 0;

	hit += ProcessPlayerCircle(origin.Col3(3), attack_stats);

	return hit;
}

int Hachiko::Scripting::CombatManager::EnemyMeleeAttack(const float4x4& origin, const AttackStats& attack_stats)
{
	switch (attack_stats.type)
	{
	case AttackType::RECTANGLE:
		return EnemyRectangleAttack(origin, attack_stats);
	case AttackType::CONE:
		return EnemyConeAttack(origin, attack_stats);
	case AttackType::CIRCLE:
		return EnemyCircleAttack(origin, attack_stats);
	default:
		return 0;
	}
}

void Hachiko::Scripting::CombatManager::RunBulletSimulation()
{
	for (unsigned i = 0; i < _bullets.size(); i++)
	{
		GameObject* bullet = _bullets[i];
		// Check if its time to destroy
		BulletStats& stats = _bullet_stats[i];
		if (!stats.alive)
		{
			// Already dead bullet
			continue;
		}
		if (stats.lifetime <= stats.elapsed_lifetime)
		{
			// Disable if lifetime is over
			DeactivateBullet(i);
			continue;
		}
		if (stats.current_charge < stats.charge_time)
		{
			// Do not launch yet
			stats.current_charge += Time::DeltaTime();
			//bullet->GetTransform()->SetGlobalScale(float3(stats.GetChargedPercent()));
			SetBulletTrajectory(i);
			stats.update_ui = true;
			continue;
		}

		if (stats.current_charge >= stats.charge_time && stats.update_ui)	// When a bullet starts moving only update ui once
		{
			bullet->SetActive(true);
			_charge_particles->Stop();
			_shot_particles->Play();
			_shot_particles->Restart();
			if (!_player_controller)	return;

			_player_controller->UpdateAmmoUI();
			stats.update_ui = false;
		}
		
		// Move bullet forward
		UpdateBulletTrajectory(i);
		// Check if it collides with an enemy
		if (CheckBulletCollisions(i))
		{
			DeactivateBullet(i);
			continue;
		}

		// If no collision just lower lifetime
		stats.elapsed_lifetime += Time::DeltaTime();
	}
}

void Hachiko::Scripting::CombatManager::UpdateBulletTrajectory(unsigned bullet_idx)
{
	GameObject* bullet = _bullets[bullet_idx];
	BulletStats& stats = _bullet_stats[bullet_idx];
	ComponentTransform* bullet_transform = bullet->GetTransform();
	float3 current_position = bullet_transform->GetGlobalPosition();
	stats.prev_position = current_position;
	float3 new_position = current_position + stats.direction * stats.speed * Time::DeltaTime();
	bullet_transform->SetGlobalPosition(new_position);
}

bool Hachiko::Scripting::CombatManager::CheckBulletCollisions(unsigned bullet_idx)
{
	// Process hit (if any) for closest enemy or obstacle

	BulletStats& stats = _bullet_stats[bullet_idx];
	GameObject* bullet = _bullets[bullet_idx];
	LineSegment trajectory = LineSegment(stats.prev_position, bullet->GetTransform()->GetGlobalPosition());

	float closest_enemy_hit = FLT_MAX;
	EnemyController* hit_enemy = FindBulletClosestEnemyHit(bullet, stats.size, trajectory, closest_enemy_hit);
	float closest_obstacle_hit = FLT_MAX;
	GameObject* hit_obstacle = FindBulletClosestObstacleHit(bullet, stats.size, trajectory, closest_obstacle_hit);
	
	if (closest_enemy_hit < closest_obstacle_hit)
	{
		if (hit_enemy && hit_enemy->IsAlive())
		{
			float3 knockback_dir = hit_enemy->GetGameObject()->GetTransform()->GetGlobalPosition() - bullet->GetTransform()->GetGlobalPosition();
			HitEnemy(hit_enemy, stats.damage, 0.f, knockback_dir, true, true);
			return true;
		}
	}

	if (hit_obstacle)
	{
		HitObstacle(hit_obstacle, stats.damage);
		return true;
	}

	return false;
}

void Hachiko::Scripting::CombatManager::ActivateBullet(unsigned bullet_idx)
{
	_bullet_stats[bullet_idx].alive = true;
	_bullet_stats[bullet_idx].current_charge = 0.f;
	_bullet_stats[bullet_idx].elapsed_lifetime = 0.f;

	_bullets[bullet_idx]->GetTransform()->SetGlobalScale(float3(_bullet_stats[bullet_idx].size));
	//_bullets[bullet_idx]->SetActive(true);
}

void Hachiko::Scripting::CombatManager::DeactivateBullet(unsigned bullet_idx)
{
	_bullet_stats[bullet_idx].alive = false;
	_bullets[bullet_idx]->SetActive(false);
}

void Hachiko::Scripting::CombatManager::SetBulletTrajectory(unsigned bullet_idx)
{
	GameObject* bullet = _bullets[bullet_idx];
	BulletStats& stats = _bullet_stats[bullet_idx];
	ComponentTransform* bullet_transform = bullet->GetTransform();
	const float bullet_offset = 1.25f;
	const float3 bullet_vertical_offset = float3(.0f, 1.0f, .0f);
	float3 emitter_direction = stats.emitter_transform->GetFront().Normalized();
	float3 emitter_position = (stats.emitter_transform->GetGlobalPosition() + emitter_direction * bullet_offset) + bullet_vertical_offset;
	Quat emitter_rotation = stats.emitter_transform->GetGlobalRotation();
	
	stats.prev_position = emitter_position;

	stats.direction = emitter_direction;
	bullet_transform->SetGlobalPosition(emitter_position);
	bullet_transform->SetGlobalRotation(emitter_rotation);
	// If the bullet is out of culling nothing its updating its transform unless selected in engine
	bullet_transform->GetLocalPosition();
}

Hachiko::Scripting::EnemyController* Hachiko::Scripting::CombatManager::FindBulletClosestEnemyHit(GameObject* bullet, float bullet_size, LineSegment& trajectory, float& closest_hit)
{
	EnemyController* hit_target = nullptr;
	
	if (!_enemy_packs_container)
	{
		return nullptr;
	}

	float3 bullet_position = bullet->GetTransform()->GetGlobalPosition();


	std::vector<GameObject*>& enemy_packs = _enemy_packs_container->children;
	for (int i = 0; i < enemy_packs.size(); ++i)
	{
		GameObject* pack = enemy_packs[i];
		if (!pack->IsActive())
		{
			continue;
		}

		std::vector<GameObject*>& enemies = pack->children;
		for (int i = 0; i < enemies.size(); ++i)
		{
			GameObject* enemy = enemies[i];
			if (!enemy->IsActive())
			{
				continue;
			}

			ComponentAgent* agent = enemy->GetComponent<ComponentAgent>();
			if (!agent)
			{
				continue;
			}

			float3 enemy_position = enemy->GetTransform()->GetGlobalPosition();
			float enemy_radius = agent->GetRadius();
			Sphere hitbox = Sphere(enemy_position, enemy_radius + bullet_size);

			if (trajectory.Intersects(hitbox))
			{
				EnemyController* enemy_controller = enemy->GetComponent<EnemyController>();
				float hit_distance = bullet_position.Distance(enemy_position);
				if (enemy_controller && hit_distance < closest_hit)
				{

					closest_hit = hit_distance;
					hit_target = enemy_controller;
				}
			}
		}
	}
	return hit_target;
}

Hachiko::GameObject* Hachiko::Scripting::CombatManager::FindBulletClosestObstacleHit(GameObject* bullet, float bullet_size, LineSegment& trajectory, float& closest_hit)
{
	GameObject* hit_target = nullptr;

	GameObject* obstacle_container = game_object->scene_owner->GetRoot()->GetFirstChildWithName("Crystals");
	if (!obstacle_container)
	{
		return nullptr;
	}

	float3 bullet_position = bullet->GetTransform()->GetGlobalPosition();


	std::vector<GameObject*> obstacles = obstacle_container->children;
	for (int i = 0; i < obstacles.size(); ++i)
	{
		GameObject* obstacle = obstacles[i];
		if (!obstacle->IsActive())
		{
			continue;
		}

		ComponentObstacle* obstacle_component = obstacle->GetComponent<ComponentObstacle>();
		if (!obstacle_component)
		{
			continue;
		}

		float3 obstacle_position = obstacle->GetTransform()->GetGlobalPosition();
		// Cylinder obstacles ignore z
		float obstacle_radius = obstacle_component->GetSize().x;
		Sphere hitbox = Sphere(obstacle_position, obstacle_radius + bullet_size);

		if (obstacle->active && obstacle_component->IsActive() && trajectory.Intersects(hitbox))
		{
			float hit_distance = bullet_position.Distance(obstacle_position);
			if (hit_distance < closest_hit)
			{
				closest_hit = hit_distance;
				hit_target = obstacle;
			}
		}
	}
	return hit_target;
}

void Hachiko::Scripting::CombatManager::SerializeEnemyPacks()
{
	for (unsigned i = 0; i < _enemy_packs_container->children.size(); ++i)
	{
		GameObject* pack = _enemy_packs_container->children[i];
		_initial_transforms.push_back(std::vector<float4x4>());
		for (unsigned j = 0; j < pack->children.size(); ++j)
		{
			GameObject* enemy = pack->children[j];
			_initial_transforms[i].push_back(enemy->GetTransform()->GetGlobalMatrix());
		}
	}
}

int Hachiko::Scripting::CombatManager::ShootBullet(ComponentTransform* emitter_transform, BulletStats new_stats)
{
	_charge_particles->Play();
	_charge_particles->Restart();

	// We use a pointer to represent when there is no bullet shoot
	for (unsigned i = 0; i < _bullets.size(); i++)
	{
		BulletStats& stats = _bullet_stats[i];
		if (stats.alive)
		{
			// Bullet already being used
			continue;
		}
		// Update stats with the new bullet stats
		stats = new_stats;
		stats.emitter_transform = emitter_transform;
		SetBulletTrajectory(i);

		ActivateBullet(i);
		// After a bullet is generated exit
		return static_cast<int>(i);
	}
	return -1;
}

void Hachiko::Scripting::CombatManager::StopBullet(unsigned bullet_index)
{
	_charge_particles->Stop();
	DeactivateBullet(bullet_index);
}

const Hachiko::Scripting::CombatManager::BulletStats& Hachiko::Scripting::CombatManager::GetBulletStats(unsigned bullet_idx) const
{
	// TODO: insert return statement here
	return _bullet_stats[bullet_idx];
}

OBB Hachiko::Scripting::CombatManager::CreateAttackHitbox(const float4x4& origin, const AttackStats& attack_stats) const
{
	return OBB(origin.Col3(3), float3(attack_stats.width / 2.f, 1.f, attack_stats.range / 2.f), origin.WorldX().Normalized(), origin.WorldY().Normalized(), origin.WorldZ().Normalized());
}

bool Hachiko::Scripting::CombatManager::IsPackDead(GameObject* pack) const
{
	for (GameObject* enemy : pack->children)
	{
		EnemyController* enemy_controller = enemy->GetComponent<EnemyController>();
		if (enemy_controller && enemy_controller->IsAlive())
		{
			return false;
		}
	}
	return true;
}

bool Hachiko::Scripting::CombatManager::IsPackDead(unsigned pack_idx) const
{
	return IsPackDead(_enemy_packs_container->children[pack_idx]);
}

void Hachiko::Scripting::CombatManager::ResetEnemyPack(GameObject* pack, bool is_gauntlet)
{
	// Make sure that pack exist in serialization
	auto it = std::find(_enemy_packs_container->children.begin(), _enemy_packs_container->children.end(), pack);

	if (it == _enemy_packs_container->children.end()) return;
	
	unsigned pack_idx = std::distance(_enemy_packs_container->children.begin(), it);

	GameObject* enemies = *it;	

	for (int i = 0; i < enemies->children.size(); ++i)
	{
		ComponentAgent* agent = enemies->children[i]->GetComponent<ComponentAgent>();
		if (agent)
		{
			agent->RemoveFromCrowd();
		}
		EnemyController* enemy_controller = enemies->children[i]->GetComponent<EnemyController>();
		if (enemy_controller)
		{
			enemy_controller->SetIsFromGauntlet(is_gauntlet);
			enemy_controller->ResetEnemy();
			enemy_controller->ResetEnemyPosition();
		}		
		if (agent)
		{
			agent->AddToCrowd();
		}
	}
}

void Hachiko::Scripting::CombatManager::ActivateEnemyPack(GameObject* pack)
{
	pack->SetActive(true);
}

void Hachiko::Scripting::CombatManager::DeactivateEnemyPack(GameObject* pack)
{
	pack->SetActive(false);
}


int Hachiko::Scripting::CombatManager::ProcessAgentsCone(const float3& attack_source_pos, const float3& attack_dir, float min_dot_prod, float hit_distance, const AttackStats& attack_stats, bool is_from_player)
{
	GameObject* agent_container = game_object->scene_owner->GetRoot()->GetFirstChildWithName("Enemies");

	if (!agent_container)
	{
		return 0;
	}

	int hit = 0;
	std::vector<GameObject*>& enemy_packs = _enemy_packs_container->children;
	for (int i = 0; i < enemy_packs.size(); ++i)
	{
		GameObject* pack = enemy_packs[i];
		if (!pack->IsActive())
		{
			continue;
		}

		std::vector<GameObject*>& enemies = pack->children;
		for (int i = 0; i < enemies.size(); ++i)
		{
			GameObject* enemy = enemies[i];
			if (!enemy->IsActive())
			{
				continue;
			}

			// Col3(3) Is the world position without doing decompose
			if (ConeHitsAgent(enemy, attack_source_pos, attack_dir, min_dot_prod, hit_distance))
			{
				// Hit enemy here
				EnemyController* enemy_controller = enemy->GetComponent<EnemyController>();
				if (enemy_controller && enemy_controller->IsAlive())
				{
					hit++;
					HitEnemy(enemy_controller, attack_stats.damage, attack_stats.knockback_distance, attack_dir, is_from_player, false);
				}
			}
		}
	}
	return hit;
}

int Hachiko::Scripting::CombatManager::ProcessObstaclesCone(const float3& attack_source_pos, const float3& attack_dir, float min_dot_prod, float hit_distance, const AttackStats& attack_stats)
{
	GameObject* obstacle_container = game_object->scene_owner->GetRoot()->GetFirstChildWithName("Crystals");
	if (!obstacle_container)
	{
		return 0;
	}
	
	int hit = 0;
	std::vector<GameObject*> obstacles = obstacle_container->children;
	for (int i = 0; i < obstacles.size(); ++i)
	{
		GameObject* obstacle = obstacles[i];
		if (!obstacle->IsActive())
		{
			continue;
		}

		// Col3(3) Is the world position without doing decompose
		if (ConeHitsObstacle(obstacle, attack_source_pos, attack_dir, min_dot_prod, hit_distance))
		{
			CrystalExplosion* crystal_controller = obstacle->GetComponent<CrystalExplosion>();
			if (crystal_controller && crystal_controller->isAlive())
			{
				hit++;
				HitObstacle(obstacle, attack_stats.damage);
			}
		}
	}
	return hit;
}

int Hachiko::Scripting::CombatManager::ProcessPlayerCone(const float3& attack_source_pos, const float3& attack_dir, float min_dot_prod, float hit_distance, const AttackStats& attack_stats)
{
	if (!_player)
	{
		return 0;
	}

	int hit = 0;

	if (ConeHitsPlayer(attack_source_pos, attack_dir, min_dot_prod, hit_distance))
	{
		// Hit enemy here
		if (_player->GetComponent<PlayerController>()->IsAlive())
		{
			hit++;
			//TODO: add knockback
			HitPlayer(attack_stats.damage, attack_stats.knockback_distance, attack_dir);
		}
	}
	return hit;
}

int Hachiko::Scripting::CombatManager::ProcessAgentsOBB(const OBB& attack_box, const AttackStats& attack_stats, const float3& attack_source_pos, bool is_from_player)
{
	GameObject* agent_container = game_object->scene_owner->GetRoot()->GetFirstChildWithName("Enemies");

	if (!agent_container)
	{
		return 0;
	}

	int hit = 0;
	std::vector<GameObject*>& enemy_packs = _enemy_packs_container->children;
	for (int i = 0; i < enemy_packs.size(); ++i)
	{
		GameObject* pack = enemy_packs[i];
		if (!pack->IsActive())
		{
			continue;
		}

		std::vector<GameObject*>& enemies = pack->children;
		for (int i = 0; i < enemies.size(); ++i)
		{
			GameObject* enemy = enemies[i];
			if (!enemy->IsActive())
			{
				continue;
			}

			if (OBBHitsAgent(enemy, attack_box))
			{
				// Hit enemy here
				EnemyController* enemy_controller = enemy->GetComponent<EnemyController>();
				if (enemy_controller && enemy_controller->IsAlive())
				{
					hit++;
					float3 knockback_dir = enemy->GetTransform()->GetGlobalPosition() - attack_source_pos;
					knockback_dir.y = 0;
					knockback_dir.Normalize();
					HitEnemy(enemy_controller, attack_stats.damage, attack_stats.knockback_distance, knockback_dir, is_from_player, false);
				}
			}
		}
	}
	return hit;
}

int Hachiko::Scripting::CombatManager::ProcessObstaclesOBB(const OBB& attack_box, const AttackStats& attack_stats)
{
	GameObject* obstacle_container = game_object->scene_owner->GetRoot()->GetFirstChildWithName("Crystals");
	if (!obstacle_container)
	{
		return 0;
	}

	int hit = 0;
	std::vector<GameObject*> obstacles = obstacle_container->children;
	for (int i = 0; i < obstacles.size(); ++i)
	{
		GameObject* obstacle = obstacles[i];
		if (!obstacle->IsActive())
		{
			continue;
		}

		if (OBBHitsObstacle(obstacle, attack_box))
		{
			// Hit enemy here
			CrystalExplosion* crystal_controller = obstacle->GetComponent<CrystalExplosion>();
			if (crystal_controller && crystal_controller->isAlive())
			{
				hit++;
				HitObstacle(obstacle, attack_stats.damage);
			}
		}
	}
	return hit;
}

int Hachiko::Scripting::CombatManager::ProcessPlayerOBB(const OBB& attack_box, const AttackStats& attack_stats, const float3& attack_source_pos)
{
	if (!_player)
	{
		return 0;
	}

	int hit = 0;

	if (OBBHitsPlayer(attack_box))
	{
		// Hit enemy here
		if (_player->GetComponent<PlayerController>()->IsAlive())
		{
			hit++;
			float3 knockback_dir = _player->GetTransform()->GetGlobalPosition() - attack_source_pos;
			knockback_dir.y = 0;
			knockback_dir.Normalize();
			HitPlayer(attack_stats.damage, attack_stats.knockback_distance, knockback_dir);
		}
	}
	return hit;
}

int Hachiko::Scripting::CombatManager::ProcessAgentsCircle(const float3& attack_source_pos, const AttackStats& attack_stats, bool is_from_player)
{
	GameObject* agent_container = game_object->scene_owner->GetRoot()->GetFirstChildWithName("Enemies");

	if (!agent_container)
	{
		return 0;
	}

	int hit = 0;
	std::vector<GameObject*>& enemy_packs = _enemy_packs_container->children;
	for (int i = 0; i < enemy_packs.size(); ++i)
	{
		GameObject* pack = enemy_packs[i];
		if (!pack->IsActive())
		{
			continue;
		}

		std::vector<GameObject*>& enemies = pack->children;
		for (int i = 0; i < enemies.size(); ++i)
		{
			GameObject* enemy = enemies[i];
			if (!enemy->IsActive())
			{
				continue;
			}

			if (CircleHitsAgent(enemy, attack_source_pos, attack_stats.range))
			{
				// Hit enemy here
				EnemyController* enemy_controller = enemy->GetComponent<EnemyController>();
				if (enemy_controller && enemy_controller->IsAlive())
				{
					hit++;
					float3 knockback_dir = enemy->GetTransform()->GetGlobalPosition() - attack_source_pos;
					knockback_dir.y = 0;
					knockback_dir.Normalize();
					HitEnemy(enemy_controller, attack_stats.damage, attack_stats.knockback_distance, knockback_dir, is_from_player);
				}
			}
		}
	}
	return hit;
}

int Hachiko::Scripting::CombatManager::ProcessObstaclesCircle(const float3& attack_source_pos, const AttackStats& attack_stats)
{
	GameObject* obstacle_container = game_object->scene_owner->GetRoot()->GetFirstChildWithName("Crystals");
	if (!obstacle_container)
	{
		return 0;
	}

	int hit = 0;
	std::vector<GameObject*> obstacles = obstacle_container->children;
	for (int i = 0; i < obstacles.size(); ++i)
	{
		GameObject* obstacle = obstacles[i];
		if (!obstacle->IsActive())
		{
			continue;
		}

		if (CircleHitsAgent(obstacle, attack_source_pos, attack_stats.range))
		{
			// Hit enemy here
			CrystalExplosion* crystal_controller = obstacle->GetComponent<CrystalExplosion>();
			if (crystal_controller && crystal_controller->isAlive())
			{
				hit++;
				HitObstacle(obstacle, attack_stats.damage);
			}
		}
	}
	return hit;
}

int Hachiko::Scripting::CombatManager::ProcessPlayerCircle(const float3& attack_source_pos, const AttackStats& attack_stats)
{
	if (CircleHitsPlayer(attack_source_pos, attack_stats.range))
	{
		float3 knockback_dir = _player->GetTransform()->GetGlobalPosition() - attack_source_pos;
		knockback_dir.y = 0;
		knockback_dir.Normalize();
		HitPlayer(attack_stats.damage, attack_stats.knockback_distance, knockback_dir);
		return 1;
	}
	return 0;
}

bool Hachiko::Scripting::CombatManager::ConeHitsAgent(GameObject* agent_go, const float3& attack_source_pos, const float3& attack_dir, float min_dot_prod, float hit_distance)
{
	ComponentAgent* agent = agent_go->GetComponent<ComponentAgent>();
	if (!agent)
	{
		return false;
	}

	float3 enemy_position = agent_go->GetTransform()->GetGlobalPosition();
	float3 source_to_enemy_dir = (enemy_position - attack_source_pos).Normalized();
	float enemy_radius = agent->GetRadius();

	bool valid_angle = attack_dir.Dot(source_to_enemy_dir) >= min_dot_prod;
	// Take into account the agent radius
	if (!valid_angle)
	{
		return false;
	}

	float enemy_distance = enemy_position.Distance(attack_source_pos) - enemy_radius;
	bool valid_distance = enemy_distance <= hit_distance;

	return valid_distance;
}

bool Hachiko::Scripting::CombatManager::ConeHitsObstacle(GameObject* obstacle_go, const float3& attack_source_pos, const float3& attack_dir, float min_dot_prod, float hit_distance)
{
	ComponentObstacle* obstacle = obstacle_go->GetComponent<ComponentObstacle>();
	if (!obstacle)
	{
		return false;
	}

	float3 obstacle_position = obstacle_go->GetTransform()->GetGlobalPosition();
	float3 source_to_obstacle_dir = (obstacle_position - attack_source_pos).Normalized();
	// Assume obstacle is a cylinder for now
	float obstacle_radius = obstacle->GetSize().x;

	bool valid_angle = attack_dir.Dot(source_to_obstacle_dir) >= min_dot_prod;
	// Take into account the agent radius
	if (!valid_angle)
	{
		return false;
	}

	float obstacle_distance = obstacle_position.Distance(attack_source_pos) - obstacle_radius;
	bool valid_distance = obstacle_distance <= hit_distance;

	return valid_distance;
}

bool Hachiko::Scripting::CombatManager::ConeHitsPlayer(const float3& attack_source_pos, const float3& attack_dir, float min_dot_prod, float hit_distance)
{
	if (!_player)
	{
		return false;
	}

	float3 player_position = _player->GetTransform()->GetGlobalPosition();
	float3 source_to_obstacle_dir = (player_position - attack_source_pos).Normalized();
	
	float player_radius = 0.25;

	bool valid_angle = attack_dir.Dot(source_to_obstacle_dir) >= min_dot_prod;
	// Take into account the agent radius
	if (!valid_angle)
	{
		return false;
	}

	float player_distance = player_position.Distance(attack_source_pos) - player_radius;
	bool valid_distance = player_distance <= hit_distance;

	return valid_distance;
}

bool Hachiko::Scripting::CombatManager::OBBHitsAgent(GameObject* agent_go, const OBB& attack_box)
{
	ComponentAgent* agent = agent_go->GetComponent<ComponentAgent>();
	if (!agent)
	{
		return false;
	}

	float3 enemy_position = agent_go->GetTransform()->GetGlobalPosition();
	float enemy_radius = agent->GetRadius();
	Sphere hitbox = Sphere(enemy_position, enemy_radius);

	return attack_box.Intersects(hitbox);
}

bool Hachiko::Scripting::CombatManager::OBBHitsObstacle(GameObject* obstacle_go, const OBB& attack_box)
{
	ComponentObstacle* obstacle_cpomponent = obstacle_go->GetComponent<ComponentObstacle>();
	if (!obstacle_cpomponent)
	{
		return false;
	}

	float3 enemy_position = obstacle_go->GetTransform()->GetGlobalPosition();
	// Assume it is in cylinder mode
	float enemy_radius = obstacle_cpomponent->GetSize().x;
	Sphere hitbox = Sphere(enemy_position, enemy_radius);

	return attack_box.Intersects(hitbox);
}

bool Hachiko::Scripting::CombatManager::OBBHitsPlayer(const OBB& attack_box)
{
	float3 player_position = _player->GetTransform()->GetGlobalPosition();
	float player_radius = 0.25f; // Small radius for the player to avoid being hit by something that visualy didnt hit you
	Sphere hitbox = Sphere(player_position, player_radius);

	return attack_box.Intersects(hitbox);
}

bool Hachiko::Scripting::CombatManager::CircleHitsAgent(GameObject* agent_go, const float3& attack_source_pos, float radius)
{
	if (attack_source_pos.Distance(agent_go->GetTransform()->GetGlobalPosition()) <= radius)
	{
		return true;
	}
	return false;
}

bool Hachiko::Scripting::CombatManager::CircleHitsObstacle(GameObject* obstacle_go, const float3& attack_source_pos, float radius)
{
	if (attack_source_pos.Distance(obstacle_go->GetTransform()->GetGlobalPosition()) <= radius)
	{
		return true;
	}
	return false;
}

bool Hachiko::Scripting::CombatManager::CircleHitsPlayer(const float3& attack_source_pos, float radius)
{
	if (attack_source_pos.Distance(_player->GetTransform()->GetGlobalPosition()) <= radius)
	{
		return true;
	}
	return false;
}

void Hachiko::Scripting::CombatManager::HitObstacle(GameObject* obstacle, float damage)
{
	CrystalExplosion* is_crystal_explotion = obstacle->GetComponent<CrystalExplosion>();
	if (is_crystal_explotion) {
		is_crystal_explotion->RegisterHit(damage);
	}
	else // Case is a platform crystal trigger
	{
		obstacle->GetComponent<Stats>()->ReceiveDamage(damage);
	}
}

void Hachiko::Scripting::CombatManager::HitEnemy(EnemyController* enemy, int damage, float knockback, float3 knockback_dir, bool is_from_player, bool is_ranged)
{
	enemy->RegisterHit(damage, knockback_dir, knockback, is_from_player, is_ranged);
}

void Hachiko::Scripting::CombatManager::HitPlayer(int damage, float knockback, float3 knockback_dir)
{
	_player->GetComponent<PlayerController>()->RegisterHit(damage, knockback, knockback_dir);
}

float Hachiko::Scripting::CombatManager::BulletStats::GetChargedPercent()
{
	return current_charge /charge_time;
}
