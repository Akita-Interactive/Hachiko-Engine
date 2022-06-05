#include "core/hepch.h"
#include "components/ComponentParticleSystem.h"
#include "components/ComponentTransform.h"

#include "modules/ModuleSceneManager.h"

#include "ui/widgets/Widgets.h"

#include "debugdraw.h"

Hachiko::ComponentParticleSystem::ComponentParticleSystem(GameObject* container) :
    Component(Type::PARTICLE_SYSTEM, container)
{
    particle_modules.push_back(std::make_shared<VelocityParticleModule>("Velocity over lifetime"));
    particle_modules.push_back(std::make_shared<SizeParticleModule>("Size over lifetime"));
    particle_modules.push_back(std::make_shared<ColorParticleModule>("Color over lifetime"));
    particle_modules.push_back(std::make_shared<ForceParticleModule>("Force over lifetime"));

    std::function edit_curve = [&](Event& evt) {
        const auto data = evt.GetEventData<CurveEditorEventPayload>();
        current_curve_editing_title = data.GetTitle();
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
    for (auto& particle : particles)
    {
        particle.SetEmitter(this);
        particle.SetInitialLife(life.values.x);
    }
}

void Hachiko::ComponentParticleSystem::Update()
{
    if (!in_scene)
    {
        App->scene_manager->GetActiveScene()->AddParticleComponent(this);
        in_scene = true;
        Start(); // TODO: For DEBUG as Start is not called
    }

    for (auto& particle : particles)
    {
        particle.Update();
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
            Widgets::MultiTypeSelector("Rate over time", delay);
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

            Widgets::AxisSliderConfig scale_config;
            scale_config.min = float3(0.001f);

            switch (emitter_type)
            {
            case ParticleSystem::Emitter::Type::CONE:

                break;
            case ParticleSystem::Emitter::Type::BOX:

                break;
            case ParticleSystem::Emitter::Type::SPHERE:

                break;
            case ParticleSystem::Emitter::Type::CIRCLE:

                break;
            case ParticleSystem::Emitter::Type::RECTANGLE:
                scale_config.enabled = bool3(true, false, true);
                break;
            }

            ImGuiUtils::DisplayTooltip("Selects the shape of this particle system");
            Widgets::AxisSlider("Position", emitter_properties.position);
            Widgets::AxisSlider("Rotation", emitter_properties.rotation);
            Widgets::AxisSlider("Scale", emitter_properties.scale, &scale_config);
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

        if (CollapsingHeader("Lights", &lights_section, Widgets::CollapsibleHeaderType::Checkbox))
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
                ImGui::TextWrapped(current_curve_editing_title.c_str());
                ImGui::PopStyleColor();
                ImGui::Spacing();

                ImGui::Curve("##",
                             ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetCurrentContext()->FontSize * 5.0f),
                             ParticleSystem::CURVE_TICKS - 1,
                             reinterpret_cast<ImVec2*>(current_curve_editing));
            }
        }
        ImGui::PopStyleVar();
        ImGui::Unindent();
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
        dd::cone(current_model.TranslatePart(),
                 direction,
                 dd::colors::White,
                 emitter_properties.top,
                 emitter_properties.radius);
        dd::cone(current_model.TranslatePart(),
                 direction,
                 dd::colors::White,
                 emitter_properties.top * (1 - emitter_properties.radius_thickness),
                 emitter_properties.radius * (1 - emitter_properties.radius_thickness));
        break;
    case ParticleSystem::Emitter::Type::BOX:
    {
        dd::box(current_model.TranslatePart(), dd::colors::White, current_model.GetScale().x, current_model.GetScale().y, current_model.GetScale().z);
    }
    break;
    case ParticleSystem::Emitter::Type::SPHERE:
        dd::sphere(current_model.TranslatePart(), dd::colors::White, emitter_properties.radius);
        dd::sphere(current_model.TranslatePart(), dd::colors::White, emitter_properties.radius * (1 - emitter_properties.radius_thickness));
        break;
    case ParticleSystem::Emitter::Type::CIRCLE:
        dd::circle(current_model.TranslatePart(), direction, dd::colors::White, emitter_properties.radius, 50.0f);
        dd::circle(current_model.TranslatePart(), direction, dd::colors::White, emitter_properties.radius * (1 - emitter_properties.radius_thickness), 50.0f);
        break;
    case ParticleSystem::Emitter::Type::RECTANGLE:
        dd::box(current_model.TranslatePart(), dd::colors::White, current_model.GetScale().x, 0.0001f, current_model.GetScale().z);
        break;
    }
}

void Hachiko::ComponentParticleSystem::Save(YAML::Node& node) const
{
}

void Hachiko::ComponentParticleSystem::Load(const YAML::Node& node)
{
}

Hachiko::ParticleSystem::VariableTypeProperty
Hachiko::ComponentParticleSystem::GetParticlesLife() const
{
    return life;
}

Hachiko::ParticleSystem::VariableTypeProperty
Hachiko::ComponentParticleSystem::GetParticlesSize() const
{
    return size;
}
