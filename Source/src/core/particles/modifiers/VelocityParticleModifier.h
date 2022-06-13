#pragma once

#include "core/particles/ParticleModifier.h"

namespace Hachiko
{
    class VelocityParticleModifier : public ParticleModifier
    {
    public:
        VelocityParticleModifier(const std::string& name);
        ~VelocityParticleModifier() override = default;

        void Update(std::vector<Particle>&) override;
        void DrawGui() override;

        void Save(YAML::Node& node) const override;
        void Load(const YAML::Node& node) override;

    private:
        ParticleSystem::VariableTypeProperty delta_speed{float2::zero, 0.001f};
        Widgets::DragFloatConfig cfg;
        void UpdateVelocityOverTime(Particle& particle);
    };
}
