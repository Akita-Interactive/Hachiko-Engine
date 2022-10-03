#include "scriptingUtil/gameplaypch.h"

#include "BounceEffect.h"

Hachiko::Scripting::BounceEffect::BounceEffect(GameObject* game_object)
    : Script(game_object, "Bounce effect"),
      _altitude(1.0f),
      _magnitude(0.5f),
      _speed(1.0f)
{
}

void Hachiko::Scripting::BounceEffect::OnAwake()
{
    _original_position = game_object->GetTransform()->GetGlobalPosition();
    _value = 0.0f;
}

void Hachiko::Scripting::BounceEffect::OnUpdate()
{
    static float direction = 1.0f;

    _value += (Time::DeltaTime() / _magnitude) * direction * _speed;
    auto _val = 1.0f - ((_value -1)*(_value -1));

    if (_val > 0.999f)
    {
        direction = -1.0f;
    }
    else if (_val < 0)
    {
        _val = 0;
        direction = 1.0f;
    }

    _val = Lerp(0, _magnitude, _val);
    
    const auto current_position = game_object->GetTransform()->GetGlobalPosition();
    game_object->GetTransform()->SetGlobalPosition(float3(current_position.x,
                                                          _original_position.y + _val + _altitude,
                                                          current_position.z));
}
