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
        if (!particle.IsActive())
        {
            continue;
        }

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

void Hachiko::ColorParticleModule::Save(YAML::Node& node) const
{
    node[COLOR_GRADIENT] = *gradient;
    node[COLOR_CYCLES] = color_cycles;
}

void Hachiko::ColorParticleModule::Load(const YAML::Node& node)
{
    color_cycles = node[COLOR_CYCLES].IsDefined() ? node[COLOR_CYCLES].as<int>() : 0;

    if (node[COLOR_GRADIENT].IsDefined())
    {
        gradient->clearMarks();
        for (int i = 0; i < node[COLOR_GRADIENT].size(); ++i)
        {
            auto mark = node[COLOR_GRADIENT][i].as<ImGradientMark>();
            gradient->addMark(mark.position, ImColor(mark.color[0], mark.color[1], mark.color[2], mark.color[3]));
        }
    }
}

void Hachiko::ColorParticleModule::UpdateColorOverTime(Particle& particle)
{
    float particle_life = particle.GetInitialLife();
    float color_frame = 1 - (particle.GetCurrentLife() / particle_life);
    float4 color = float4::one;
    gradient->getColorAt(color_frame, color.ptr());
    particle.SetCurrentColor(color);
}

