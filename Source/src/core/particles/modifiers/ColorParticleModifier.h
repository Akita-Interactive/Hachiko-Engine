#pragma once
#include "core/particles/ParticleModifier.h"

namespace Hachiko
{
    class ColorParticleModifier : public ParticleModifier
    {
    public:
        ColorParticleModifier(const std::string& name);
        ~ColorParticleModifier() override;

        void Update(std::vector<Particle>&) override;
        void DrawGui() override;
                
        void Save(YAML::Node& node) const override;
        void Load(const YAML::Node& node) override;

    private:
        int color_cycles = 1;
        ImGradient* gradient = nullptr;
        ImGradientMark* draggingGradient = nullptr;
        ImGradientMark* selectedGradient = nullptr;

        void UpdateColorOverTime(Particle& particle) const;
    };
}
