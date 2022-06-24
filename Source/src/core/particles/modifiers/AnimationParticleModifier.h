#pragma once

#include "core/particles/ParticleModifier.h"

namespace Hachiko
{
    class ResourceTexture;

    class AnimationParticleModifier : public ParticleModifier
    {
    public:
        AnimationParticleModifier(const std::string& name);
        ~AnimationParticleModifier() override = default;

        void Update(std::vector<Particle>&) override;
        void DrawGui() override;

        void Save(YAML::Node& node) const override;
        void Load(const YAML::Node& node) override;

    private:
        Widgets::DragFloatConfig cfg;
        bool fit_to_lifetime = false;
        float animation_speed = 0.0f;
        float blend_factor = 0.0f;
        float time = 0.0f;
        void UpdateAnimation(Particle& particle);
    };
}