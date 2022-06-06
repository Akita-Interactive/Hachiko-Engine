#include "core/hepch.h"
#include "core/particles/modules/VelocityParticleModule.h"

Hachiko::VelocityParticleModule::VelocityParticleModule(const std::string& name):
    ParticleModule(name, false)
{
    cfg.min = -10.0f;
    cfg.max = 10.0f;
    cfg.speed = 0.01f;
    cfg.format = "%.2f";
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

void Hachiko::VelocityParticleModule::UpdateVelocityOverTime(Particle& particle)
{
    particle.SetCurrentSpeed(speed * 0.1f);
}
