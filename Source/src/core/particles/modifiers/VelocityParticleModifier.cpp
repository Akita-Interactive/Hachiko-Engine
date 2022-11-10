#include "core/hepch.h"
#include "core/particles/modifiers/VelocityParticleModifier.h"

Hachiko::VelocityParticleModifier::VelocityParticleModifier(const std::string& name):
    ParticleModifier(name, false)
{
    cfg.min = -100.0f;
    cfg.max = 100.0f;
    cfg.speed = 0.05f;
    cfg.format = "%.2f";
}

void Hachiko::VelocityParticleModifier::Update(std::vector<Particle>& particles, const float delta_time)
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
    MultiTypeSelector("Particle velocity", delta_speed, &cfg);
}

void Hachiko::VelocityParticleModifier::Save(YAML::Node& node) const
{
    YAML::Node velocity_module = node[MODIFIER_VELOCITY];
    ParticleModifier::Save(velocity_module);
    velocity_module[SPEED] = delta_speed;
}

void Hachiko::VelocityParticleModifier::Load(const YAML::Node& node)
{
    ParticleModifier::Load(node[MODIFIER_VELOCITY]);
    delta_speed = node[MODIFIER_VELOCITY][SPEED].IsDefined() ? node[MODIFIER_VELOCITY][SPEED].as<ParticleSystem::VariableTypeProperty>() : delta_speed;
}

void Hachiko::VelocityParticleModifier::UpdateVelocityOverTime(Particle& particle)
{
    float particle_speed = particle.GetCurrentSpeed();
    particle_speed += delta_speed.GetValue(particle.GetCurrentLifeNormalized());
    particle.SetCurrentSpeed(particle_speed);
}
