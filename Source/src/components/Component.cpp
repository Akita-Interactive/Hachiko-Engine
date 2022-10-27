#include "core/hepch.h"
#include "core/GameObject.h"

Hachiko::Component::Component(const Type type, GameObject* container, UID id, bool updateable) 
    : game_object(container)
    , type(type), uid(id), 
    updateable(updateable)
{
    if (!uid)
    {
        uid = UUID::GenerateUID();
    }

    if (updateable)
    {
        game_object->AddUpdateableComponent();
    }
}

bool Hachiko::Component::IsActive() const
{
    return active;
}

bool Hachiko::Component::CanBeRemoved() const
{
    return !HasDependentComponents(game_object);
}

bool Hachiko::Component::HasDependentComponents(GameObject* game_object) const
{
    return false;
}

void Hachiko::Component::SetUpdateable(bool v)
{
    if (updateable == v)
    {
        return;
    }
    updateable = v;
    if (updateable)
    {
        game_object->AddUpdateableComponent();
        return;
    }
    game_object->RemoveUpdateableComponent();
}

bool Hachiko::Component::IsUpdateable() const
{
    return updateable;
}
