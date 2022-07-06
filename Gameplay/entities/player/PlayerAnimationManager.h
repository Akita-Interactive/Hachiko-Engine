#pragma once

#include <scripting/Script.h>
#include "entities/player/PlayerController.h"

namespace Hachiko
{

class GameObject;
class ComponentAnimation;

namespace Scripting
{

class PlayerController;

class PlayerAnimationManager : public Script
{
	SERIALIZATION_METHODS(false)

public:
	PlayerAnimationManager(Hachiko::GameObject* game_object);
	~PlayerAnimationManager() override = default;

	void OnAwake() override;
	void OnStart() override;
	void OnUpdate() override;

private:
	PlayerController* _player_controller;

	SERIALIZE_FIELD(ComponentAnimation*, _animator);
	SERIALIZE_FIELD(PlayerState, _previous_state);
	SERIALIZE_FIELD(std::string, _state_string);
	SERIALIZE_FIELD(unsigned, _idle_index);
	SERIALIZE_FIELD(unsigned, _walking_index);
	SERIALIZE_FIELD(unsigned, _dashing_index);
	SERIALIZE_FIELD(unsigned, _melee_index);
	SERIALIZE_FIELD(unsigned, _ranged_index);
};
}
} // namespace Hachiko

