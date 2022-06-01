#include "core/hepch.h"
#include "core/particles/modules/ForceParticleModule.h"

Hachiko::ForceParticleModule::ForceParticleModule(const std::string& name) :
    ParticleModule(name, false)
{
}

void Hachiko::ForceParticleModule::Update(Particle& particle)
{
}

void Hachiko::ForceParticleModule::DrawGui()
{
    ImGui::TextUnformatted("Force over lifetime content");
}
