#include "scriptingUtil/gameplaypch.h"
#include "TriggerAnim.h"


Hachiko::Scripting::TriggerAnim::TriggerAnim(GameObject* game_object)
	: Script(game_object, "TriggerAnim")
{
}

void Hachiko::Scripting::TriggerAnim::OnAwake()
{
}

void Hachiko::Scripting::TriggerAnim::OnStart()
{
	animation = game_object->GetComponent<ComponentAnimation>();
	animation->StartAnimating();
}

void Hachiko::Scripting::TriggerAnim::OnUpdate()
{
}
