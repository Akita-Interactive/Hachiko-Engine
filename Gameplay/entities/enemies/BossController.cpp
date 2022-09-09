#include "scriptingUtil/gameplaypch.h"

#include "entities/enemies/BossController.h"
#include "entities/Stats.h"
#include "constants/Scenes.h"

Hachiko::Scripting::BossController::BossController(GameObject* game_object)
	: Script(game_object, "BossController")
	, state_value(0)
	, hp_bar_go(nullptr)
	, cocoon_placeholder_go(nullptr)
{
	
}

void Hachiko::Scripting::BossController::OnAwake()
{
	player = Scenes::GetPlayer();
	transform = game_object->GetTransform();
	combat_stats = game_object->GetComponent<Stats>();
	agent = game_object->GetComponent<ComponentAgent>();
	agent->SetMaxSpeed(combat_stats->_move_speed);
	SetUpHpBar();
	// Hp bar starts hidden untill encounter starts
	SetHpBarActive(false);
	if (cocoon_placeholder_go)
	{
		cocoon_placeholder_go->SetActive(false);
	}
	gauntlet = gauntlet_go->GetComponent<GauntletManager>();
}

void Hachiko::Scripting::BossController::OnStart()
{
}

void Hachiko::Scripting::BossController::OnUpdate()
{
	if (Input::IsKeyDown(Input::KeyCode::KEY_H))
	{
		constexpr int player_dmg = 5;
		RegisterHit(player_dmg);
	}
	StateController();
	state_value = static_cast<int>(state);
}

bool Hachiko::Scripting::BossController::IsAlive() const
{
	return combat_stats->IsAlive();
}

void Hachiko::Scripting::BossController::RegisterHit(int dmg)
{
	if (hitable)
	{
		combat_stats->_current_hp -= dmg;
		UpdateHpBar();
	}
}

void Hachiko::Scripting::BossController::UpdateHpBar()
{
	if (hp_bar)
	{
		hp_bar->SetFilledValue(combat_stats->_current_hp);
	}
}


void Hachiko::Scripting::BossController::SetUpHpBar()
{
	if (hp_bar_go)
	{
		hp_bar = hp_bar_go->GetComponent<ComponentProgressBar>();
	}
	if (hp_bar)
	{
		hp_bar->SetMax(combat_stats->_max_hp);
		hp_bar->SetMin(0);
		UpdateHpBar();
	}
}

void Hachiko::Scripting::BossController::SetHpBarActive(bool v)
{
	if (hp_bar_go)
	{
		hp_bar_go->SetActive(v);
	}
}

void Hachiko::Scripting::BossController::StateController()
{
	StateTransitionController();

	switch (state)
	{
	case BossState::WAITING_ENCOUNTER:
		WaitingController();
		break;
	case BossState::STARTING_ENCOUNTER:
		StartEncounterController();
		break;
	case BossState::COMBAT_FORM:
		CombatController();
		break;
	case BossState::CACOON_FORM:
		CacoonController();
		break;
	case BossState::DEAD:
		break;
	}
}

void Hachiko::Scripting::BossController::StateTransitionController()
{
	bool state_changed = state != prev_state;

	if (!state_changed)
	{
		return;
	}

	switch (state)
	{
	case BossState::WAITING_ENCOUNTER:
		// We already start in this state, no transition for now
		// We can reset the scene on death here which will be easier
		break;
	case BossState::STARTING_ENCOUNTER:
		StartEncounter();
		break;
	case BossState::COMBAT_FORM:
		ResetCombatState();
		break;
	case BossState::CACOON_FORM:
		StartCacoon();
		break;
	case BossState::DEAD:
		break;
	}

	prev_state = state;
}

void Hachiko::Scripting::BossController::CombatController()
{
	if (CacoonTrigger())
	{
		state = BossState::CACOON_FORM;
		return;
	}

	if (combat_stats->_current_hp <= 0)
	{
		state = BossState::DEAD;
		return;
	}
	
	CombatTransitionController();

	switch (combat_state)
	{
	case CombatState::IDLE:
		break;
	case CombatState::CHASING:
		ChaseController();
		break;
	case CombatState::ATTACKING:
		MeleeAttackController();
		break;
	case CombatState::SPAWNING_CRYSTALS:
		SpawnCrystalsController();
		break;
	case CombatState::CONSUMING_PARASYTES:
		ConsumeParasytesController();
		break;
	case CombatState::CRYSTAL_JUMP:
		// Do not implement for now
		break;
	}
}

void Hachiko::Scripting::BossController::CombatTransitionController()
{
	bool state_changed = combat_state != prev_combat_state;

	if (!state_changed)
	{
		return;
	}

	switch (combat_state)
	{
	case CombatState::IDLE:
		Chase();
		break;
	case CombatState::CHASING:
		break;
	case CombatState::ATTACKING:
		MeleeAttack();
		break;
	case CombatState::SPAWNING_CRYSTALS:
		SpawnCrystals();
		break;
	case CombatState::CONSUMING_PARASYTES:
		ConsumeParasytes();
		break;
	case CombatState::CRYSTAL_JUMP:
		// Do not implement for now
		break;
	}

	prev_combat_state = combat_state;
}

float Hachiko::Scripting::BossController::GetPlayerDistance()
{
	const float3& player_position = player->GetTransform()->GetGlobalPosition();

	// If player is very cloose change to attack mode
	return transform->GetGlobalPosition().Distance(player_position);
}


void Hachiko::Scripting::BossController::WaitingController()
{
	if (GetPlayerDistance() <= start_encounter_range)
	{
		// Will trigger StartEncounter
		state = BossState::STARTING_ENCOUNTER;
	}
}

void Hachiko::Scripting::BossController::StartEncounter()
{
	SetHpBarActive(true);
}

void Hachiko::Scripting::BossController::StartEncounterController()
{
	// If there is any start sequence manage it here and hold state untill it ends
	// For now it goes to next state instantly
	state = BossState::COMBAT_FORM;
}

void Hachiko::Scripting::BossController::ResetCombatState()
{
	// Reset combat related variables to prevent bugs
	combat_state = CombatState::IDLE;
	prev_combat_state = combat_state;
}

void Hachiko::Scripting::BossController::Die()
{
	// Start death
}

void Hachiko::Scripting::BossController::DieController()
{
}

void Hachiko::Scripting::BossController::StartCacoon()
{
	hitable = false;
	if (cocoon_placeholder_go)
	{
		cocoon_placeholder_go->SetActive(true);
	}
}

void Hachiko::Scripting::BossController::CacoonController()
{
	// Replace with is gauntlet completed
	if (gauntlet->IsFinished() || Input::IsKeyDown(Input::KeyCode::KEY_J))
	{
		FinishCacoon();
	}
}

bool Hachiko::Scripting::BossController::CacoonTrigger()
{
	if (gauntlet_thresholds_percent.empty())
	{
		return false;
	}
	if (gauntlet_thresholds_percent.back() >= static_cast<float>(combat_stats->_current_hp) / combat_stats->_max_hp)
	{
		gauntlet_thresholds_percent.pop_back();
		gauntlet->StartGauntlet();
		return true;
	}
	return false;
}

void Hachiko::Scripting::BossController::FinishCacoon()
{
	if (state != BossState::CACOON_FORM)
	{
		return;
	}
	hitable = true;
	if (cocoon_placeholder_go)
	{
		cocoon_placeholder_go->SetActive(false);
	}
	gauntlet->ResetGauntlet();
	state = BossState::COMBAT_FORM;
}

void Hachiko::Scripting::BossController::Chase()
{
}

void Hachiko::Scripting::BossController::ChaseController()
{
	const float3& player_position = player->GetTransform()->GetGlobalPosition();

	// If player is very cloose change to attack mode
	float player_distance = transform->GetGlobalPosition().Distance(player_position);
	
	if (player_distance <= combat_stats->_attack_range) {
		// We expect transition to attack to start the attack
		combat_state = CombatState::ATTACKING;
		return;
	}

	// If not close enough to attack keep chasing
	// Possible improvement: If player is close enough not chase to not go inside of it
	float3 corrected_position = Navigation::GetCorrectedPosition(player_position, math::float3(10.0f, 10.0f, 10.0f));
	if (corrected_position.x < FLT_MAX)
	{
		target_position = corrected_position;
		transform->LookAtTarget(target_position);
		ComponentAgent* agc = game_object->GetComponent<ComponentAgent>();
		agent->SetTargetPosition(player_position);
	}
}

void Hachiko::Scripting::BossController::MeleeAttack()
{
}

void Hachiko::Scripting::BossController::MeleeAttackController()
{
}

void Hachiko::Scripting::BossController::SpawnCrystals()
{
}

void Hachiko::Scripting::BossController::SpawnCrystalsController()
{
}

void Hachiko::Scripting::BossController::ConsumeParasytes()
{
}

void Hachiko::Scripting::BossController::ConsumeParasytesController()
{
}
