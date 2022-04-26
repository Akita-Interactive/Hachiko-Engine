#include "core/hepch.h"
#include "core/GameObject.h"

Hachiko::Component::Component(const Type type, GameObject* container, UID id) 
    : game_object(container)
    , type(type)
    , uid(id)
{
    if (!uid)
    {
        uid = UUID::GenerateUID();
    }
}

bool Hachiko::Component::CanBeRemoved() const
{
    return !HasDependentComponents(game_object);
}

bool Hachiko::Component::HasDependentComponents(GameObject* game_object) const
{
    return false;
}
