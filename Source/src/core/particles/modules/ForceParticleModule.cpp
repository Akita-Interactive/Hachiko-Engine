#include "core/hepch.h"
#include "core/particles/modules/ForceParticleModule.h"

Hachiko::ForceParticleModule::ForceParticleModule(const std::string& name) :
    ParticleModule(name, false)
{
}

void Hachiko::ForceParticleModule::Update(std::vector<Particle>& particles)
{
    for (auto& particle : particles)
    {
        if (!particle.IsActive())
        {
            continue;
        }

        UpdateDirectionOverTime(particle);
        UpdatePositionOverTime(particle);
    }
}

void Hachiko::ForceParticleModule::DrawGui()
{
    ImGui::TextUnformatted("Force over lifetime content");
}

void Hachiko::ForceParticleModule::Save(YAML::Node& node) const
{
    YAML::Node force_module = node[MODULE_FORCE];
    ParticleModule::Save(force_module);
}

void Hachiko::ForceParticleModule::Load(const YAML::Node& node)
{
    ParticleModule::Load(node[MODULE_FORCE]);
}

void Hachiko::ForceParticleModule::UpdatePositionOverTime(Particle& particle) 
{
    float3 position = particle.GetCurrentPosition();
    position += particle.GetCurrentDirection() * particle.GetCurrentSpeed();
    particle.SetCurrentPosition(position);
}

void Hachiko::ForceParticleModule::UpdateDirectionOverTime(Particle& particle)
{
    //float3 direction = particle.GetCurrentDirection();
    //direction = float3(Random::RandomFloat(), Random::RandomFloat(), Random::RandomFloat());
    //direction.Normalize();
    //particle.SetCurrentDirection(direction);
}
