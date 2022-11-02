#include "scriptingUtil/gameplaypch.h"

#include "misc/OrbController.h";


Hachiko::Scripting::OrbController::OrbController(GameObject* game_object)
	: Script(game_object, "OrbController")
{}

void Hachiko::Scripting::OrbController::OnAwake()
{
	cp_animation = game_object->GetComponent<ComponentAnimation>();

	if (cp_animation) {
		cp_animation->StartAnimating();
	}
}

void Hachiko::Scripting::OrbController::OnUpdate()
{
	if (picked) {
		float transition = math::Sqrt(_parasite_dissolve_time - _parasite_dissolving_time_progress) * _parasite_dissolving;
		_orb->ChangeDissolveProgress(transition, true);
		_parasite_dissolving_time_progress += Time::DeltaTimeScaled();
	}

	if (cp_animation->IsAnimationStopped() && picked)
	{
		_orb->SetActive(false);
	}
}

void Hachiko::Scripting::OrbController::DestroyOrb()
{
	cp_animation->SendTrigger("isTakeLife");
	picked = true;
}
