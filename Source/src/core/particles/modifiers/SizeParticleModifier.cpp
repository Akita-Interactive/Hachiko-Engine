#include "core/hepch.h"
#include "core/particles/modifiers/SizeParticleModifier.h"

#include "ui/widgets/MultiTypeSelector.h"

Hachiko::SizeParticleModifier::SizeParticleModifier(const std::string& name):
    ParticleModifier(name, false)
{
    cfg.min = -100.0f;
    cfg.max = 100.0f;
    cfg.speed = 0.1f;
    cfg.format = "%.2f";
    cfg.ui_factor = {100.0f, 0.01f};
}

void Hachiko::SizeParticleModifier::Update(std::vector<Particle>& particles)
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

void Hachiko::SizeParticleModifier::DrawGui()
{
    MultiTypeSelector("Size", size, &cfg);
}

void Hachiko::SizeParticleModifier::Save(YAML::Node& node) const
{
    YAML::Node size_module = node[MODULE_SIZE];
    ParticleModifier::Save(size_module);
    size_module[SIZE] = size;
}

void Hachiko::SizeParticleModifier::Load(const YAML::Node& node)
{
    ParticleModifier::Load(node[MODULE_SIZE]);
    size = node[MODULE_SIZE][SIZE].IsDefined() ? node[MODULE_SIZE][SIZE].as<ParticleSystem::VariableTypeProperty>() : size;
}

void Hachiko::SizeParticleModifier::UpdateSizeOverTime(Particle& particle) const
{
    float particle_size = particle.GetCurrentSize();
    particle_size += size.GetValue();
    if (particle_size < 0.0f)
    {
        particle_size = 0.0f;
    }

    particle.SetCurrentSize(particle_size);
}
