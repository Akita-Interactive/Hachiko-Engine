#pragma once

#include "core/particles/ParticleModule.h"

namespace Hachiko
{
    class SizeParticleModule : public ParticleModule
    {
    public:
        SizeParticleModule(const std::string& name);
        ~SizeParticleModule() override = default;

        void Update(std::vector<Particle>&) override;
        void DrawGui() override;
    };
}
