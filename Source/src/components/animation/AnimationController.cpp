#include "core/hepch.h"
#include "AnimationController.h"

Hachiko::AnimationController::AnimationController() {}

Hachiko::AnimationController::~AnimationController() {}

void Hachiko::AnimationController::Play() {}

void Hachiko::AnimationController::Stop() {}

void Hachiko::AnimationController::Update() {}

bool Hachiko::AnimationController::GetTransform(const std::string& channel_name, math::float3& position, Quat& rotation) const
{
    return false;
}

float3 Hachiko::AnimationController::Interpolate(const float3& first, const float3& second, float lambda)
{
    return first * (1.0f - lambda) + second * lambda;
}

Quat Hachiko::AnimationController::Interpolate(const Quat& first, const Quat& second, float lambda) const
{
    Quat result;
    float dot = first.Dot(second);

    if (dot >= 0.0f) // Interpolate through the shortest path
    {
        result.x = first.x * (1.0f - lambda) + second.x * lambda;
        result.y = first.y * (1.0f - lambda) + second.y * lambda;
        result.z = first.z * (1.0f - lambda) + second.z * lambda;
        result.w = first.w * (1.0f - lambda) + second.w * lambda;
    }
    else
    {
        result.x = first.x * (1.0f - lambda) - second.x * lambda;
        result.y = first.y * (1.0f - lambda) - second.y * lambda;
        result.z = first.z * (1.0f - lambda) - second.z * lambda;
        result.w = first.w * (1.0f - lambda) - second.w * lambda;
    }

    result.Normalize();

    return result;
}