#pragma once

#include "core/particles/ParticleModule.h"

namespace Hachiko
{
    class SpeedParticleModule : public ParticleModule
    {
    public:
        SpeedParticleModule(const std::string& name);
        ~SpeedParticleModule() override = default;

        void Update(ParticleSystem::Particle&) override;
        void DrawGui() override;
    };
}
