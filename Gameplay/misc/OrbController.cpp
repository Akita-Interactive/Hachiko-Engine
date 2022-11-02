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
		_parasite_dissolving_time_progress += Time::DeltaTimeScaled();
		float transition = (_parasite_dissolve_time - _parasite_dissolving_time_progress) / _parasite_dissolve_time;
		_orb->ChangeDissolveProgress(transition, true);
		

		if (transition < 0.f)
		{
			_orb->SetActive(false);
		}
	}
}

void Hachiko::Scripting::OrbController::DestroyOrb()
{
	cp_animation->SendTrigger("isTakeLife");
	picked = true;
}
