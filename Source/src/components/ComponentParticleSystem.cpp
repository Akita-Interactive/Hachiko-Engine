#include "core/hepch.h"
#include "components/ComponentParticleSystem.h"
#include "components/ComponentTransform.h"

#include "modules/ModuleSceneManager.h"

#include "debugdraw.h"

#include "ui/widgets/Widgets.h"

Hachiko::ComponentParticleSystem::ComponentParticleSystem(GameObject* container) :
    Component(Type::PARTICLE_SYSTEM, container)
{
    particle_modules.push_back(std::make_shared<VelocityParticleModule>("Velocity over lifetime"));
    particle_modules.push_back(std::make_shared<SizeParticleModule>("Size over lifetime"));
    particle_modules.push_back(std::make_shared<ColorParticleModule>("Color over lifetime"));
    particle_modules.push_back(std::make_shared<ForceParticleModule>("Force over lifetime"));
    particle_modules.push_back(std::make_shared<TextureParticleModule>("Texture over lifetime"));
}


Hachiko::ComponentParticleSystem::~ComponentParticleSystem()
{
    App->event->Unsubscribe(Event::Type::CURVE_EDITOR, GetID());
    particle_modules.clear();
    App->scene_manager->GetActiveScene()->RemoveParticleComponent(GetID());
    current_curve_editing_property = nullptr;
}

void Hachiko::ComponentParticleSystem::Start()
{
    for (auto& particle : particles)
    {
        particle.SetEmitter(this);
    }

    std::function edit_curve = [&](Event& evt) {
        const auto data = evt.GetEventData<CurveEditorEventPayload>();
        if (current_curve_editing_property)
        {
            current_curve_editing_property->selected = false;
        }
        if (current_curve_editing_property == data.GetValue())
        {
            current_curve_editing_property = nullptr;
        }
        else
        {
            current_curve_editing_property = data.GetValue();
            current_curve_editing_title = data.GetTitle();
            data.GetValue()->selected = true;
        }
    };
    App->event->Subscribe(Event::Type::CURVE_EDITOR, edit_curve, GetID());
}

void Hachiko::ComponentParticleSystem::Update()
{
    if (!in_scene)
    {
        App->scene_manager->GetActiveScene()->AddParticleComponent(this);
        in_scene = true;
        Start(); // TODO: For DEBUG as Start is not called
    }

    ActivateParticles();
    UpdateActiveParticles();
    UpdateModules();
}

void Hachiko::ComponentParticleSystem::Draw(ComponentCamera* camera, Program* program)
{
    for (auto& particle : particles)
    {
        if (!particle.IsActive())
        {
            continue;
        }

        particle.Draw(camera, program);
    }
}

void Hachiko::ComponentParticleSystem::DrawGui()
{
    if (ImGuiUtils::CollapsingHeader(game_object, this, "Particle system"))
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
        ImGui::Indent();
        if (CollapsingHeader("Parameters", &parameters_section, Widgets::CollapsibleHeaderType::Icon, ICON_FA_BURST))
        {
            Widgets::DragFloat("Duration", duration);
            Widgets::Checkbox("Loop", &loop);
            Widgets::MultiTypeSelector("Start delay", delay);
            Widgets::MultiTypeSelector("Start lifetime", life);
            Widgets::MultiTypeSelector("Start speed", speed);
            Widgets::MultiTypeSelector("Start size", size);
            Widgets::MultiTypeSelector("Start rotation", rotation);
        }

        if (CollapsingHeader("Emission", &emission_section, Widgets::CollapsibleHeaderType::Checkbox))
        {
            ImGui::BeginDisabled(!emission_section);
            Widgets::MultiTypeSelector("Rate over time", rate_over_time);
            ImGui::EndDisabled();
        }

        if (CollapsingHeader("Shape", &shape_section, Widgets::CollapsibleHeaderType::Checkbox))
        {
            ImGui::BeginDisabled(!shape_section);
            const char* shapes[] = {"Cone", "Box", "Sphere", "Circle", "Rectangle"};
            const char* emit_from_options[] = {"Volume", "Shell", "Edge"};

            int emitter = static_cast<int>(emitter_type);
            int emit_from = static_cast<int>(emitter_properties.emit_from);

            if (Widgets::Combo("Shape", &emitter, shapes, IM_ARRAYSIZE(shapes)))
            {
                emitter_type = static_cast<ParticleSystem::Emitter::Type>(emitter);
                App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
            }

            Widgets::DragFloat3Config scale_config;
            scale_config.min = float3(0.001f);
            Widgets::DragFloatConfig top;
            Widgets::DragFloatConfig radius;
            radius.min = 0.001f;
            radius.speed = 0.05f;

            Widgets::DragFloatConfig thickness;
            thickness.min = 0.001f;
            thickness.max = 1.0f;
            thickness.speed = 0.05f;

            Widgets::DragFloatConfig arc;
            arc.min = 0.0f;
            arc.max = 360.0f;
            arc.speed = 1.0f;
            arc.format = "%.f";

            switch (emitter_type)
            {
            case ParticleSystem::Emitter::Type::CONE:
                top.speed = 0.01f;
                top.min = 0.001f;
                DragFloat("Top", emitter_properties.top, &top);
                DragFloat("Radius", emitter_properties.radius, &radius);
                DragFloat("Radius thickness", emitter_properties.radius_thickness, &thickness);
                DragFloat("Arc", emitter_properties.arc, &arc);

                break;
            case ParticleSystem::Emitter::Type::BOX:
                if (Widgets::Combo("Emit from", &emit_from, emit_from_options, IM_ARRAYSIZE(emit_from_options)))
                {
                    emitter_properties.emit_from = static_cast<ParticleSystem::Emitter::EmitFrom>(emit_from);
                }
                break;
            case ParticleSystem::Emitter::Type::SPHERE:
            case ParticleSystem::Emitter::Type::CIRCLE:
                DragFloat("Radius", emitter_properties.radius, &radius);
                DragFloat("Radius thickness", emitter_properties.radius_thickness, &thickness);
                DragFloat("Arc", emitter_properties.arc, &arc);
                break;
            case ParticleSystem::Emitter::Type::RECTANGLE:
                scale_config.enabled = bool3(true, false, true);
                break;
            }

            ImGuiUtils::DisplayTooltip("Selects the shape of this particle system");
            Widgets::DragFloat3("Position", emitter_properties.position);
            Widgets::DragFloat3("Rotation", emitter_properties.rotation);
            DragFloat3("Scale", emitter_properties.scale, &scale_config);
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

        if (current_curve_editing_property)
        {
            bool always_open = true;
            if (CollapsingHeader("Curve editor", &always_open, Widgets::CollapsibleHeaderType::Icon, ICON_FA_BEZIER_CURVE, ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::TextUnformatted("Current curve:");
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
                ImGui::TextWrapped(current_curve_editing_title.c_str());
                ImGui::Spacing();
                ImGui::PopStyleColor();

                ImGui::Curve("##",
                             ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetCurrentContext()->FontSize * 5.0f),
                             ParticleSystem::CURVE_TICKS - 1,
                             reinterpret_cast<ImVec2*>(current_curve_editing_property->curve));
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
    node.SetTag("particle_system");
    // sections
    YAML::Node sections;
    sections[PARAMETER_SECTION] = parameters_section;
    sections[EMISSION_SECTION] = emission_section;
    sections[SHAPE_SECTION] = shape_section;
    sections[LIGHTS_SECTION] = lights_section;
    sections[RENDERER_SECTION] = renderer_section;
    node[PARTICLE_SECTIONS] = sections;

    // particle config
    YAML::Node config;
    config[PARTICLES_DURATION] = duration;
    config[PARTICLES_LOOP] = loop;
    config[PARTICLES_LIFE] = life;
    config[PARTICLES_SPEED] = speed;
    config[PARTICLES_SIZE] = size;
    config[PARTICLES_ROTATION] = rotation;
    config[PARTICLE_DELAY] = delay;
    node[PARTICLE_PARAMETERS] = config;

    // emission
    node[PARTICLE_EMISSION][RATE] = rate_over_time;

    // emitter
    YAML::Node emitter;
    emitter[EMITTER_TYPE] = static_cast<int>(emitter_type);
    emitter[EMITTER_PROPERTIES] = emitter_properties;
    node[EMITTER] = emitter;

    YAML::Node modules;
    for (const auto& particle_module : particle_modules)
    {
        particle_module->Save(modules);
    }
    node[PARTICLE_MODULES] = modules;
}

void Hachiko::ComponentParticleSystem::Load(const YAML::Node& node)
{
    // sections
    parameters_section = node[PARTICLE_SECTIONS][PARAMETER_SECTION].as<bool>();
    emission_section = node[PARTICLE_SECTIONS][EMISSION_SECTION].as<bool>();
    shape_section = node[PARTICLE_SECTIONS][SHAPE_SECTION].as<bool>();
    lights_section = node[PARTICLE_SECTIONS][LIGHTS_SECTION].as<bool>();
    renderer_section = node[PARTICLE_SECTIONS][RENDERER_SECTION].as<bool>();

    // particle config
    duration = node[PARTICLE_PARAMETERS][PARTICLES_DURATION].as<float>();
    loop = node[PARTICLE_PARAMETERS][PARTICLES_LOOP].as<bool>();
    life = node[PARTICLE_PARAMETERS][PARTICLES_LIFE].as<ParticleSystem::VariableTypeProperty>();
    speed = node[PARTICLE_PARAMETERS][PARTICLES_SPEED].as<ParticleSystem::VariableTypeProperty>();
    size = node[PARTICLE_PARAMETERS][PARTICLES_SIZE].as<ParticleSystem::VariableTypeProperty>();
    rotation = node[PARTICLE_PARAMETERS][PARTICLES_ROTATION].as<ParticleSystem::VariableTypeProperty>();
    delay = node[PARTICLE_PARAMETERS][PARTICLE_DELAY].as<ParticleSystem::VariableTypeProperty>();
    // emission
    rate_over_time = node[PARTICLE_EMISSION][RATE].as<ParticleSystem::VariableTypeProperty>();

    // emitter

    emitter_type = static_cast<ParticleSystem::Emitter::Type>(node[EMITTER][EMITTER_TYPE].as<int>());
    emitter_properties = node[EMITTER][EMITTER_PROPERTIES].as<ParticleSystem::Emitter::Properties>();

    for (const auto& particle_module : particle_modules)
    {
        particle_module->Load(node[PARTICLE_MODULES]);
    }
}

Hachiko::ParticleSystem::VariableTypeProperty
Hachiko::ComponentParticleSystem::GetParticlesLife() const
{
    return life;
}

Hachiko::ParticleSystem::VariableTypeProperty Hachiko::ComponentParticleSystem::GetParticlesSize() const
{
    return size;
}

Hachiko::ParticleSystem::VariableTypeProperty Hachiko::ComponentParticleSystem::GetParticlesSpeed() const
{
    return speed;
}

Hachiko::ParticleSystem::VariableTypeProperty Hachiko::ComponentParticleSystem::GetParticlesColor() const
{
    return ParticleSystem::VariableTypeProperty();
}

float3 Hachiko::ComponentParticleSystem::GetParticlesDirection() const
{
    const float4x4 model = game_object->GetComponent<ComponentTransform>()->GetGlobalMatrix();
    const float3 shape_direction = GetParticlesDirectionFromShape();
    const float4x4 emitter = float4x4::FromTRS(emitter_properties.position,
                                               Quat::FromEulerXYZ(shape_direction.x, shape_direction.y, shape_direction.z),
                                               emitter_properties.scale);
    const float4x4 current_model = model * emitter;
    const float3 random_direction = (current_model.RotatePart() * float3::unitY).Normalized();
    return random_direction;
}

float3 Hachiko::ComponentParticleSystem::GetParticlesEmissionPosition() const
{
    return emitter_properties.position + game_object->GetComponent<ComponentTransform>()->GetGlobalPosition();
}

void Hachiko::ComponentParticleSystem::UpdateActiveParticles()
{
    for (auto& particle : particles)
    {
        if (!particle.IsActive())
        {
            continue;
        }

        particle.Update();
    }
}

void Hachiko::ComponentParticleSystem::UpdateModules()
{
    for (const auto& particle_module : particle_modules)
    {
        particle_module->Update(particles);
    }
}

void Hachiko::ComponentParticleSystem::ActivateParticles()
{
    time += EngineTimer::delta_time;
    if (time * 1000.0f <= ONE_SEC_IN_MS / (int)rate_over_time.values.x)
    {
        return;
    }

    time = 0.0f;
    for (auto& particle : particles)
    {
        if (!particle.IsActive())
        {
            particle.Activate();
            return;
        }
    }
}

float3 Hachiko::ComponentParticleSystem::GetParticlesDirectionFromShape() const
{
    float3 particle_direction = float3::one;
    switch (emitter_type)
    {
    case ParticleSystem::Emitter::Type::CONE:
    {
        float effective_radius = emitter_properties.radius * (1 - emitter_properties.radius_thickness);
        particle_direction.x = emitter_properties.rotation.x + effective_radius * Random::RandomSignedFloat();
        particle_direction.z = emitter_properties.rotation.z + effective_radius * Random::RandomSignedFloat();
        break;
    }
    case ParticleSystem::Emitter::Type::SPHERE:
    {
        // TODO: This is not working perfectly. Emission depends on the size of the radius and it shouldn't
        float effective_radius = emitter_properties.radius * (1 - emitter_properties.radius_thickness);
        particle_direction.x = emitter_properties.rotation.x + effective_radius * Random::RandomSignedFloat();
        particle_direction.y = emitter_properties.rotation.y * (Random::RandomSignedFloat() > 0.0 ? 1.0 : -1.0);
        particle_direction.z = emitter_properties.rotation.z + effective_radius * Random::RandomSignedFloat();
        break;
    }
    case ParticleSystem::Emitter::Type::BOX:
    case ParticleSystem::Emitter::Type::CIRCLE:
    case ParticleSystem::Emitter::Type::RECTANGLE:
        break;
    }

    return particle_direction;
}
