#pragma once

#include "core/particles/ParticleSystem.h"
#include "core/particles/ParticleModifier.h"
#include <vector>

#include "OpenSimplex/OpenSimplex2S.h"

namespace Hachiko
{
    class NoiseParticleModifier : public ParticleModifier
    {
    public:
        NoiseParticleModifier(const std::string& name);
        ~NoiseParticleModifier() override;

        void Update(std::vector<Particle>&, float delta_time) override;
        void DrawGui() override;

        void Save(YAML::Node& node) const override;
        void Load(const YAML::Node& node) override;

    private:
        bool separate_axes = false;

        ParticleSystem::VariableTypeProperty strength_x{float2::one, 0.01f};
        ParticleSystem::VariableTypeProperty strength_y{float2::one, 0.01f};
        ParticleSystem::VariableTypeProperty strength_z{float2::one, 0.01f};

        ParticleSystem::VariableTypeProperty position_amount{float2::one, 0.01f};
        ParticleSystem::VariableTypeProperty rotation_amount{float2::zero, 0.01f};
        ParticleSystem::VariableTypeProperty scale_amount{float2::zero, 0.01f};

        Widgets::DragFloatConfig cfg{};

        OpenSimplex2S osn{};
    };
}
