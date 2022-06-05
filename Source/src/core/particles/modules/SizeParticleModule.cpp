#include "core/hepch.h"
#include "core/particles/modules/SizeParticleModule.h"

#include "ui/widgets/MultiTypeSelector.h"

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
    Widgets::MultiTypeSelector("Size over lifetime", size);
}

void Hachiko::SizeParticleModule::UpdateSizeOverTime(Particle& particle)
{
    float2 particle_size = particle.GetCurrentSize();
    particle_size += size.values;
    particle.SetCurrentSize(particle_size);
}
