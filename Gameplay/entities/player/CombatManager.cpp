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
	UID bullet_uid = 14999767472668584259;
	_bullets = SceneManagement::Instantiate(bullet_uid, game_object, _max_bullets);

	_bullet_stats.reserve(_bullets.size());

	for (unsigned i = 0; i < _bullets.size(); i++)
	{
		GameObject* bullet = _bullets[i];
		bullet->SetActive(false);
		_bullet_stats.push_back(BulletStats());
	}
}

void Hachiko::Scripting::CombatManager::OnUpdate()
{
	
	if (!game_object->IsActive())
	{
		return;
	}

	RunBulletSimulation();
}

bool Hachiko::Scripting::CombatManager::PlayerConeAttack(const float4x4& origin, const AttackStats& attack_stats)
{
	
	float3 attack_dir = origin.WorldZ().Normalized();
	float min_dot_product = std::cos(math::DegToRad(attack_stats.width));
	
	bool hit = false;
	hit = hit || ProcessAgentsCone(origin.Col3(3), attack_dir, min_dot_product, attack_stats.range, attack_stats);
	hit = hit || ProcessObstaclesCone(origin.Col3(3), attack_dir, min_dot_product, attack_stats.range, attack_stats);
	return hit;
}

bool Hachiko::Scripting::CombatManager::PlayerRectangleAttack(const float4x4& origin, const AttackStats& attack_stats)
{
	OBB hitbox = CreateAttackHitbox(origin, attack_stats);
	bool hit = false;

	hit = hit || ProcessAgentsOBB(hitbox, attack_stats);
	hit = hit || ProcessObstaclesOBB(hitbox, attack_stats);
	
	return false;
}

bool Hachiko::Scripting::CombatManager::PlayerMeleeAttack(const float4x4& origin, const AttackStats& attack_stats)
{
	if (attack_stats.type == AttackType::RECTANGLE)
	{
		return PlayerRectangleAttack(origin, attack_stats);
	}
	return PlayerConeAttack(origin, attack_stats);
}

bool Hachiko::Scripting::CombatManager::EnemyConeAttack(const float4x4& origin, float hit_angle_deg, float hit_distance, const AttackStats& attack_stats)
{
	// To implement after it works with player
	return false;

}

bool Hachiko::Scripting::CombatManager::EnemyRectangleAttack(const float4x4& origin, float width, float length, const AttackStats& attack_stats)
{
	// To implement after it works with player
	return false;

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
			bullet->GetTransform()->SetGlobalScale(float3(stats.GetChargedPercent()));
			SetBulletTrajectory(i);
			continue;
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
	CrystalExplosion* hit_obstacle = FindBulletClosestObstacleHit(bullet, stats.size, trajectory, closest_obstacle_hit);
	
	if (closest_enemy_hit < closest_obstacle_hit)
	{
		if (hit_enemy && hit_enemy->isAlive())
		{
			float3 knockback_dir = hit_enemy->GetGameObject()->GetTransform()->GetGlobalPosition() - bullet->GetTransform()->GetGlobalPosition();
			HitEnemy(hit_enemy, stats.damage, 0.f, knockback_dir);
			return true;
		}
	}

	if (hit_obstacle && hit_obstacle->isAlive())
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
	_bullets[bullet_idx]->SetActive(true);
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
	float3 emitter_direction = stats.emitter_transform->GetFront().Normalized();
	float3 emitter_position = stats.emitter_transform->GetGlobalPosition() + emitter_direction * bullet_offset;
	
	stats.prev_position = emitter_position;

	stats.direction = emitter_direction;
	bullet_transform->SetGlobalPosition(emitter_position);
	// If the bullet is out of culling nothing its updating its transform unless selected in engine
	bullet_transform->GetLocalPosition();
}

Hachiko::Scripting::EnemyController* Hachiko::Scripting::CombatManager::FindBulletClosestEnemyHit(GameObject* bullet, float bullet_size, LineSegment& trajectory, float& closest_hit)
{
	EnemyController* hit_target = nullptr;
	
	GameObject* agent_container = game_object->scene_owner->GetRoot()->GetFirstChildWithName("Enemies");
	if (!agent_container)
	{
		return nullptr;
	}

	float3 bullet_position = bullet->GetTransform()->GetGlobalPosition();


	std::vector<GameObject*> enemies = agent_container->children;
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
	return hit_target;
}

Hachiko::Scripting::CrystalExplosion* Hachiko::Scripting::CombatManager::FindBulletClosestObstacleHit(GameObject* bullet, float bullet_size, LineSegment& trajectory, float& closest_hit)
{
	CrystalExplosion* hit_target = nullptr;

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

		ComponentObstacle* obstacle_cpomponent = obstacle->GetComponent<ComponentObstacle>();
		if (!obstacle_cpomponent)
		{
			continue;
		}
		
		float3 obstacle_position = obstacle->GetTransform()->GetGlobalPosition();
		// Cylinder obstacles ignore z
		float obstacle_radius = obstacle_cpomponent->GetSize().x;
		Sphere hitbox = Sphere(obstacle_position, obstacle_radius + bullet_size);

		if (obstacle->active && trajectory.Intersects(hitbox))
		{
			//float3 dir = enemies_children[i]->GetTransform()->GetGlobalPosition() - bullet->GetTransform()->GetGlobalPosition();
			CrystalExplosion* obstacle_controller = obstacle->GetComponent<CrystalExplosion>();
			float hit_distance = bullet_position.Distance(obstacle_position);
			if (obstacle_controller && hit_distance < closest_hit)
			{
				closest_hit = hit_distance;
				hit_target = obstacle_controller;
			}
		}
	}
	return hit_target;
}

int Hachiko::Scripting::CombatManager::ShootBullet(ComponentTransform* emitter_transform, BulletStats new_stats)
{
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

bool Hachiko::Scripting::CombatManager::ProcessAgentsCone(const float3& attack_source_pos, const float3& attack_dir, float min_dot_prod, float hit_distance, const AttackStats& attack_stats)
{
	GameObject* agent_container = game_object->scene_owner->GetRoot()->GetFirstChildWithName("Enemies");

	if (!agent_container)
	{
		return false;
	}

	bool hit = false;
	std::vector<GameObject*> enemies = agent_container->children;
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
			if (enemy_controller && enemy_controller->isAlive())
			{
				// TODO: Add Knockback
				hit = true;
				HitEnemy(enemy_controller, attack_stats.damage);
			}
		}
	}
	return hit;
}

bool Hachiko::Scripting::CombatManager::ProcessObstaclesCone(const float3& attack_source_pos, const float3& attack_dir, float min_dot_prod, float hit_distance, const AttackStats& attack_stats)
{
	GameObject* obstacle_container = game_object->scene_owner->GetRoot()->GetFirstChildWithName("Crystals");
	if (!obstacle_container)
	{
		return false;
	}
	
	bool hit = false;
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
			// Hit enemy here
			CrystalExplosion* crystal_controller = obstacle->GetComponent<CrystalExplosion>();
			if (crystal_controller && crystal_controller->isAlive())
			{
				hit = true;
				HitObstacle(crystal_controller, attack_stats.damage);
			}
		}
	}
	return hit;
}

bool Hachiko::Scripting::CombatManager::ProcessAgentsOBB(const OBB& attack_box, const AttackStats& attack_stats)
{
	GameObject* agent_container = game_object->scene_owner->GetRoot()->GetFirstChildWithName("Enemies");

	if (!agent_container)
	{
		return false;
	}

	bool hit = false;
	std::vector<GameObject*> enemies = agent_container->children;
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
			if (enemy_controller && enemy_controller->isAlive())
			{
				// TODO: Add Knockback
				hit = true;
				HitEnemy(enemy_controller, attack_stats.damage);
			}
		}
	}
	return hit;
}

bool Hachiko::Scripting::CombatManager::ProcessObstaclesOBB(const OBB& attack_box, const AttackStats& attack_stats)
{
	GameObject* obstacle_container = game_object->scene_owner->GetRoot()->GetFirstChildWithName("Crystals");
	if (!obstacle_container)
	{
		return false;
	}

	bool hit = false;
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
				hit = true;
				HitObstacle(crystal_controller, attack_stats.damage);
			}
		}
	}
	return hit;
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

void Hachiko::Scripting::CombatManager::HitObstacle(CrystalExplosion* obstacle, float damage)
{
	obstacle->RegisterHit(damage);
}

void Hachiko::Scripting::CombatManager::HitEnemy(EnemyController* enemy, float damage, float knockback, float3 knockback_dir)
{
	enemy->RegisterHit(damage, knockback_dir, knockback);
}

float Hachiko::Scripting::CombatManager::BulletStats::GetChargedPercent()
{
	return current_charge /charge_time;
}
