#pragma once

#include "core/particles/ParticleSystem.h"
#include "core/particles/ParticleModule.h"
#include "ui/widgets/DragFloat.h"

namespace Hachiko
{
    class SizeParticleModule : public ParticleModule
    {
    public:
        SizeParticleModule(const std::string& name);
        ~SizeParticleModule() override = default;

        void Update(std::vector<Particle>&) override;
        void DrawGui() override;

        void Save(YAML::Node& node) const override;
        void Load(const YAML::Node& node) override;

    private:
        ParticleSystem::VariableTypeProperty size {float2::zero, true, false};
        Widgets::DragFloatConfig cfg;
        void UpdateSizeOverTime(Particle& particle) const;
    };
}
