#include "core/hepch.h"
#include "core/particles/modules/SpeedParticleModule.h"

Hachiko::SpeedParticleModule::SpeedParticleModule(const std::string& name):
    ParticleModule(name, false)
{
}

void Hachiko::SpeedParticleModule::Update(std::vector<Particle>& particles)
{
    ParticleModule::Update(particles);
}

void Hachiko::SpeedParticleModule::DrawGui()
{
    ImGui::TextUnformatted("Speed over lifetime content");
}
