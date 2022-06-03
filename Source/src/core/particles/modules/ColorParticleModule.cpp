#include "core/hepch.h"
#include "core/particles/modules/ColorParticleModule.h"
#include "components/ComponentParticleSystem.h"

Hachiko::ColorParticleModule::ColorParticleModule(const std::string& name) :
    ParticleModule(name, false)
{
    gradient = new ImGradient();
}

Hachiko::ColorParticleModule::~ColorParticleModule()
{
    delete gradient;
}

void Hachiko::ColorParticleModule::Update(std::vector<Particle>& particles)
{
    if (!IsActive())
    {
        return;
    }

    for (auto& particle : particles)
    {
        UpdateColorOverTime(particle);
    }
}

void Hachiko::ColorParticleModule::DrawGui()
{
    ImGui::PushItemWidth(200);
    ImGui::GradientEditor(gradient, draggingGradient, selectedGradient);
    ImGui::PushItemWidth(150);
    ImGui::NewLine();
    ImGui::DragInt("Cycles over lifetime##color_cycles", &color_cycles, 1, 1, inf);
}

void Hachiko::ColorParticleModule::UpdateColorOverTime(Particle& particle)
{
    float particle_life = particle.GetEmitter()->GetParticlesLife().values.x;
    float color_frame = 1 - (particle.GetCurrentLife() / particle_life);
    float4 color = float4::one;
    gradient->getColorAt(color_frame, color.ptr());
    particle.SetCurrentColor(color);
}

