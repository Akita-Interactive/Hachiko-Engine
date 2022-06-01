#include "core/hepch.h"
#include "core/particles/modules/SpeedParticleModule.h"

Hachiko::SpeedParticleModule::SpeedParticleModule(const std::string& name):
    ParticleModule(name, false)
{
}

void Hachiko::SpeedParticleModule::Update(Particle& particle)
{
}

void Hachiko::SpeedParticleModule::DrawGui()
{
    ImGui::TextUnformatted("Speed over lifetime content");
}
