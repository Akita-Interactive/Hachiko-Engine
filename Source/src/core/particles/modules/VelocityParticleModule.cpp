#include "core/hepch.h"
#include "core/particles/modules/VelocityParticleModule.h"

Hachiko::VelocityParticleModule::VelocityParticleModule(const std::string& name):
    ParticleModule(name, false)
{
    cfg.min = -10.0f;
    cfg.max = 10.0f;
    cfg.speed = 0.001f;
    cfg.format = "%.3f";
}

void Hachiko::VelocityParticleModule::Update(std::vector<Particle>& particles)
{
    for (auto& particle : particles)
    {
        if (!particle.IsActive())
        {
            continue;
        }

        UpdateVelocityOverTime(particle);
    }
}

void Hachiko::VelocityParticleModule::DrawGui()
{
    DragFloat("Particle velocity", speed, &cfg);
}

void Hachiko::VelocityParticleModule::Save(YAML::Node& node) const
{
    YAML::Node velocity_module = node[MODULE_VELOCITY];
    ParticleModule::Save(velocity_module);
    velocity_module[SPEED] = speed;
}

void Hachiko::VelocityParticleModule::Load(const YAML::Node& node)
{
    ParticleModule::Load(node[MODULE_VELOCITY]);
    speed = node[MODULE_VELOCITY][SPEED].IsDefined() ? node[MODULE_VELOCITY][SPEED].as<float>() : speed;
}

void Hachiko::VelocityParticleModule::UpdateVelocityOverTime(Particle& particle)
{
    particle.SetCurrentSpeed(speed);
}
