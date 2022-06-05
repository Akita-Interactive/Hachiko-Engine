#pragma once

#include "core/particles/ParticleModule.h"

namespace Hachiko
{
    class VelocityParticleModule : public ParticleModule
    {
    public:
        VelocityParticleModule(const std::string& name);
        ~VelocityParticleModule() override = default;

        void Update(std::vector<Particle>&) override;
        void DrawGui() override;
    private:
        float speed = 0.0f;
        void UpdateVelocityOverTime(Particle& particle);
    };
}
