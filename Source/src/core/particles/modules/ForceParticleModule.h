#pragma once
#include "core/particles/ParticleModule.h"

namespace Hachiko
{
    class ForceParticleModule : public ParticleModule
    {
    public:
        ForceParticleModule(const std::string& name);
        ~ForceParticleModule() override = default;

        void Update(std::vector<Particle>&) override;
        void DrawGui() override;
        
        void Save(YAML::Node& node) const override;
        void Load(const YAML::Node& node) override;

    private:
        void UpdatePositionOverTime(Particle& particle);
        void UpdateDirectionOverTime(Particle& particle);
    };
}
