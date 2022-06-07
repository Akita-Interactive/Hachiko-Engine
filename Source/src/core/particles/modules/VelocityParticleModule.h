#pragma once

#include "core/particles/ParticleModule.h"

namespace Hachiko
{
    class VelocityParticleModule : public ParticleModule
    {
    public:
        VelocityParticleModule(const std::string& name);
        ~VelocityParticleModule() override = default;

        void Update(std::vector<Particle>&) override;
        void DrawGui() override;

        void Save(YAML::Node& node) const override;
        void Load(const YAML::Node& node) override;
    
    private:
        float speed_delta = 0.0f;
        Widgets::DragFloatConfig cfg;
        void UpdateVelocityOverTime(Particle& particle);
    };
}