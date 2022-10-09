#include "scriptingUtil/gameplaypch.h"
#include "RotationEffect.h"

Hachiko::Scripting::RotationEffect::RotationEffect(GameObject* game_object)
    : Script(game_object, "RotationEffect"),
      _rotate_x(false),
      _rotate_y(true),
      _rotate_z(false),
      _rotation_speed(1.0f)
{
}

void Hachiko::Scripting::RotationEffect::OnUpdate()
{
    float3 rotation = game_object->GetTransform()->GetLocalRotationEuler();
    
    if (_rotate_x)
    {
        rotation.x += Time::DeltaTime() * _rotation_speed;
    }
    if (_rotate_y)
    {
        rotation.y += Time::DeltaTime() * _rotation_speed;
    }
    if (_rotate_z)
    {
        rotation.z += Time::DeltaTime() * _rotation_speed;
    }

    game_object->GetTransform()->SetLocalRotationEuler(rotation);
}
