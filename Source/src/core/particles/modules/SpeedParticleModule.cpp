#include "core/hepch.h"
#include "core/particles/modules/SpeedParticleModule.h"

Hachiko::SpeedParticleModule::SpeedParticleModule(const std::string& name):
    ParticleModule(name, false)
{
}

void Hachiko::SpeedParticleModule::Update(std::vector<Particle>& particles)
{
    for (auto& particle : particles)
    {
        UpdateSpeedOverTime(particle);
    }
}

void Hachiko::SpeedParticleModule::DrawGui()
{
    ImGui::SliderFloat("Particle speed##speed_particle", &speed, 0.0f, 0.1f);
}

void Hachiko::SpeedParticleModule::UpdateSpeedOverTime(Particle& particle)
{
    particle.SetCurrentSpeed(speed);
}
