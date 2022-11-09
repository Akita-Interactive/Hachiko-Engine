#include "scriptingUtil/gameplaypch.h"
#include "entities/TimeScaleModeManager.h"

Hachiko::Scripting::TimeScaleModeManager::TimeScaleModeManager(
	GameObject* game_object)
	: Script(game_object, "TimeScaleModeManager")
	, _is_scaled(false)
	, _is_recursive(false)
{
}

void Hachiko::Scripting::TimeScaleModeManager::OnAwake()
{
	SetScaled(_is_scaled, _is_recursive);
}

void Hachiko::Scripting::TimeScaleModeManager::SetScaled(
	const bool is_scaled, 
	const bool is_recursive)
{
	_is_scaled = is_scaled;
	_is_recursive = is_recursive;

	const TimeScaleMode mode = _is_scaled
		? TimeScaleMode::SCALED
		: TimeScaleMode::UNSCALED;

	game_object->SetTimeScaleMode(mode);

	if (!_is_recursive)
	{
		return;
	}

	const std::vector<Component*> descendant_transforms =
		game_object->GetComponentsInDescendants(
			Component::Type::TRANSFORM);

	for (const Component* transform : descendant_transforms)
	{
		transform->GetGameObject()->SetTimeScaleMode(mode);
	}
}

bool Hachiko::Scripting::TimeScaleModeManager::IsScaled() const
{
	return _is_scaled;
}