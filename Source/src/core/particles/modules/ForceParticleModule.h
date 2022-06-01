#pragma once
#include "core/particles/ParticleModule.h"

namespace Hachiko
{
    class ForceParticleModule : public ParticleModule
    {
    public:
        ForceParticleModule(const std::string& name);
        ~ForceParticleModule() override = default;

        void Update(Particle&) override;
        void DrawGui() override;
    };
}
