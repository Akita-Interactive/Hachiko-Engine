#include "scriptingUtil/gameplaypch.h"

#include "entities/enemies/BossController.h"
#include "entities/enemies/EnemyController.h"
#include "misc/LaserController.h"
#include "misc/StalagmiteManager.h"
#include "entities/crystals/CrystalExplosion.h"
#include "entities/Stats.h"
#include "constants/Scenes.h"
#include "components/ComponentObstacle.h"
#include "components/ComponentAudioSource.h"
#include "constants/Sounds.h"

#include "entities/player/CombatVisualEffectsPool.h"

Hachiko::Scripting::BossController::BossController(GameObject* game_object)
    : Script(game_object, "BossController")
    , state_value(0)
    , combat_state_value(0)
    , second_phase(false)
    , hp_bar_go(nullptr)
    , crystal_target_go(nullptr)
    , cocoons_parent(nullptr)
    , gauntlet_go(nullptr)
    , _explosive_crystals({})
    , _current_index_crystals(0)
    , camera_transition_speed(2.0f)
    , encounter_start_duration(2.0f)
    , pre_combat_camera_offset(float3(0.f, 13.f, 8.f))
    , _jump_start_delay(0.0f)
    , _jump_ascending_duration(0.0f)
    , _jump_post_ascending_delay(0.0f)
    , _jump_on_highest_point_duration(0.0f)
    , _jump_post_on_highest_point_delay(0.0f)
    , _jump_descending_duration(0.0f)
    , _jump_post_descending_delay(0.0f)
    , _jump_getting_up_duration(0.0f)
    , _jump_skill_delay(0.0f)
    , _jump_skill_duration(0.0f)
    , _jump_post_skill_delay(0.0f)
    , _jump_height(0.0f)
    , _jump_offset(0.0f)
    , _jumping_state(JumpingState::NOT_TRIGGERED)
    , _jump_start_position(float3::zero)
    , _jump_end_position(float3::zero)
    , _crystal_jump_position(float3::zero)
    , _jumping_timer(0.0f)
    , _jump_pattern_index(-1)
    , _current_jumping_mode(JumpingMode::BASIC_ATTACK)
    , _boss_state_text_ui(nullptr)
    , attack_current_cd(0.0f)
    , time_between_enemies(5.0)
    , enemy_pool(nullptr)
    , _rotating_lasers(nullptr)
    , _laser_wall(nullptr)
    , _laser_wall_duration(2.0f)
    , _laser_jump_height(50.0f)
    , damage_effect_duration(1.0f)
    , chasing_time_limit(3.0f)
{
}

void Hachiko::Scripting::BossController::OnAwake()
{
    player = Scenes::GetPlayer();
    level_manager = Scenes::GetLevelManager()->GetComponent<LevelManager>();
    player_camera = Scenes::GetMainCamera()->GetComponent<PlayerCamera>();
    combat_manager = Scenes::GetCombatManager()->GetComponent<CombatManager>();
    combat_visual_effects_pool = Scenes::GetCombatVisualEffectsPool()->GetComponent<CombatVisualEffectsPool>();
    transform = game_object->GetTransform();
    combat_stats = game_object->GetComponent<Stats>();
    agent = game_object->GetComponent<ComponentAgent>();
    agent->SetMaxSpeed(combat_stats->_move_speed);
    agent->RemoveFromCrowd();
    obstacle = game_object->GetComponent<ComponentObstacle>();
    audio_source = game_object->GetComponent<ComponentAudioSource>();
    obstacle->AddObstacle();

	animation = game_object->GetComponent<ComponentAnimation>();

    if (_melee_trail_right != nullptr)
    {
        _melee_trail_right->SetTimeScaleMode(TimeScaleMode::SCALED);
        _weapon_trail_billboard_right = _melee_trail_right->GetComponent<ComponentBillboard>();
    }

	SetUpHpBar();
	// Hp bar starts hidden until encounter starts
	SetHpBarActive(false);
	SetUpCocoon();
	gauntlet = gauntlet_go->GetComponent<GauntletManager>();

	if (enemy_pool)
	{
		for (GameObject* enemy : enemy_pool->children)
		{
			enemies.push_back(enemy->GetComponent<EnemyController>());
		}
		ResetEnemies();
	}

	_explosive_crystals.clear();
	_explosive_crystals.reserve(5);

	for (GameObject* crystal_go : crystal_pool->children)
	{
		_explosive_crystals.push_back(crystal_go);
	}

	for (GameObject* laser : _laser_wall->children)
	{
		laser->GetComponent<LaserController>()->ChangeState(LaserController::State::INACTIVE);
	}

    for (GameObject* laser : _rotating_lasers->children)
    {
        laser->GetComponent<LaserController>()->ChangeState(LaserController::State::INACTIVE);
        laser->GetComponent<LaserController>()->_toggle_activation = false;
    }

    if (_stalagmite_manager_go)
    {
        _stalagmite_manager =
            _stalagmite_manager_go->GetComponent<StalagmiteManager>();
    }

    initial_position = transform->GetGlobalPosition();
    initial_rotation = transform->GetGlobalRotationEuler();
}

void Hachiko::Scripting::BossController::OnStart()
{
	animation->StartAnimating();
	animation->SendTrigger("isCacoonLoop");
    audio_source->PostEvent(Sounds::BOSS_BREATHE);
	OverrideCameraOffset();
}

void Hachiko::Scripting::BossController::OnUpdate()
{
	if (Input::IsKeyDown(Input::KeyCode::KEY_H))
	{
		constexpr int player_dmg = 5;
		RegisterHit(player_dmg);
	}

	if (damage_effect_progress >= 0.0f)
	{
		damage_effect_progress -= Time::DeltaTime() / damage_effect_duration;
		float progress = damage_effect_progress / damage_effect_duration;
		game_object->ChangeEmissiveColor(float4(1.0f, 1.0f, 1.0f, progress), true);
	}

	StateController();
	state_value = static_cast<int>(state);
	combat_state_value = static_cast<int>(combat_state);
}

bool Hachiko::Scripting::BossController::IsAlive() const
{
	return combat_stats->IsAlive();
}

void Hachiko::Scripting::BossController::RegisterHit(int dmg)
{
	if (state == BossState::WAITING_ENCOUNTER)
	{
		state = BossState::STARTING_ENCOUNTER;
	}

	if (!hitable)
	{
		return;
	}

	damage_effect_progress = damage_effect_duration;
	combat_stats->_current_hp -= dmg;
	UpdateHpBar();
}

void Hachiko::Scripting::BossController::UpdateHpBar() const
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

void Hachiko::Scripting::BossController::SetHpBarActive(bool v) const
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
	case BossState::COCOON_FORM:
		CocoonController();
		break;
	case BossState::DEAD:
        _stalagmite_manager->DestroyAllStalagmites();
        if (animation->IsAnimationStopped())
        {
            level_manager->BossKilled();
        }
		break;
	}
}

void Hachiko::Scripting::BossController::StateTransitionController()
{
	const bool state_changed = state != prev_state;

	if (!state_changed)
	{
		return;
	}

	switch (state)
	{
	case BossState::WAITING_ENCOUNTER:
		break;
	case BossState::STARTING_ENCOUNTER:
		
		StartEncounter();
		break;
	case BossState::COMBAT_FORM:
		ResetCombatState();
		break;
	case BossState::COCOON_FORM:
		StartCocoon();
		break;
	case BossState::DEAD:
		break;
	}

	prev_state = state;
}

void Hachiko::Scripting::BossController::CombatController()
{
	if (CocoonTrigger())
	{
		state = BossState::COCOON_FORM;
		return;
	}

    if (combat_stats->_current_hp <= 0)
    {
        KillEnemies();
        audio_source->PostEvent(Sounds::BOSS_DEATH);
        animation->SendTrigger("isDeath");
        state = BossState::DEAD;
        return;
    }

	SpawnEnemy();

	CombatTransitionController();

	switch (combat_state)
	{
	case CombatState::IDLE:
		animation->SendTrigger("isIdle");
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
	case CombatState::JUMPING:
		ExecuteJumpingState();
		break;
	}
}

void Hachiko::Scripting::BossController::CombatTransitionController()
{
	const bool state_changed = combat_state != prev_combat_state;

	if (!state_changed)
	{
		return;
	}

	switch (combat_state)
	{
	case CombatState::IDLE:
		animation->SendTrigger("isWalk");
		combat_state = CombatState::CHASING;
		break;
	case CombatState::CHASING:
		Chase();
		break;
	case CombatState::ATTACKING:
		MeleeAttack();
		break;
	case CombatState::SPAWNING_CRYSTALS:
		SpawnCrystals();
		break;
	case CombatState::JUMPING:
		// TODO: The frame that the boss's health drops below 50% trigger crystal jump:
		// TriggerJumpingState(JumpingMode::CRYSTAL);
		TriggerJumpingState(JumpingMode::DETERMINED_BY_PATTERN);
		break;
	}

	prev_combat_state = combat_state;
}

float Hachiko::Scripting::BossController::GetPlayerDistance()
{
	const float3& player_position = player->GetTransform()->GetGlobalPosition();

	// If player is very close change to attack mode
	return transform->GetGlobalPosition().Distance(player_position);
}

void Hachiko::Scripting::BossController::WaitingController()
{
}

void Hachiko::Scripting::BossController::StartEncounter()
{
    encounter_start_timer = 0.f;
    RestoreCameraOffset();
    BreakCocoon();

    obstacle->RemoveObstacle();
}

void Hachiko::Scripting::BossController::StartEncounterController()
{

    // Add any effects desired for combat start, for now it only delays while camera is transitioning
    audio_source->PostEvent(Sounds::BOSS_ROAR);
    
    enemy_timer += Time::DeltaTime();
    if (enemy_timer < encounter_start_duration)
    {
        cocoons_parent->ChangeDissolveProgress(1 - enemy_timer / encounter_start_duration, true);
        return;
    }
    audio_source->PostEvent(Sounds::SET_STATE2_BOSS_FIGHT);

    for (GameObject* crystal : cocoons_parent->children)
    {
        crystal->SetActive(false);
    }
    SetHpBarActive(true);
    agent->AddToCrowd();

	player_camera->SetDoLookAhead(true);

    hitable = true;
	std::copy(_jumping_pattern_1, _jumping_pattern_1 + JumpUtil::JUMP_PATTERN_SIZE, _current_jumping_pattern);

	state = BossState::COMBAT_FORM;
}

void Hachiko::Scripting::BossController::ResetCombatState()
{
	// Reset combat related variables to prevent bugs:
	combat_state = CombatState::IDLE;
	prev_combat_state = CombatState::JUMPING;
}

void Hachiko::Scripting::BossController::Die()
{
	// Start death
}

void Hachiko::Scripting::BossController::DieController()
{
}

float4x4 Hachiko::Scripting::BossController::GetMeleeAttackOrigin(float attack_range) const
{
	float3 emitter_direction = transform->GetFront().Normalized();
	float3 emitter_position = transform->GetGlobalPosition() + emitter_direction * (attack_range / 2.f);
	float4x4 emitter = float4x4::FromTRS(emitter_position, transform->GetGlobalRotation(), transform->GetGlobalScale());
	return emitter;
}

void Hachiko::Scripting::BossController::StartCocoon()
{
    animation->SendTrigger("isWalk");

    time_elapse = 0.0;
    SetUpCocoon();
    FocusCameraOnBoss(true);
    audio_source->PostEvent(Sounds::SET_STATE1_BOSS_FIGHT);
    //Remove all stalactites and enemies
    _stalagmite_manager->DestroyAllStalagmites(true);
    KillEnemies();

    // Direct boss to intial position
    moving_to_initial_pos = true;
    initial_position.y = transform->GetGlobalPosition().y;
    transform->LookAtTarget(initial_position);
    agent->SetTargetPosition(initial_position);
}

void Hachiko::Scripting::BossController::CocoonController()
{
    // Handle boss movement to initial position
    if (moving_to_initial_pos)
    {
        agent->SetTargetPosition(initial_position);

        // Knockback player
        float3 player_pos = player->GetTransform()->GetGlobalPosition();
        if (Distance(player_pos, transform->GetGlobalPosition()) <= 3)
        {
            float3 player_end_pos = player_pos + (player_pos - transform->GetGlobalPosition()).Normalized() * 3;
            player->GetTransform()->SetGlobalPosition(player_end_pos);
        }

        float3 current_pos = transform->GetGlobalPosition();
        if (Distance(current_pos, initial_position) <= agent->GetRadius())
        {
            animation->SendTrigger("isCacoonLoop");
            audio_source->PostEvent(Sounds::STOP_BOSS_FOOTSTEPS);

            moving_to_initial_pos = false;
            transform->SetGlobalPosition(initial_position);
            transform->SetGlobalRotationEuler(initial_rotation);

            agent->RemoveFromCrowd();
            obstacle->AddObstacle();


            for (GameObject* crystal: cocoons_parent->children)
            {
                crystal->SetActive(true);
                crystal->ChangeDissolveProgress(1.f, true);
                crystal->GetComponent<CrystalExplosion>()->RegenCrystal();
            }
            
        }
        return;
    }

    // Handle camera focus
    if (camera_focus_on_boss)
    {
        time_elapse += Time::DeltaTime();

        if (time_elapse >= 2) // HARDCODED TIME FOR CAMERA FOCUS
        {
            FocusCameraOnBoss(false);
            // Gauntlet Starts once the camera is unlocked
            InitCocoonGauntlet();
        }
    }

    if (_rotating_lasers)
    {
        float3 rot_lasers_rotation = _rotating_lasers->GetTransform()->GetLocalRotationEuler();
        rot_lasers_rotation.y += Time::DeltaTime() * 25;
        _rotating_lasers->GetTransform()->SetLocalRotationEuler(rot_lasers_rotation);
    }

    // Replace with is gauntlet completed
    if (gauntlet->IsFinished() || Input::IsKeyDown(Input::KeyCode::KEY_J))
    {
        FinishCocoon();
        BreakCocoon();
    }
}

void Hachiko::Scripting::BossController::SetUpCocoon()
{
    hitable = false;
}

void Hachiko::Scripting::BossController::BreakCocoon()
{
    for (GameObject* crystal: cocoons_parent->children)
    {
        crystal->GetComponent<CrystalExplosion>()->DestroyCrystal();
    }
    animation->SendTrigger("isCacoonComingOut");
}

void Hachiko::Scripting::BossController::InitCocoonGauntlet()
{
    // Unlock the lasers
    for (GameObject* laser : _rotating_lasers->children)
    {
        laser->GetComponent<LaserController>()->_toggle_activation = true;
    }
    audio_source->PostEvent(Sounds::GAUNTLET_NEXT_ROUND);
    // Start spawning enemies
    gauntlet->StartGauntlet();
}

bool Hachiko::Scripting::BossController::CocoonTrigger()
{
    if (gauntlet_thresholds_percent.empty())
    {
        return false;
    }
    if (gauntlet_thresholds_percent.back() >= static_cast<float>(combat_stats->_current_hp) / combat_stats->_max_hp)
    {
        gauntlet_thresholds_percent.pop_back();

		ChangeStateText("In Cocoon.");

		return true;
	}
	return false;
}

void Hachiko::Scripting::BossController::FinishCocoon()
{
	hitable = true;
	second_phase = true;
	BreakCocoon();
    audio_source->PostEvent(Sounds::SET_STATE3_BOSS_FIGHT);
    obstacle->RemoveObstacle();
    agent->AddToCrowd();

    // We change our jumping pattern to the second one and reset the index
    std::copy(_jumping_pattern_2, _jumping_pattern_2 + JumpUtil::JUMP_PATTERN_SIZE, _current_jumping_pattern);
    _jump_pattern_index = -1;

	gauntlet->ResetGauntlet(true);

    for (GameObject* laser : _rotating_lasers->children)
    {
        laser->GetComponent<LaserController>()->ChangeState(LaserController::State::INACTIVE);
        laser->GetComponent<LaserController>()->_toggle_activation = false;
    }

    ChangeStateText("Finished Cocoon.");

	state = BossState::COMBAT_FORM;
}

void Hachiko::Scripting::BossController::Chase()
{
    ChangeStateText("Chasing player.");
    audio_source->PostEvent(Sounds::BOSS_FOOTSTEPS);
    if (animation->IsAnimationStopped())
    {
        animation->SendTrigger("isWalk");
    }
    chasing_timer = 0.0f;
    combat_state = CombatState::CHASING;
}

void Hachiko::Scripting::BossController::ChaseController()
{
	const float3& player_position = player->GetTransform()->GetGlobalPosition();

	transform->LookAtTarget(player_position);

    chasing_timer += Time::DeltaTimeScaled();
    if (chasing_timer >= chasing_time_limit)
    {
        combat_state = CombatState::JUMPING;
        return;
    }

	// If player is very close change to attack mode
	const float player_distance =
		transform->GetGlobalPosition().Distance(player_position);

	if (player_distance <= combat_stats->_attack_range)
	{
		// We expect transition to attack to start the attack
		combat_state = CombatState::ATTACKING;
		return;
	}

	float3 agent_target_position = player_position;

	agent->SetMaxSpeed(combat_stats->_move_speed);

	const float3 corrected_position = Navigation::GetCorrectedPosition(
		agent_target_position,
		math::float3(10.0f));

	if (corrected_position.x < FLT_MAX)
	{
		agent_target_position = corrected_position;
		transform->LookAtTarget(agent_target_position);
		agent->SetTargetPosition(agent_target_position);
	}
}

void Hachiko::Scripting::BossController::MeleeAttack()
{
	attack_delay_timer = 0.f;
	after_attack_wait_timer = 0.f;
	attacked = false;

	ChangeStateText("Melee attacking.");

    animation->SendTrigger("isMelee");
}

void Hachiko::Scripting::BossController::MeleeAttackController()
{
	attack_delay_timer += Time::DeltaTime();

	if (attack_delay_timer < attack_delay)
	{
		return;
	}

    if (!attacked)
    {
        audio_source->PostEvent(Sounds::BOSS_MELEE_STAND);
        CombatManager::AttackStats attack_stats;
        attack_stats.damage = 1.0f;
        attack_stats.knockback_distance = 0.0f;
        attack_stats.width = 4.f;
        attack_stats.range = combat_stats->_attack_range * 1.3f;
        attack_stats.type = CombatManager::AttackType::RECTANGLE;

        _weapon_trail_billboard_right->Play();

        combat_manager->EnemyMeleeAttack(GetMeleeAttackOrigin(attack_stats.range), attack_stats);
        attacked = true;
    }
	
    after_attack_wait_timer += Time::DeltaTime();

	if (after_attack_wait_timer < after_attack_wait)
	{
		return;
	}

	combat_state = CombatState::JUMPING;
}

void Hachiko::Scripting::BossController::SpawnCrystals()
{
	ChangeStateText("Spawning crystals.");
    animation->SendTrigger("isSummonCrystal");
    audio_source->PostEvent(Sounds::BOSS_LAUGH);
    if (_explosive_crystals.empty())
    {
        return;
    }

    _current_index_crystals = _current_index_crystals % _explosive_crystals.size();

    const GameObject* current_crystal_to_spawn =
        _explosive_crystals[_current_index_crystals];

    if (current_crystal_to_spawn == nullptr)
    {
        return;
    }

    const float3& player_position = player->GetTransform()->GetGlobalPosition();

    const float3 offset = float3(
        RandomUtil::RandomBetween(3.f, 5.f) * (float)RandomUtil::RandomInt(), // x
        0.f,                                 // y
        RandomUtil::RandomBetween(3.f, 5.f) * (float)RandomUtil::RandomInt());// z

    const float3 emitter_position = Navigation::GetCorrectedPosition(player_position + offset, float3(5.0f, 5.0f, 5.0f));

    current_crystal_to_spawn->FindDescendantWithName("ExplosionIndicatorHelper")->SetActive(false);
    current_crystal_to_spawn->GetTransform()->SetGlobalPosition(emitter_position);

    _current_index_crystals = (_current_index_crystals + 1) % _explosive_crystals.size();
}

void Hachiko::Scripting::BossController::SpawnCrystalsController()
{
    if (!animation->IsAnimationStopped())
    {
        return;
    }
    
	combat_state = CombatState::CHASING;
}



///////////////////////////////////////////////////////////////////////////////
/// Jumping State Related Methods:

void Hachiko::Scripting::BossController::TriggerJumpingState(
	JumpingMode jumping_mode)
{
	combat_state = CombatState::JUMPING;

	ResetJumpingState();

	_current_jumping_mode = jumping_mode;

	if (_current_jumping_mode == JumpingMode::DETERMINED_BY_PATTERN)
	{
		// Increment jumping mode:
		_jump_pattern_index =
			(_jump_pattern_index + 1) % JumpUtil::JUMP_PATTERN_SIZE;
		// Set the jumping mode:
		_current_jumping_mode = _current_jumping_pattern[_jump_pattern_index];
	}
}

void Hachiko::Scripting::BossController::ResetJumpingState()
{
	_jumping_state = JumpingState::NOT_TRIGGERED;
	_jumping_timer = 0.0f;
}

inline bool ShouldDealBasicAOE(Hachiko::Scripting::JumpingMode mode)
{
	return (mode == Hachiko::Scripting::JumpingMode::STALAGMITE ||
		mode == Hachiko::Scripting::JumpingMode::BASIC_ATTACK ||
		mode == Hachiko::Scripting::JumpingMode::LASER);
}

void Hachiko::Scripting::BossController::ExecuteJumpingState()
{
    // TODO: This is for debug, delete this after adding the anims.
    ///////////////////////////////////////////////////////////////////////////
    std::string jump_type;

    if (_current_jumping_mode == JumpingMode::STALAGMITE)
    {
        jump_type = "(Stalagmite) ";
    }
    else if (_current_jumping_mode == JumpingMode::BASIC_ATTACK)
    {
        jump_type = "(Basic Attack)";
    }
    else if (_current_jumping_mode == JumpingMode::CRYSTAL)
    {
        jump_type = "(Crystal)";
    }
    else if (_current_jumping_mode == JumpingMode::LASER)
    {
        jump_type = "(Laser)";
    }
    ///////////////////////////////////////////////////////////////////////////

    if (_jumping_state == JumpingState::NOT_TRIGGERED)
    {
        _jumping_state = JumpingState::WAITING_TO_JUMP;
        _jumping_timer = 0.0f;

        ChangeStateText((jump_type + "Waiting to jump.").c_str());

		if (_current_jumping_mode == JumpingMode::STALAGMITE)
		{
			animation->SendTrigger("isPreJumpCrystal");
		}
		else {
			animation->SendTrigger("isPreJump");
		}

        animation->SendTrigger("isPreJump");

        // Disable the agent component, gets enabled back when boss lands back:
        agent->RemoveFromCrowd();
        agent->Disable();

        // TODO: Maybe calculate start, mid and end positions here instead.
        // TODO: Maybe trigger rotating the boss to the jump direction here.
    }

    // It's important that we do the increment here, by this, if the duration
    // of current state is set to < 0.0f, we don't need to have check for
    // zero or negative conditions:
    _jumping_timer += Time::DeltaTime();

    switch (_jumping_state)
    {
    case JumpingState::WAITING_TO_JUMP:
    {
        if (_jumping_timer < _jump_start_delay)
        {
            if (_current_jumping_mode == JumpingMode::CRYSTAL)
            {
                game_object->GetTransform()->LookAtTarget(_crystal_jump_position);
            }
            else
            {
                game_object->GetTransform()->LookAtTarget(player->GetTransform()->GetGlobalPosition());
            }

            // Boss is waiting for jump here:
            // SFX
            if (_current_jumping_mode == JumpingMode::BASIC_ATTACK)
            {
                audio_source->PostEvent(Sounds::BOSS_PRE_JUMP);
            }

            break;
        }

        // Boss starts ascending here:
        _jumping_state = JumpingState::ASCENDING;
        _jumping_timer = 0.0f;

        hitable = false;

        // Set the start & end positions for the jump:
        _jump_start_position = game_object->GetTransform()->GetGlobalPosition();

        if (_current_jumping_mode == JumpingMode::CRYSTAL)
        {
            _jump_end_position = _crystal_jump_position;
        }
        else
        {
            _jump_end_position = player->GetTransform()->GetGlobalPosition();

            // Apply offset to the end position, if the offset is zero, the end
            // position of the jump will be basically equal to the player
            // position:
            const float3 jump_direction(
                (_jump_start_position - _jump_end_position).Normalized());
            _jump_end_position =
                _jump_end_position + jump_direction * _jump_offset;
            _jump_end_position.y = _jump_start_position.y;
        }
        game_object->GetTransform()->LookAtTarget(_jump_end_position);

        // Calculate mid position (only x and z is significant, z is ignored as
        // height is calculated separately), to avoid recalculation each frame
        // during the position lerping:
        _jump_mid_position = (_jump_start_position + _jump_end_position) * 0.5f;

        ChangeStateText((jump_type + "Ascending").c_str());

        if (_current_jumping_mode == JumpingMode::STALAGMITE)
		{
			animation->SendTrigger("isJumpLoopCrystal");
		}
		else {
			animation->SendTrigger("isJumpLoop");
		}

		if (_jump_start_delay > 0.0f)
		{
			break;
		}
    }

    case JumpingState::ASCENDING:
    {
        if (_jumping_timer < _jump_ascending_duration)
        {
            // Boss is ascending here:
            UpdateAscendingPosition();
            break;
        }

        // Boss starts waiting on the highest point of jump here:
        _jumping_state = JumpingState::POST_ASCENDING_DELAY;
        _jumping_timer = 0.0f;

        ChangeStateText((jump_type + "Waiting for the delay before highest point.").c_str());

        if (_jump_ascending_duration > 0.0f)
		{
			break;
		}
    }

    case JumpingState::POST_ASCENDING_DELAY:
    {
        if (_jumping_timer < _jump_post_ascending_delay)
        {
            // Boss waiting on the highest point here:
            break;
        }

        // Boss starts playing the highest point animation here:
        _jumping_state = JumpingState::ON_HIGHEST_POINT;
        _jumping_timer = 0.0f;
       
        // TODO: Trigger highest point animation here.

        ChangeStateText((jump_type + "On the highest point of the jump.").c_str());


        if (_current_jumping_mode == JumpingMode::LASER)
        {
            const int dir = RandomUtil::RandomIntBetween(0, 3);
            
            // Set a random direction for the lasers
            float3 laser_wall_rot = _laser_wall->GetTransform()->GetLocalRotationEuler();
            laser_wall_rot.y = _wall_dir_angles[dir];
            _laser_wall->GetTransform()->SetLocalRotationEuler(laser_wall_rot);

            // Kill all enemies before lasers
            KillEnemies();

            for (GameObject* laser : _laser_wall->children)
            {
                laser->GetComponent<LaserController>()->ChangeState(LaserController::State::ACTIVATING);
            }
            _laser_wall_current_time = 0.0f;
        }

        if (_jump_post_ascending_delay > 0.0f)
		{
			break;
		}
    }

    case JumpingState::ON_HIGHEST_POINT:
    {
        if (_current_jumping_mode == JumpingMode::LASER)
        {
            if (!ControlLasers())
            {
                break;
            }
        }

        if (_jumping_timer < _jump_on_highest_point_duration)
        {
            // Boss is on the highest point here:
            break;
        }

        // Boss starts waiting after the highest point animation here:
        _jumping_state = JumpingState::POST_ON_HIGHEST_POINT;
        _jumping_timer = 0.0f;

        ChangeStateText((jump_type + "Waiting before descending.").c_str());

        if (_jump_on_highest_point_duration > 0.0f)
		{
			break;
		}
    }

    case JumpingState::POST_ON_HIGHEST_POINT:
    {
        if (_jumping_timer < _jump_post_on_highest_point_delay)
        {
            // Boss is waiting before descending here:
            break;
        }

        // Boss starts descending animation here:
        _jumping_state = JumpingState::DESCENDING;
        _jumping_timer = 0.0f;

        ChangeStateText((jump_type + "Descending.").c_str());

        if (_jump_post_on_highest_point_delay > 0.0f)
		{
			break;
		}
    }

    case JumpingState::DESCENDING:
    {

        UpdateDescendingPosition();

        if (_jumping_timer < _jump_descending_duration)
        {
            // Boss is only descending here, the update continues:
            break;
        }

        // Boss lands to the ground here:
        _jumping_state = JumpingState::POST_DESCENDING_DELAY;
        _jumping_timer = 0.0f;

        // Boss is now hittable:			
        hitable = true;

        if (_jump_descending_duration > 0.0f)
		{
			// Boss is only descending here, the update continues:
			break;
		}
    }

    case JumpingState::POST_DESCENDING_DELAY:
    {
        audio_source->PostEvent(Sounds::BOSS_HIT_FLOOR);

        if (_jumping_timer < _jump_post_descending_delay)
        {
            break;
        }

        // Boss starts getting up here:
        _jumping_state = JumpingState::GETTING_UP;
        _jumping_timer = 0.0f;


        if (_current_jumping_mode == JumpingMode::STALAGMITE)
        {
            // TODO: Trigger special landing & getting up animation here.
        }
        else
        {
            // TODO: Trigger normal landing & getting up animation here.
        }

        if (_current_jumping_mode == JumpingMode::STALAGMITE)
		{
			animation->SendTrigger("isLandingCrystal");
		}
		else
		{
			animation->SendTrigger("isLanding");
		}

        player_camera->Shake(2.5f, 2.f);

        ChangeStateText((jump_type + "Landed & waiting").c_str());

        if (_jump_post_descending_delay > 0.0f)
		{
			break;
		}
    }

    case JumpingState::GETTING_UP:
    {
        combat_visual_effects_pool->PlayGroundCrackEffect(transform->GetGlobalPosition());

        if (_jumping_timer < _jump_getting_up_duration)
        {
            // Boss is getting up here:
            if (ShouldDealBasicAOE(_current_jumping_mode))
            {
                std::vector<GameObject*> check_hit = {};
                std::vector<GameObject*> elements_hit = {};

                check_hit.insert(check_hit.end(), enemy_pool->children.begin(), enemy_pool->children.end());
                check_hit.push_back(Scenes::GetPlayer());

                for (int i = 0; i < check_hit.size(); ++i)
                {
                    if (check_hit[i]->active &&
                        _damage_AOE >= transform->GetGlobalPosition().Distance(check_hit[i]->GetTransform()->GetGlobalPosition()))
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
                        enemy_controller->RegisterHit(combat_stats->_attack_power, relative_dir.Normalized(), 0.7f, false, false);
                    }

                    if (player_controller != nullptr)
                    {
                        player_controller->RegisterHit(combat_stats->_attack_power, true, relative_dir.Normalized(), false, PlayerController::DamageType::CRYSTAL);
                    }
                }
            }

            break;
        }

        if (_current_jumping_mode != JumpingMode::STALAGMITE)
        {
            animation->SendTrigger("isGettingUpJump");
        }
        

        // Boss starts waiting for the skill here:
        _jumping_state = JumpingState::WAITING_FOR_SKILL;
        _jumping_timer = 0.0f;

        // Reset jump start and end positions to ensure correct & clean state:
        _jump_end_position = float3::zero;
        _jump_start_position = float3::zero;

        // Enable the agent component, it was disabled when the jumping was
        // started:
        agent->Enable();
        agent->AddToCrowd();

        ChangeStateText((jump_type + "Waiting to cast skill").c_str());

        // TODO: Trigger Getting ready for skill animation here.

        if (_jump_getting_up_duration > 0.0f)
		{
			break;
		}
    }


    case JumpingState::WAITING_FOR_SKILL:
    {
        if (_jumping_timer < _jump_skill_delay)
        {
            // Boss is waiting to cast skill here:
            break;
        }

        // Boss starts casting skill here:
        _jumping_state = JumpingState::CASTING_SKILL;
        _jumping_timer = 0.0f;

        // TODO: Trigger skill casting animation here.

        if (_current_jumping_mode == JumpingMode::STALAGMITE)
        {
            if (_stalagmite_manager)
            {
                _stalagmite_manager->TriggerStalagmites();
            }
        }
        

        ChangeStateText((jump_type + "Casting Skill.").c_str());

        break;
    }

    case JumpingState::CASTING_SKILL:
    {
        if (_jumping_timer < _jump_skill_duration)
        {
            // Boss is casting skill here:
            break;
        }



        if (_current_jumping_mode == JumpingMode::STALAGMITE)
        {
            if (_stalagmite_manager && !_stalagmite_manager->AllStalactitesCollapsed())
            {
                break;
            }
        }

        if (_current_jumping_mode == JumpingMode::STALAGMITE)
        {
            animation->SendTrigger("isGetingUpCrystalJump");
        }

        // Boss deals the aoe damage here:
        _jumping_state = JumpingState::POST_CAST_SKILL;
        _jumping_timer = 0.0f;

        // TODO: Deal skill damage here.
        // TODO: Trigger post skill stuff here.

        ChangeStateText((jump_type + "Casted Skill.").c_str());

        break;
    }

    case JumpingState::POST_CAST_SKILL:
    {
        if (_jumping_timer < _jump_post_skill_delay)
        {
            // Boss is doing whatever it's supposed to do after it casts aoe:
            break;
        }

        // Boss finishes jump&aoe business here:
        _jumping_state = JumpingState::NOT_TRIGGERED;
        _jumping_timer = 0.0f;

        // Transition to correct state:
        if (ShouldDoAFollowUpJump())
        {
            // Set prev step something != JUMPING to ensure that state tree,
            // runs TriggerJumping again:
            prev_combat_state = CombatState::IDLE;
            combat_state = CombatState::JUMPING;
        }
        else
        {
            prev_combat_state = CombatState::JUMPING;
            combat_state = CombatState::SPAWNING_CRYSTALS;
        }

        break;
    }

    case JumpingState::NOT_TRIGGERED:
    case JumpingState::COUNT:
        break;
    }
}

inline void LerpJump(
	const Hachiko::GameObject* game_object,
	const float3& position_start,
	const float3& position_end,
	const float position_step,
	const float jump_start,
	const float jump_height,
	const float height_step)
{
	float3 position = math::Lerp(
		position_start,
		position_end,
		position_step);

	position.y = math::Lerp(
		jump_start,
		jump_height,
		height_step);

	game_object->GetTransform()->SetGlobalPosition(position);
}

inline float CalculateAscendingHeightStep(const float position_step)
{
	return 1.0f - powf(position_step - 1.0f, 2.0f);
}

void Hachiko::Scripting::BossController::UpdateAscendingPosition() const
{
	// Get the step i.e percentage of ascending for x & z position:
	const float position_step = std::min(
		1.0f,
		_jumping_timer / _jump_ascending_duration);
	// Get the step of y position. We want it to be a parabolic jump, which
	// looks like a gravity is applied:
	const float height_step = CalculateAscendingHeightStep(position_step);

	const float _real_height = _current_jumping_mode == JumpingMode::LASER ? _laser_jump_height : _jump_height;

	LerpJump(
		game_object,
		_jump_start_position,
		_jump_mid_position,
		position_step,
		_jump_start_position.y,
		_real_height,
		height_step);
}

inline float CalculateDescendingHeightStep(const float position_step)
{
	return 1.0f - (position_step * position_step);
}

void Hachiko::Scripting::BossController::UpdateDescendingPosition() const
{
	// Get the step i.e percentage of descending for x & z position:
	const float position_step = std::min(
		1.0f,
		_jumping_timer / _jump_descending_duration);
	// Get the step of y position. We want it to be a parabolic jump, which
	// looks like a gravity is applied:
	const float height_step = CalculateDescendingHeightStep(position_step);

	const float _real_height = _current_jumping_mode == JumpingMode::LASER ? _laser_jump_height : _jump_height;

	LerpJump(
		game_object,
		_jump_mid_position,
		_jump_end_position,
		position_step,
		_jump_start_position.y,
		_real_height,
		height_step);
}

bool Hachiko::Scripting::BossController::ShouldDoAFollowUpJump() const
{
	return (_jump_pattern_index + 2) % 3 == 0;
}

///
///////////////////////////////////////////////////////////////////////////////

void Hachiko::Scripting::BossController::ChangeStateText(
	const char* state_string) const
{
	if (!_boss_state_text_ui)
	{
		return;
	}

	_boss_state_text_ui->SetText(state_string);
}

void Hachiko::Scripting::BossController::OverrideCameraOffset()
{
	overriden_original_camera_offset = true;
	original_camera_offset = player_camera->GetRelativePosition();
	player_camera->ChangeRelativePosition(pre_combat_camera_offset);
}

void Hachiko::Scripting::BossController::RestoreCameraOffset()
{
	if (!overriden_original_camera_offset)
	{
		// Prevents using unitialized pre_combat_camera_offset
		return;
	}
	overriden_original_camera_offset = false;
	constexpr bool do_lookahead = false;
	player_camera->ChangeRelativePosition(original_camera_offset, do_lookahead, camera_transition_speed);
}

void Hachiko::Scripting::BossController::FocusCameraOnBoss(bool focus_on_boss)
{
	camera_focus_on_boss = focus_on_boss;
	if (camera_focus_on_boss)
	{
		level_manager->BlockInputs(true);
		player_camera->SetObjective(game_object);
	}
	else
	{
		level_manager->BlockInputs(false);
		player_camera->SetObjective(player);
	}
}

void Hachiko::Scripting::BossController::SpawnEnemy()
{
	enemy_timer += Time::DeltaTime();
	if (enemy_timer < time_between_enemies)
	{
		return;
	}

	for (EnemyController* enemy_controller : enemies)
	{
		GameObject* enemy = enemy_controller->GetGameObject();

		if (enemy->IsActive())
		{
			continue;
		}

		enemy->SetActive(true);
		enemy_controller->OnStart();

		ComponentAgent* agent = enemy->GetComponent<ComponentAgent>();
		if (agent)
		{
			agent->AddToCrowd();
		}

		break;
	}

	enemy_timer = 0;
}

void Hachiko::Scripting::BossController::ResetEnemies()
{
	for (EnemyController* enemy_controller : enemies)
	{
		GameObject* enemy = enemy_controller->GetGameObject();
		ComponentAgent* agent = enemy_controller->GetGameObject()->GetComponent<ComponentAgent>();

		if (agent)
		{
			agent->RemoveFromCrowd();
		}

		if (enemy_controller)
		{
			// Maybe pack these methods into a method of EnemyController for
			// ease of extensibility.
			enemy_controller->SetIsFromBoss(true);
			enemy_controller->ResetEnemy();
			enemy_controller->ResetEnemyPosition();
		}

		enemy->SetActive(false);
	}
}

void Hachiko::Scripting::BossController::KillEnemies()
{
    for (EnemyController* enemy_controller : enemies)
    {
        enemy_controller->SetDead();
    }
}

/**
* Returns true if all lasers are finished
*/
bool Hachiko::Scripting::BossController::ControlLasers()
{
	if (_laser_wall_current_time < _laser_wall_duration)
	{
		_laser_wall_current_time += Time::DeltaTime();
		return false;
	}
	else
	{
		for (GameObject* laser : _laser_wall->children)
		{
			laser->GetComponent<LaserController>()->ChangeState(LaserController::State::DISSOLVING);
		}
		return true;
	}
}
