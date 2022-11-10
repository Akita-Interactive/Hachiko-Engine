#pragma once
#include "core/particles/ParticleModifier.h"

namespace Hachiko
{
    class ForceParticleModifier : public ParticleModifier
    {
    public:
        ForceParticleModifier(const std::string& name);
        ~ForceParticleModifier() override = default;

        void Update(std::vector<Particle>&, float delta_time) override;
        void DrawGui() override;
        void Save(YAML::Node& node) const override;
        void Load(const YAML::Node& node) override;

    private:
        static void UpdatePositionOverTime(Particle& particle, float delta_time);
        void UpdateDirectionOverTime(Particle& particle, float delta_time) const;
        void UpdateRotationOverTime(Particle& particle, float delta_time) const;

    private:
        ParticleSystem::VariableTypeProperty rotation_delta {float2::zero, 0.1f};
        ParticleSystem::VariableTypeProperty x_force {float2::zero, 0.1f};
        ParticleSystem::VariableTypeProperty y_force {float2::zero, 0.1f};
        ParticleSystem::VariableTypeProperty z_force {float2::zero, 0.1f};
        Widgets::DragFloatConfig cfg;

    };
}
