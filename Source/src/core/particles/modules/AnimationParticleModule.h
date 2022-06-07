#pragma once

#include "core/particles/ParticleModule.h"

namespace Hachiko
{
    class ResourceTexture;

    class AnimationParticleModule : public ParticleModule
    {
    public:
        AnimationParticleModule(const std::string& name);
        ~AnimationParticleModule() override = default;

        void Update(std::vector<Particle>&) override;
        void DrawGui() override;

        void Save(YAML::Node& node) const override;
        void Load(const YAML::Node& node) override;

    private:
        Widgets::DragFloatConfig cfg;
        bool fit_to_lifetime = false;
        float skip_frames = 0.0f;
        float time = 0.0f;
        void UpdateAnimation(Particle& particle);
    };
}