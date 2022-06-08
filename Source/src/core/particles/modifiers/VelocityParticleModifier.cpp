#include "core/hepch.h"
#include "core/particles/modifiers/VelocityParticleModifier.h"

Hachiko::VelocityParticleModifier::VelocityParticleModifier(const std::string& name):
    ParticleModifier(name, false)
{
    cfg.min = -10.0f;
    cfg.max = 10.0f;
    cfg.speed = 0.001f;
    cfg.format = "%.3f";
}

void Hachiko::VelocityParticleModifier::Update(std::vector<Particle>& particles)
{
    if (!IsActive())
    {
        return;
    }

    for (auto& particle : particles)
    {
        if (!particle.IsActive())
        {
            continue;
        }

        UpdateVelocityOverTime(particle);
    }
}

void Hachiko::VelocityParticleModifier::DrawGui()
{
    DragFloat("Particle velocity", speed_delta, &cfg);
}

void Hachiko::VelocityParticleModifier::Save(YAML::Node& node) const
{
    YAML::Node velocity_module = node[MODULE_VELOCITY];
    ParticleModifier::Save(velocity_module);
    velocity_module[SPEED] = speed_delta;
}

void Hachiko::VelocityParticleModifier::Load(const YAML::Node& node)
{
    ParticleModifier::Load(node[MODULE_VELOCITY]);
    speed_delta = node[MODULE_VELOCITY][SPEED].IsDefined() ? node[MODULE_VELOCITY][SPEED].as<float>() : speed_delta;
}

void Hachiko::VelocityParticleModifier::UpdateVelocityOverTime(Particle& particle)
{
    float particle_speed = particle.GetCurrentSpeed();
    particle_speed += speed_delta;
    particle.SetCurrentSpeed(particle_speed);
}
