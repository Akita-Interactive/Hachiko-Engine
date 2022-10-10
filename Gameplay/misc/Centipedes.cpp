#include "scriptingUtil/gameplaypch.h"
#include "Centipedes.h"
#include "constants/Scenes.h"


Hachiko::Scripting::Centipedes::Centipedes(GameObject* game_object)
	: Script(game_object, "Centipedes")
{
}

void Hachiko::Scripting::Centipedes::OnAwake()
{
	_player = Scenes::GetPlayer();
	player_transform = _player->GetComponent<ComponentTransform>();
	agent = game_object->GetComponent<ComponentAgent>();
	animation = game_object->GetComponent<ComponentAnimation>();
	_splash_effect_billboard = _damage_effect->GetComponent<ComponentBillboard>();
	audio_source = game_object->GetComponent<ComponentAudioSource>();

	animation->StartAnimating();
	agent->SetMaxSpeed(3.0f);
}

void Hachiko::Scripting::Centipedes::OnUpdate()
{
	if (_state == CentipedeState::DIE && animation->IsAnimationStopped())
	{
		before_disapear += Time::DeltaTime();
		if (before_disapear >= time_to_disapear)
		{
			game_object->SetActive(false);
		}
	}

	float3 position = game_object->GetTransform()->GetGlobalPosition();
	float3 player_position = player_transform->GetGlobalPosition();
	float3 player_to_centipide = position - player_position;

	float distance_sq = player_to_centipide.LengthSq();

	if (distance_sq < stepped_on_range && _state != CentipedeState::DIE) // DEAD
	{
		SteppedOn();
	}

	if (_state == CentipedeState::IDLE)
	{
		if (distance_sq < centipide_aggro) {
			_state = CentipedeState::RUN;
		}
	}
	else if (_state == CentipedeState::RUN)
	{
		if (distance_sq > centipide_aggro) {
			_state = CentipedeState::IDLE;
			agent->SetTargetPosition(game_object->GetComponent<ComponentTransform>()->GetGlobalPosition());
		}
		else
		{
			float3 new_destination = player_position + player_to_centipide.Normalized() * player_range;
			Quat target_rotation = Quat::LookAt(float3(-1, 0, 0), player_transform->GetFront(), float3(0, 1, 0), float3(0, 1, 0));
			Quat rotation = Quat::Slerp(game_object->GetTransform()->GetGlobalRotation(), target_rotation, Min(Time::DeltaTime() / Max(rotation_smoothness, 0.000001f), 1.0f));

			game_object->GetTransform()->SetGlobalRotation(rotation);
			agent->SetTargetPosition(new_destination);
		}
	}

	UpdateCentipedeState();

	return;
}

void Hachiko::Scripting::Centipedes::SteppedOn()
{
	audio_source->PostEvent(Sounds::PLAY_SPLASH);
	_damage_effect->SetActive(true);
	_splash_effect_billboard->Play();
	agent->Stop();
	_state = CentipedeState::DIE;

}

void Hachiko::Scripting::Centipedes::UpdateCentipedeState()
{
	CentipedeState centipide_state = GetState();
	bool state_changed = centipide_state != _previous_state;

	if (!state_changed)
	{
		return;
	}

	_previous_state = _state;

	switch (_state)
	{
	case CentipedeState::IDLE:
		animation->SendTrigger("isIdle");
		break;
	case CentipedeState::RUN:
		animation->SendTrigger("isRun");
		break;
	case CentipedeState::DIE:
		animation->SendTrigger("isDie");
		break;
	default:
		break;
	}
}

