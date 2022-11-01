#include "scriptingUtil/gameplaypch.h"
#include "Stalagmite.h"
#include "constants/Sounds.h"
#include "components/ComponentAudioSource.h"

Hachiko::Scripting::Stalagmite::Stalagmite(GameObject* game_object)
	: Script(game_object, "Stalagmite")
{
}

void Hachiko::Scripting::Stalagmite::ActiveStalagmite()
{
	game_object->SetActive(true);
	game_object->ChangeDissolveProgress(1.0f, true);
	_obstacle_comp = _obstacle->GetComponent<ComponentObstacle>();
	_audio_source = game_object->GetComponent<ComponentAudioSource>();
}

void Hachiko::Scripting::Stalagmite::ActiveEffects()
{
	_explosion_effect->SetActive(false);
	_area_indicator->SetActive(true);
	_area_indicator->GetComponent<ComponentBillboard>()->Restart();
	_audio_source->PostEvent(Sounds::STALAGMITE_CEILING_CRACK);
}

void Hachiko::Scripting::Stalagmite::Falling(float fall_progress, const GameObject* player)
{
	float3 _stalagmite_position = GEO->GetTransform()->GetLocalPosition();
	_stalagmite_position.y = Lerp(50.f, 0.f, fall_progress);
	_explosion_effect->SetActive(true);
	_explosion_effect->ChangeTintColor(float4(1.0f, 0.0f, 0.0f, Lerp(1.0f, 0.0f, fall_progress)), true);
	GEO->GetTransform()->SetLocalPosition(_stalagmite_position);

	if (fall_progress == 1.f)
	{
		_audio_source->PostEvent(Sounds::STALAGMITE_GROUND_IMPACT);
		_obstacle_comp->AddObstacle();
		_explosion_effect->SetActive(false);
		float3 player_pos = player->GetTransform()->GetGlobalPosition();
		float3 stalagmite_pos = _obstacle_comp->GetGameObject()->GetTransform()->GetGlobalPosition();
		stalagmite_pos.y = player_pos.y;
		if (Distance(player_pos, stalagmite_pos) <= 2.0f)
		{
			float dir_module = (player_pos - stalagmite_pos).Normalize();
			float3 dir = (player_pos - stalagmite_pos).Normalized() * (2.0f - dir_module);
			float3 corrected_position = Navigation::GetCorrectedPosition(player_pos + dir, float3(2.0f, 2.0f, 2.0f));
			player->GetTransform()->SetGlobalPosition(corrected_position);
		}
	}
}

void Hachiko::Scripting::Stalagmite::Dissolved()
{

	if (_obstacle && _obstacle_comp)
	{
		_obstacle_comp->RemoveObstacle();
	}

	_stalagmite_collapsed = false;

	// After it being dissolved we move the GEO out of the way

	float3 _stalagmite_position = GEO
		->GetComponent<ComponentTransform>()->GetLocalPosition();

	_stalagmite_position.y = 50.f;

	GEO->GetComponent<ComponentTransform>()
		->SetLocalPosition(_stalagmite_position);

	game_object->SetActive(false);
}

