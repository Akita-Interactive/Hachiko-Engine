#pragma once
#include "core/particles/ParticleModule.h"

namespace Hachiko
{
    class ColorParticleModule : public ParticleModule
    {
    public:
        ColorParticleModule(const std::string& name);
        ~ColorParticleModule() override = default;

        void Update(Particle&) override;
        void DrawGui() override;
    };
}
