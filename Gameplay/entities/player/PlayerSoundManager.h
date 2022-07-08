#pragma once
#include <scripting/Script.h>
#include "entities/player/PlayerController.h"

namespace Hachiko
{

class GameObject;
class ComponentAudioSource;

namespace Scripting
{

class PlayerSoundManager : public Script
{

	SERIALIZATION_METHODS(false)

public:
	PlayerSoundManager(Hachiko::GameObject* game_object);
	~PlayerSoundManager() override = default;

	void OnAwake() override;
	void OnUpdate() override;

private:
	PlayerController* _player_controller;

	SERIALIZE_FIELD(ComponentAudioSource*, _audio_source);
	SERIALIZE_FIELD(float, _step_frequency);
	SERIALIZE_FIELD(float, _melee_frequency);
	SERIALIZE_FIELD(float, _ranged_frequency);
	SERIALIZE_FIELD(float, _timer);
	SERIALIZE_FIELD(PlayerState, _previous_state);

	float _current_frequency;
};
} // namespace Scripting
} // namespace Hachiko


