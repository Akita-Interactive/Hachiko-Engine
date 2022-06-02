#include "core/hepch.h"
#include "components/ComponentParticleSystem.h"
#include "components/ComponentTransform.h"

#include "modules/ModuleSceneManager.h"

#include "core/particles/Particle.h"
#include "core/particles/modules/SpeedParticleModule.h"
#include "core/particles/modules/SizeParticleModule.h"
#include "core/particles/modules/ColorParticleModule.h"
#include "core/particles/modules/ForceParticleModule.h"

#include "debugdraw.h"

Hachiko::ComponentParticleSystem::ComponentParticleSystem(GameObject* container) :
    Component(Type::PARTICLE_SYSTEM, container)
{
    particle_modules.push_back(std::make_shared<SpeedParticleModule>("Speed over lifetime"));
    particle_modules.push_back(std::make_shared<SizeParticleModule>("Size over lifetime"));
    particle_modules.push_back(std::make_shared<ColorParticleModule>("Color over lifetime"));
    particle_modules.push_back(std::make_shared<ForceParticleModule>("Force over lifetime"));

    std::function edit_curve = [&](Event& evt) {
        const auto data = evt.GetEventData<CurveEditorEventPayload>();
        current_curve_edition_title = data.GetTitle();
        current_curve_editing = current_curve_editing == data.GetCurve() ? nullptr : data.GetCurve();

    };
    App->event->Subscribe(Event::Type::CURVE_EDITOR, edit_curve);
}

Hachiko::ComponentParticleSystem::~ComponentParticleSystem()
{
    particle_modules.clear();
    App->scene_manager->GetActiveScene()->RemoveParticleComponent(GetID());
}

void Hachiko::ComponentParticleSystem::Start()
{
}

void Hachiko::ComponentParticleSystem::Update()
{
    if (!in_scene)
    {
        App->scene_manager->GetActiveScene()->AddParticleComponent(this);
        in_scene = true;
    }

    for (const auto& particle_module : particle_modules)
    {
        particle_module->Update(particles);
    }
}

void Hachiko::ComponentParticleSystem::Draw(ComponentCamera* camera, Program* program)
{
    for (auto& particle : particles)
    {
        particle.Draw(camera, program);
    }
}

void Hachiko::ComponentParticleSystem::DrawGui()
{
    if (ImGuiUtils::CollapsingHeader(game_object, this, "Particle system"))
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
        ImGui::Indent();
        if (CollapsingHeader("Parameters", &parameters_section, Widgets::CollapsibleHeaderType::Space))
        {
            Widgets::MultiTypeSelector("Start delay", delay);
            Widgets::MultiTypeSelector("Start lifetime", life);
            Widgets::MultiTypeSelector("Start speed", speed);
            Widgets::MultiTypeSelector("Start size", size);
            Widgets::MultiTypeSelector("Start rotation", rotation);
        }

        if (CollapsingHeader("Emission", &emission_section, Widgets::CollapsibleHeaderType::Checkbox))
        {
            ImGui::BeginDisabled(!emission_section);

            ImGui::EndDisabled();
        }

        if (CollapsingHeader("Shape", &shape_section, Widgets::CollapsibleHeaderType::Checkbox))
        {
            ImGui::BeginDisabled(!shape_section);
            const char* items[] = {"Cone", "Box", "Sphere", "Circle", "Rectangle"};

            int emitter = static_cast<int>(emitter_type);
            if (Widgets::Combo("Shape", &emitter, items, IM_ARRAYSIZE(items)))
            {
                emitter_type = static_cast<ParticleSystem::Emitter::Type>(emitter);
                App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
            }

            switch (emitter_type)
            {
            case ParticleSystem::Emitter::Type::CONE:
                //Cone specific code here
                break;
            case ParticleSystem::Emitter::Type::BOX:
                //Box specific code here
                break;
            case ParticleSystem::Emitter::Type::SPHERE:
                //Box specific code here
                break;
            case ParticleSystem::Emitter::Type::CIRCLE:
                //Box specific code here
                break;
            case ParticleSystem::Emitter::Type::RECTANGLE:
                //Box specific code here
                break;
            }

            ImGuiUtils::DisplayTooltip("Selects the shape of this particle system");
            Widgets::AxisSlider("Position", emitter_properties.position);
            Widgets::AxisSlider("Rotation", emitter_properties.rotation);
            Widgets::AxisSliderConfig scale_config;
            scale_config.min = float3(0.001f);
            AxisSlider("Scale", emitter_properties.scale, &scale_config);
            ImGui::EndDisabled();
        }

        for (const auto& particle_module : particle_modules)
        {
            if (CollapsingHeader(particle_module->GetName().c_str(), &particle_module->active, Widgets::CollapsibleHeaderType::Checkbox))
            {
                ImGui::BeginDisabled(!particle_module->IsActive());
                particle_module->DrawGui();
                ImGui::EndDisabled();
            }
        }

        if (CollapsingHeader("Collision", &collision_section, Widgets::CollapsibleHeaderType::Checkbox))
        {
            ImGui::BeginDisabled(renderer_section);

            ImGui::EndDisabled();
        }

        if (CollapsingHeader("Lights", &lights_section, Widgets::CollapsibleHeaderType::Checkbox))
        {
            ImGui::BeginDisabled(renderer_section);

            ImGui::EndDisabled();
        }

        if (CollapsingHeader("Trails", &trails_section, Widgets::CollapsibleHeaderType::Checkbox))
        {
            ImGui::BeginDisabled(renderer_section);

            ImGui::EndDisabled();
        }

        if (CollapsingHeader("Renderer", &renderer_section, Widgets::CollapsibleHeaderType::Checkbox))
        {
            ImGui::BeginDisabled(renderer_section);

            ImGui::EndDisabled();
        }

        if (current_curve_editing)
        {
            bool always_open = true;
            if (CollapsingHeader("Curve editor", &always_open, Widgets::CollapsibleHeaderType::Space, ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::TextUnformatted("Current curve:");
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
                ImGui::TextWrapped(current_curve_edition_title.c_str());
                ImGui::PopStyleColor();
                ImGui::Spacing();

                ImGui::Curve("##",
                             ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetCurrentContext()->FontSize * 5.0f),
                             ParticleSystem::CURVE_TICKS - 1,
                             reinterpret_cast<ImVec2*>(current_curve_editing));
            }
        }
        ImGui::PopStyleVar();
    }
}

void Hachiko::ComponentParticleSystem::DebugDraw()
{
    const float4x4 model = GetGameObject()->GetComponent<ComponentTransform>()->GetGlobalMatrix();
    const float4x4 emitter = float4x4::FromTRS(emitter_properties.position,
                                               Quat::FromEulerXYZ(
                                                   DegToRad(emitter_properties.rotation.x),
                                                   DegToRad(emitter_properties.rotation.y),
                                                   DegToRad(emitter_properties.rotation.z)),
                                               emitter_properties.scale);
    const float4x4 current_model = model * emitter;
    const float3 direction = (current_model.RotatePart() * float3::unitY).Normalized();

    switch (emitter_type)
    {
    case ParticleSystem::Emitter::Type::CONE:
        dd::cone(current_model.TranslatePart(), direction, dd::colors::White, emitter_properties.angle / 90, emitter_properties.radius);
        break;
    case ParticleSystem::Emitter::Type::BOX:
    {
        const OBB obb = OBB(current_model.TranslatePart(), current_model.GetScale(), current_model.RotatePart().WorldX(), current_model.RotatePart().WorldY(), current_model.RotatePart().WorldZ());
        float3 p[8];
        static const int order[8] = {0, 1, 5, 4, 2, 3, 7, 6};
        for (int i = 0; i < 8; ++i)
        {
            p[i] = obb.CornerPoint(order[i]);
        }
        dd::box(p, dd::colors::White);
    }
    break;
    case ParticleSystem::Emitter::Type::SPHERE:
        break;
    case ParticleSystem::Emitter::Type::CIRCLE:
        break;
    case ParticleSystem::Emitter::Type::RECTANGLE:
        break;
    }
}

void Hachiko::ComponentParticleSystem::Save(YAML::Node& node) const
{
}

void Hachiko::ComponentParticleSystem::Load(const YAML::Node& node)
{
}
