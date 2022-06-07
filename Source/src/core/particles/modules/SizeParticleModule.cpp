#include "core/hepch.h"
#include "core/particles/modules/SizeParticleModule.h"

#include "ui/widgets/MultiTypeSelector.h"

Hachiko::SizeParticleModule::SizeParticleModule(const std::string& name):
    ParticleModule(name, false)
{
    cfg.min = -1.0f;
    cfg.max = 1.0f;
    cfg.speed = 0.001f;
    cfg.format = "%.3f";
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
    MultiTypeSelector("Size", size, &cfg);
}

void Hachiko::SizeParticleModule::Save(YAML::Node& node) const
{
    YAML::Node size_module = node[MODULE_SIZE];
    ParticleModule::Save(size_module);
    size_module[SIZE] = size;
}

void Hachiko::SizeParticleModule::Load(const YAML::Node& node)
{
    ParticleModule::Load(node[MODULE_SIZE]);
    size = node[MODULE_SIZE][SIZE].IsDefined() ? node[MODULE_SIZE][SIZE].as<ParticleSystem::VariableTypeProperty>() : size;
}

void Hachiko::SizeParticleModule::UpdateSizeOverTime(Particle& particle) const
{
    float2 particle_size = particle.GetCurrentSize();
    particle_size += size.values;
    particle.SetCurrentSize(particle_size);
}
