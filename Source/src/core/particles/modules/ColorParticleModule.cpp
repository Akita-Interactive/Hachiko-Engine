#include "core/hepch.h"
#include "core/particles/modules/ColorParticleModule.h"

Hachiko::ColorParticleModule::ColorParticleModule(const std::string& name) :
    ParticleModule(name, false)
{
}

void Hachiko::ColorParticleModule::Update(Particle& particle)
{
}

void Hachiko::ColorParticleModule::DrawGui()
{
    ImGui::TextUnformatted("Color over lifetime content");
}
