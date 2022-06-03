#include "core/hepch.h"
#include "core/particles/modules/SizeParticleModule.h"

Hachiko::SizeParticleModule::SizeParticleModule(const std::string& name):
    ParticleModule(name, false)
{
}

void Hachiko::SizeParticleModule::Update(std::vector<Particle>& particles)
{
    if (!IsActive())
    {
        return;
    }

    for (auto& particle : particles)
    {
        UpdateSizeOverTime(particle);
    }
}

void Hachiko::SizeParticleModule::DrawGui()
{
    ImGui::SliderFloat("Particle size x##size_particle", &size.x, 0.0f, 0.1f);
    ImGui::SliderFloat("Particle size y##size_particle", &size.y, 0.0f, 0.1f);
}

void Hachiko::SizeParticleModule::UpdateSizeOverTime(Particle& particle)
{
    float2 particle_size = particle.GetCurrentSize();
    particle_size += size;
    particle.SetCurrentSize(particle_size);
}
