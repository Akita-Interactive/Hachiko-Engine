#include "scriptingUtil/gameplaypch.h"
#include "entities/player/RoomTeleporter.h"

// NOTE FOR BERNAT: To scripts be able to work, you need to define it's 
// constructor properly. Note the name field that is same with the class name.
Hachiko::Scripting::RoomTeleporter::RoomTeleporter(GameObject* game_object)
	: Script(game_object, "RoomTeleporter")
	, _player(nullptr)
	, _room_portal()
	, _outdoor_portal(nullptr)
	, _fade_image(nullptr)
	, _fade_duration(0.25f)
	, _trigger_distance(1.f)
{
}

void Hachiko::Scripting::RoomTeleporter::OnAwake()
{

}

void Hachiko::Scripting::RoomTeleporter::OnStart()
{
	// Don't forget, if you change any field here, it will override whatever 
	// you set from the editor, as OnLoad is called before OnAwake. As opposed
	// to OnStart, OnAwake is called again when you build scripts again at run
	// time.

	if (!_fade_image) return;

	_image_transform = _fade_image->GetGameObject()->GetComponent<ComponentTransform2D>();

	if (!_image_transform) return;

	// Center image on the screen
	float2 anchor = float2(0.5);
	_image_transform->SetAnchor(anchor);

	// Make image occupy entire screen (Scale multiplies final size)
	float2 size = float2(10.f);
	_image_transform->SetSize(size);
	float2 scale = float2(25000.f);
	_image_transform->SetScale(scale);

	// Set initial color to black without alpha
	_fade_image->SetColor(_clear_color);
}

void Hachiko::Scripting::RoomTeleporter::OnUpdate()
{
	if (!_player || !_outdoor_portal || !_room_portal || !_fade_image)
	{
		// If some reference is working the system cant work
		return;
	}
		
	if (!_already_triggered)
	{
		if (_player->GetTransform()->GetGlobalPosition().Distance(_outdoor_portal->GetTransform()->GetGlobalPosition()) <= _trigger_distance)
		{
			EnterRoom();
		}
		else if (_player->GetTransform()->GetGlobalPosition().Distance(_room_portal->GetTransform()->GetGlobalPosition()) <= _trigger_distance)
		{
			ExitRoom();
		}
	}

	if (_step == Step::WAITING)
	{
		float out_dist = _player->GetTransform()->GetGlobalPosition().Distance(_outdoor_portal->GetTransform()->GetGlobalPosition());
		float in_dist = _player->GetTransform()->GetGlobalPosition().Distance(_room_portal->GetTransform()->GetGlobalPosition());

		if (out_dist > _trigger_distance && in_dist > _trigger_distance)
		{
			_already_triggered = false;
		}
	} 
	else
	{
		SimulateTransition();

	}
}

void Hachiko::Scripting::RoomTeleporter::SetActive(bool v)
{
	_step = Step::FADE_IN;
	_already_triggered = true;
	_step_progress = 0.f;
}

void Hachiko::Scripting::RoomTeleporter::EnterRoom()
{
	_inside_room = true;
	SetActive(true);
}

void Hachiko::Scripting::RoomTeleporter::ExitRoom()
{
	_inside_room = false;
	SetActive(true);
}

void Hachiko::Scripting::RoomTeleporter::SimulateTransition()
{

	// In step logic
	if (_step == Step::FADE_IN)
	{
		_step_progress += Time::DeltaTime() / _fade_duration;
		_fade_image->SetColor(float4::Lerp(_clear_color, _opaque_color, _step_progress));
	}
	else if (_step == Step::BLACKOUT)
	{
		// Do literally nothing
		_step_progress += Time::DeltaTime() / _blackout_duration;
	}
	else if (_step == Step::FADE_OUT)
	{
		_step_progress += Time::DeltaTime() / _fade_duration;
		_fade_image->SetColor(float4::Lerp(_opaque_color, _clear_color, _step_progress));
	}

	// Step transition logic
	if (_step_progress >= 1.f)
	{
		_step_progress = 0.f;
		
		if (_step == Step::FADE_IN)
		{
			// Teleport player
			TeleportPlayer(_inside_room);
			// Toggle skybox based on indoors
			SceneManagement::SetSkyboxActive(!_inside_room);
			_step = Step::BLACKOUT;
		}
		else if (_step == Step::BLACKOUT)
		{
			_step = Step::FADE_OUT;
		}
		else if (_step == Step::FADE_OUT)
		{
			_step = Step::WAITING;
		}
	}
	
}

void Hachiko::Scripting::RoomTeleporter::TeleportPlayer(bool indoors)
{
	if (indoors)
	{
		_player->GetTransform()->SetGlobalPosition(_room_portal->GetTransform()->GetGlobalPosition());
		return;
	}
	_player->GetTransform()->SetGlobalPosition(_outdoor_portal->GetTransform()->GetGlobalPosition());
}
