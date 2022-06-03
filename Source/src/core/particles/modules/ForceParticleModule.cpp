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
        //TODO: Add UpdateDirectionOverTime();
        UpdatePositionOverTime(particle);
    }
}

void Hachiko::ForceParticleModule::DrawGui()
{
    ImGui::TextUnformatted("Force over lifetime content");
}

void Hachiko::ForceParticleModule::UpdatePositionOverTime(Particle& particle) 
{
    float3 position = particle.GetCurrentPosition();
    position.y += particle.GetCurrentSpeed(); // TODO: This should be position += particle.GetCurrentDirection() * particle.GetCurrentSpeed();
    particle.SetCurrentPosition(position);
}
