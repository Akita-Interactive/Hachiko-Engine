#include "scriptingUtil/gameplaypch.h"

#include "HoverEffect.h"

Hachiko::Scripting::HoverEffect::HoverEffect(GameObject* game_object)
    : Script(game_object, "HoverEffect"),
      _altitude(1.0f),
      _magnitude(0.5f),
      _speed(1.0f)
{
}

void Hachiko::Scripting::HoverEffect::OnAwake()
{
    _original_position = game_object->GetTransform()->GetGlobalPosition();
    _value = 0.0f;
}

void Hachiko::Scripting::HoverEffect::OnUpdate()
{
    _value += (Time::DeltaTime() / _magnitude) * _speed;
    auto _val = cos(_value) + 1;

    _val = Lerp(0, _magnitude, _val);

    const auto current_position = game_object->GetTransform()->GetGlobalPosition();
    game_object->GetTransform()->SetGlobalPosition(float3(current_position.x,
                                                          _original_position.y + _val + _altitude,
                                                          current_position.z));
}
