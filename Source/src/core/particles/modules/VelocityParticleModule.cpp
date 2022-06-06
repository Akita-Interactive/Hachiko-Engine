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
    Widgets::DragFloat("Particle velocity", speed, &cfg);
}

void Hachiko::VelocityParticleModule::Save(YAML::Node& node) const
{
    node[VELOCITY_OVER_TIME] = speed;
}

void Hachiko::VelocityParticleModule::Load(const YAML::Node& node)
{
    speed = node[VELOCITY_OVER_TIME].IsDefined() ? node[VELOCITY_OVER_TIME].as<float>() : speed;
}

void Hachiko::VelocityParticleModule::UpdateVelocityOverTime(Particle& particle)
{
    particle.SetCurrentSpeed(speed);
}
