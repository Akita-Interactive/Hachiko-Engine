#include "scriptingUtil/gameplaypch.h"

#include "entities/enemies/BossController.h"
#include "entities/enemies/EnemyController.h"
#include "misc/StalagmiteManager.h"
#include "entities/Stats.h"
#include "constants/Scenes.h"

Hachiko::Scripting::BossController::BossController(GameObject* game_object)
    : Script(game_object, "BossController")
    , state_value(0)
    , combat_state_value(0)
    , second_phase(false)
    , hp_bar_go(nullptr)
    , crystal_target_go(nullptr)
    , cocoon_placeholder_go(nullptr)
    , gauntlet_go(nullptr)
    , _explosive_crystals({})
    , _current_index_crystals(0)
    , start_encounter_range(0.0f)
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
    , _jump_pattern_index(0)
    , _current_jumping_mode(JumpingMode::BASIC_ATTACK)
    , _boss_state_text_ui(nullptr)
    , attack_current_cd(0.0f)
    , time_between_enemies(5.0)
    , enemy_pool(nullptr)
{
    CreateBossWeapons();
}

void Hachiko::Scripting::BossController::CreateBossWeapons()
{
    Weapon melee;
    melee.name = "Claw";
    melee.color = float4(255.0f, 0.0f, 0.0f, 255.0f);
    melee.unlimited = true;
    melee.attacks.push_back(GetAttackType(AttackType::MELEE_CLAW));

    Weapon sword;
    sword.name = "Sword";
    sword.color = float4(0.0f, 255.0f, 0.0f, 255.0f);
    sword.unlimited = true;
    sword.attacks.push_back(GetAttackType(AttackType::SWORD));

    Weapon hammer;
    hammer.name = "Hammer";
    hammer.color = float4(0.0f, 255.0f, 255.0f, 0.0f);
    hammer.unlimited = true;
    hammer.attacks.push_back(GetAttackType(AttackType::HAMMER));

    weapons.push_back(melee);
    weapons.push_back(sword);
    weapons.push_back(hammer);
}

void Hachiko::Scripting::BossController::OnAwake()
{
    player = Scenes::GetPlayer();
    level_manager = Scenes::GetLevelManager()->GetComponent<LevelManager>();
    player_camera = Scenes::GetMainCamera()->GetComponent<PlayerCamera>();
    combat_manager = Scenes::GetCombatManager()->GetComponent<CombatManager>();
    transform = game_object->GetTransform();
    combat_stats = game_object->GetComponent<Stats>();
    agent = game_object->GetComponent<ComponentAgent>();
    agent->SetMaxSpeed(combat_stats->_move_speed);
    SetUpHpBar();
    // Hp bar starts hidden until encounter starts
    SetHpBarActive(false);
    if (cocoon_placeholder_go)
    {
        cocoon_placeholder_go->SetActive(false);
    }
    gauntlet = gauntlet_go->GetComponent<GauntletManager>();

    ChangeWeapon(0);

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

    if (_stalagmite_manager_go)
    {
        _stalagmite_manager =
            _stalagmite_manager_go->GetComponent<StalagmiteManager>();
    }
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
    combat_state_value = static_cast<int>(combat_state);
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

        game_object->ChangeEmissiveColor(float4(255, 255, 255, 255), 0.3f, true);
    }
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
        // We already start in this state, no transition for now
        // We can reset the scene on death here which will be easier
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
        level_manager->GoalReached();
        state = BossState::DEAD;
        return;
    }

    SpawnEnemy();

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
    // If there is any start sequence manage it here and hold state until it
    // ends. For now it goes to next state instantly:
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
    hitable = false;
    time_elapse = 0.0;
    if (cocoon_placeholder_go)
    {
        cocoon_placeholder_go->SetActive(true);
    }
    FocusCamera(true);
}

void Hachiko::Scripting::BossController::CocoonController()
{
    // Handle camera focus
    if (camera_focus_on_boss)
    {
        time_elapse += Time::DeltaTime();

        if (time_elapse >= 2) // HARDCODED TIME FOR CAMERA FOCUS
        {
            FocusCamera(false);
        }
    }

    // Replace with is gauntlet completed
    if (gauntlet->IsFinished() || Input::IsKeyDown(Input::KeyCode::KEY_J))
    {
        FinishCocoon();
    }
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
        gauntlet->StartGauntlet();

        ChangeStateText("In Cocoon.");

        return true;
    }
    return false;
}

void Hachiko::Scripting::BossController::FinishCocoon()
{
    hitable = true;
    second_phase = true;
    if (cocoon_placeholder_go)
    {
        cocoon_placeholder_go->SetActive(false);
    }
    gauntlet->ResetGauntlet();

    ChangeStateText("Finished Cocoon.");

    state = BossState::COMBAT_FORM;
}

void Hachiko::Scripting::BossController::Chase()
{
    ChangeStateText("Chasing player.");
    combat_state = CombatState::CHASING;
}

void Hachiko::Scripting::BossController::ChaseController()
{
    const float3& player_position = player->GetTransform()->GetGlobalPosition();
    const BossAttack& boss_attack = GetCurrentAttack();

    transform->LookAtTarget(player_position);

    transform->LookAtTarget(player_position);

    // If player is very close change to attack mode
    const float player_distance =
        transform->GetGlobalPosition().Distance(player_position);

    if (player_distance <= boss_attack.stats.range)
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
        const Weapon& weapon = GetCurrentWeapon();
        const BossAttack& boss_attack = GetCurrentAttack();

        // Debug::DebugDraw(combat_manager->CreateAttackHitbox(GetMeleeAttackOrigin(boss_attack.stats.range), boss_attack.stats), weapon.color.Float3Part());
        combat_manager->EnemyMeleeAttack(GetMeleeAttackOrigin(boss_attack.stats.range), boss_attack.stats);
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
}

void Hachiko::Scripting::BossController::SpawnCrystalsController()
{
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

    const float3 emitter_direction = transform->GetFront().Normalized();
    const float3 emitter_position =
        transform->GetGlobalPosition() + emitter_direction *
        (combat_stats->_attack_range + 5.0f);

    current_crystal_to_spawn->FindDescendantWithName("ExplosionIndicatorHelper")->SetActive(false);
    current_crystal_to_spawn->GetTransform()->SetGlobalPosition(emitter_position);

    _current_index_crystals = (_current_index_crystals + 1) % _explosive_crystals.size();

    combat_state = CombatState::CHASING;
}

void Hachiko::Scripting::BossController::FocusCamera(bool focus_on_boss)
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
        _current_jumping_mode = _jumping_pattern[_jump_pattern_index];
    }
}

void Hachiko::Scripting::BossController::ResetJumpingState()
{
    _jumping_state = JumpingState::NOT_TRIGGERED;
    _jumping_timer = 0.0f;
}

inline bool ShouldDealBasicAOE(Hachiko::Scripting::JumpingMode mode)
{
    return (mode == Hachiko::Scripting::JumpingMode::WEAPON_CHANGE ||
        mode == Hachiko::Scripting::JumpingMode::STALAGMITE ||
        mode == Hachiko::Scripting::JumpingMode::BASIC_ATTACK);
}

void Hachiko::Scripting::BossController::ExecuteJumpingState()
{
    // TODO: This is for debug, delete this after adding the anims.
    ///////////////////////////////////////////////////////////////////////////
    std::string jump_type;

    if (_current_jumping_mode == JumpingMode::WEAPON_CHANGE)
    {
        jump_type = "(Weapon Change) ";
    }
    else if (_current_jumping_mode == JumpingMode::STALAGMITE)
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
    ///////////////////////////////////////////////////////////////////////////

    if (_jumping_state == JumpingState::NOT_TRIGGERED)
    {
        _jumping_state = JumpingState::WAITING_TO_JUMP;
        _jumping_timer = 0.0f;
        hitable = false;

        ChangeStateText((jump_type + "Waiting to jump.").c_str());

        // TODO: Trigger getting ready for jump animation here.

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
            // Boss is waiting for jump here:
            break;
        }

        // Boss starts ascending here:
        _jumping_state = JumpingState::ASCENDING;
        _jumping_timer = 0.0f;

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

        // Calculate mid position (only x and z is significant, z is ignored as
        // height is calculated separately), to avoid recalculation each frame
        // during the position lerping:
        _jump_mid_position = (_jump_start_position + _jump_end_position) * 0.5f;

        ChangeStateText((jump_type + "Ascending").c_str());

        // TODO: Trigger Jumping animation here.

        break;
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

        break;
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

        break;
    }

    case JumpingState::ON_HIGHEST_POINT:
    {
        if (_jumping_timer < _jump_on_highest_point_duration)
        {
            // Boss is on the highest point here:
            break;
        }

        // Boss starts waiting after the highest point animation here:
        _jumping_state = JumpingState::POST_ON_HIGHEST_POINT;
        _jumping_timer = 0.0f;

        ChangeStateText((jump_type + "Waiting before descending.").c_str());

        break;
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

        break;
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

        break;
    }

    case JumpingState::POST_DESCENDING_DELAY:
    {
        if (_jumping_timer < _jump_post_descending_delay)
        {
            // Boss is waiting to get up here:
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

        ChangeStateText((jump_type + "Landed & waiting").c_str());

        break;
    }

    case JumpingState::GETTING_UP:
    {
        if (_jumping_timer < _jump_getting_up_duration)
        {
            // Boss is getting up here:

            if (ShouldDealBasicAOE(_current_jumping_mode))
            {
                // TODO: Update basic attack AOE indicator & stuff here.
            }

            break;
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

        break;
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
        else if (_current_jumping_mode == JumpingMode::WEAPON_CHANGE)
        {
            // TODO: Trigger weapon changing animation here.

            // TODO: Fit this into a pattern.
            const unsigned new_weapon = (_current_weapon + 1) % 3;
            ChangeWeapon(new_weapon);
        }
        else if (_current_jumping_mode == JumpingMode::CRYSTAL)
        {
            // TODO: Trigger cocoon animation here.
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

    LerpJump(
        game_object,
        _jump_start_position,
        _jump_mid_position,
        position_step,
        _jump_start_position.y,
        _jump_height,
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

    LerpJump(
        game_object,
        _jump_mid_position,
        _jump_end_position,
        position_step,
        _jump_start_position.y,
        _jump_height,
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

Hachiko::Scripting::BossController::BossAttack
Hachiko::Scripting::BossController::GetAttackType(AttackType attack_type)
{
    BossAttack attack;
    switch (attack_type)
    {
    // COMMON ATTACKS
    case AttackType::MELEE_CLAW:
        // Make hit delay shorter than duration!
        attack.hit_delay = 0.05f;
        attack.duration = 1.5f; // 10 frames .45ms
        attack.cooldown = 0.0f;
        attack.stats.type = CombatManager::AttackType::RECTANGLE;
        attack.stats.damage = 1;
        attack.stats.knockback_distance = 0.3f;
    // If its cone use degrees on width
        attack.stats.width = 3.f;
        attack.stats.range = 2.5f;
        break;

    // HEAVY ATTACKS
    case AttackType::SWORD:
        attack.hit_delay = 0.1f;
        attack.duration = 2.f;
        attack.cooldown = 0.0f;
        attack.stats.type = CombatManager::AttackType::RECTANGLE;
        attack.stats.damage = 1;
        attack.stats.knockback_distance = 1.f;
    // If its cone use degrees on width
        attack.stats.width = 5.f;
        attack.stats.range = 3.f;
        break;

    case AttackType::HAMMER:
        attack.hit_delay = 0.1f;
        attack.duration = 3.f;
        attack.cooldown = 0.0f;
        attack.stats.type = CombatManager::AttackType::RECTANGLE;
        attack.stats.damage = 1;
        attack.stats.knockback_distance = 1.f;
    // If its cone use degrees on width
        attack.stats.width = 5.f;
        attack.stats.range = 3.f;
        break;
    }
    return attack;
}

Hachiko::Scripting::BossController::Weapon& Hachiko::Scripting::BossController::GetCurrentWeapon()
{
    return weapons[_current_weapon];
}

const Hachiko::Scripting::BossController::BossAttack& Hachiko::Scripting::BossController::GetNextAttack()
{
    // TODO: Avoid magic next attack changes

    return GetCurrentAttack();
}

const Hachiko::Scripting::BossController::BossAttack& Hachiko::Scripting::BossController::GetCurrentAttack()
{
    return GetCurrentWeapon().attacks[_attack_idx];
}

void Hachiko::Scripting::BossController::ChangeWeapon(unsigned weapon_idx)
{
    _current_weapon = weapon_idx;

    if (weapon_idx >= weapons.size())
    {
        _current_weapon = 0;
    }

    if (_current_weapon == 0)
    {
        _claw_weapon->SetActive(true);
        _sword_weapon->SetActive(false);
        _hammer_weapon->SetActive(false);
    }
    else if (_current_weapon == 1)
    {
        _claw_weapon->SetActive(false);
        _sword_weapon->SetActive(true);
        _hammer_weapon->SetActive(false);
    }
    else if (_current_weapon == 2)
    {
        _claw_weapon->SetActive(false);
        _sword_weapon->SetActive(false);
        _hammer_weapon->SetActive(true);
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
