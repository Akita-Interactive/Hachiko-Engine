#pragma once
#include "core/particles/ParticleModifier.h"

namespace Hachiko
{
    class ForceParticleModifier : public ParticleModifier
    {
    public:
        ForceParticleModifier(const std::string& name);
        ~ForceParticleModifier() override = default;

        void Update(std::vector<Particle>&) override;
        void DrawGui() override;
        
        void Save(YAML::Node& node) const override;
        void Load(const YAML::Node& node) override;

    private:
        void UpdatePositionOverTime(Particle& particle);
        void UpdateDirectionOverTime(Particle& particle);
    };
}
