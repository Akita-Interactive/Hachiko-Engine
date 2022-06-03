#pragma once
#include "core/particles/ParticleModule.h"

namespace Hachiko
{
    class ColorParticleModule : public ParticleModule
    {
    public:
        ColorParticleModule(const std::string& name);
        ~ColorParticleModule() override;

        void Update(std::vector<Particle>&) override;
        void DrawGui() override;

    private:
        bool color_loop = false;
        int color_cycles = 1;
        ImGradient* gradient = nullptr;
        ImGradientMark* draggingGradient = nullptr;
        ImGradientMark* selectedGradient = nullptr;

        void UpdateColorOverTime(Particle& particle);
    };
}
