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
        if (!particle.IsActive())
        {
            continue;
        }

        UpdateSizeOverTime(particle);
    }
}

void Hachiko::SizeParticleModule::DrawGui()
{
    Widgets::DragFloat("Size x", size.x);
    Widgets::DragFloat("Size y", size.y);
}

void Hachiko::SizeParticleModule::UpdateSizeOverTime(Particle& particle)
{
    float2 particle_size = particle.GetCurrentSize();
    particle_size += size;
    particle.SetCurrentSize(particle_size);
}
