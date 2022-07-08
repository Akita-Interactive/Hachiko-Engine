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
        ParticleSystem::VariableTypeProperty rotation_delta {float2::zero, 0.1f};
        ParticleSystem::VariableTypeProperty x_force {float2::zero, 0.1f};
        ParticleSystem::VariableTypeProperty y_force {float2::zero, 0.1f};
        ParticleSystem::VariableTypeProperty z_force {float2::zero, 0.1f};
        Widgets::DragFloatConfig cfg;

        void UpdatePositionOverTime(Particle& particle);
        void UpdateDirectionOverTime(Particle& particle);
        void UpdateRotationOverTime(Particle& particle);
    };
}
