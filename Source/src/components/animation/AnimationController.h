#pragma once

#include <Math.h>

#include <vector>

namespace Hachiko
{
    class AnimationController
    {
    public:
        AnimationController();
        ~AnimationController();

        void Play();
        void Stop();
        void Update();

        bool GetTransform(const std::string& channel_name, math::float3& position, Quat& rotation) const;

    private:
        float3 Interpolate(const float3& first, const float3& second, float lambda);
        Quat Interpolate(const Quat& first, const Quat& second, float lambda) const;
    };
} // namespace Hachiko
