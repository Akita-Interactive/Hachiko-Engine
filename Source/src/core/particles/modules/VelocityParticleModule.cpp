#include "core/hepch.h"
#include "core/particles/modules/VelocityParticleModule.h"

Hachiko::VelocityParticleModule::VelocityParticleModule(const std::string& name):
    ParticleModule(name, false)
{
}

void Hachiko::VelocityParticleModule::Update(std::vector<Particle>& particles)
{
    for (auto& particle : particles)
    {
        UpdateVelocityOverTime(particle);
    }
}

void Hachiko::VelocityParticleModule::DrawGui()
{
    ImGui::SliderFloat("Particle velocity##velocity_particle", &speed, 0.0f, 0.1f);
}

void Hachiko::VelocityParticleModule::UpdateVelocityOverTime(Particle& particle)
{
    particle.SetCurrentSpeed(speed);
}
