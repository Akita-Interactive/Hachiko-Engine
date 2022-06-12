#include "core/hepch.h"
#include "components/ComponentParticleSystem.h"

#include "ComponentCamera.h"
#include "components/ComponentTransform.h"

#include "modules/ModuleSceneManager.h"
#include "modules/ModuleResources.h"

#include "debugdraw.h"
#include "modules/ModuleCamera.h"

Hachiko::ComponentParticleSystem::ComponentParticleSystem(GameObject* container) :
    Component(Type::PARTICLE_SYSTEM, container)
{
    particle_modifiers.push_back(std::make_shared<VelocityParticleModifier>("Velocity over lifetime"));
    particle_modifiers.push_back(std::make_shared<SizeParticleModifier>("Size over lifetime"));
    particle_modifiers.push_back(std::make_shared<ColorParticleModifier>("Color over lifetime"));
    particle_modifiers.push_back(std::make_shared<ForceParticleModifier>("Force over lifetime"));
    particle_modifiers.push_back(std::make_shared<AnimationParticleModifier>("Animation over lifetime"));
}

Hachiko::ComponentParticleSystem::~ComponentParticleSystem()
{
    App->event->Unsubscribe(Event::Type::CURVE_EDITOR, GetID());
    App->event->Unsubscribe(Event::Type::SELECTION_CHANGED, GetID());
    particle_modifiers.clear();
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

    std::function selection_changed = [&](Event& evt) {
        const auto data = evt.GetEventData<SelectionChangedEventPayload>();
        if (data.GetSelected() != GetGameObject())
        {
            Stop();
        }
    };
    App->event->Subscribe(Event::Type::SELECTION_CHANGED, selection_changed, GetID());
}

void Hachiko::ComponentParticleSystem::Update()
{
    if (!in_scene)
    {
        App->scene_manager->GetActiveScene()->AddParticleComponent(this);
        in_scene = true;
        Start(); // TODO: For DEBUG as Start is not called
    }

    UpdateEmitterTimes();
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
        const char* particle_render_modes[] = {"Additive", "Transparent"};
        const char* particle_orientations[] = {"Normal", "Vertical", "Horizontal", "Stretch"};
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
        ImGui::Indent();
        if (CollapsingHeader("Parameters", &parameters_section, Widgets::CollapsibleHeaderType::Icon, ICON_FA_BURST))
        {
            Widgets::DragFloatConfig duration_cfg;
            duration_cfg.min = 0.05f;
            duration_cfg.speed = 0.05f;
            duration_cfg.enabled = !loop;

            DragFloat("Duration", duration, &duration_cfg);
            Widgets::Checkbox("Loop", &loop);
            Widgets::MultiTypeSelector("Start delay", start_delay);
            Widgets::MultiTypeSelector("Start lifetime", start_life);
            Widgets::MultiTypeSelector("Start speed", start_speed);
            Widgets::MultiTypeSelector("Start size", start_size);
            Widgets::MultiTypeSelector("Start rotation", start_rotation);
        }

        if (CollapsingHeader("Emission", &emission_section, Widgets::CollapsibleHeaderType::Checkbox))
        {
            ImGui::BeginDisabled(!emission_section);
            Widgets::DragFloatConfig rate_cfg;
            rate_cfg.min = 0.0f;

            Widgets::MultiTypeSelector("Rate over time", rate_over_time, &rate_cfg);
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

        for (const auto& particle_module : particle_modifiers)
        {
            if (CollapsingHeader(particle_module->GetName().c_str(), &particle_module->active, Widgets::CollapsibleHeaderType::Checkbox))
            {
                ImGui::BeginDisabled(!particle_module->IsActive());
                particle_module->DrawGui();
                ImGui::EndDisabled();
            }
        }

        if (CollapsingHeader("Texture", &texture_section, Widgets::CollapsibleHeaderType::Checkbox))
        {
            ImGui::BeginDisabled(!texture_section);
            if (texture != nullptr)
            {
                ImGui::Image(reinterpret_cast<void*>(texture->GetImageId()), ImVec2(80, 80));
                ImGui::SameLine();
                ImGui::BeginGroup();
                ImGui::Text("%dx%d", texture->width, texture->height);
                ImGui::TextWrapped("Path: %s", texture->path.c_str());
                ImGui::Spacing();
                if (ImGui::Button("Remove texture##remove_texture", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
                {
                    RemoveTexture();
                }
                ImGui::EndGroup();

                Widgets::DragFloat2Config cfg;
                cfg.format = "%.f";
                cfg.speed = float2::one;
                cfg.min = float2::zero;

                if (DragFloat2("Tiles", tiles, &cfg))
                {
                    if (tiles.x != 0.0f)
                    {
                        factor.x = 1 / tiles.x;
                    }
                    if (tiles.y != 0.0f)
                    {
                        factor.y = 1 / tiles.y;
                    }
                }

                Widgets::DragFloatConfig tile_config;
                tile_config.format = "%.f";
                tile_config.speed = 1.0f;
                tile_config.min = 0.0f;
                DragFloat("Total tiles", total_tiles, &tile_config);

                Widgets::Checkbox("Flip X", &flip_texture.x);
                Widgets::Checkbox("Flip Y", &flip_texture.y);
            }
            else
            {
                AddTexture();
            }

            ImGui::EndDisabled();
        }

        // if (CollapsingHeader("Lights", &lights_section, Widgets::CollapsibleHeaderType::Checkbox))
        // {
        //     ImGui::BeginDisabled(!renderer_section);
        //
        //     ImGui::EndDisabled();
        // }

        if (CollapsingHeader("Renderer", &renderer_section, Widgets::CollapsibleHeaderType::Checkbox))
        {
            ImGui::BeginDisabled(!renderer_section);
            int orientation = static_cast<int>(particle_properties.orientation);
            if (Widgets::Combo("Particle Orientations", &orientation, particle_orientations, IM_ARRAYSIZE(particle_orientations)))
            {
                particle_properties.orientation = static_cast<ParticleSystem::ParticleOrientation>(orientation);
                App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
            }

            if (particle_properties.orientation == ParticleSystem::ParticleOrientation::HORIZONTAL)
            {
                ImGui::Checkbox("Orientate to direction", &particle_properties.orientate_to_direction);
            }

            int render_mode = static_cast<int>(particle_properties.render_mode);
            if (Widgets::Combo("Particle Render Mode", &render_mode, particle_render_modes, IM_ARRAYSIZE(particle_render_modes)))
            {
                particle_properties.render_mode = static_cast<ParticleSystem::ParticleRenderMode>(render_mode);
                App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
            }

            Widgets::DragFloatConfig alpha_config;
            alpha_config.format = "%.2f";
            alpha_config.speed = 0.01f;
            alpha_config.min = 0.00f;
            alpha_config.max = 1.00f;
            alpha_config.enabled = (particle_properties.render_mode == ParticleSystem::ParticleRenderMode::PARTICLE_TRANSPARENT);
            Widgets::DragFloat("Alpha channel", particle_properties.alpha, &alpha_config);
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
                             current_curve_editing_property->curve);
            }
        }
        ImGui::PopStyleVar();
        ImGui::Unindent();
    }
#ifndef PLAY_BUILD
    DisplayControls();
#endif
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
    config[PARTICLES_LIFE] = start_life;
    config[PARTICLES_SPEED] = start_speed;
    config[PARTICLES_SIZE] = start_size;
    config[PARTICLES_ROTATION] = start_rotation;
    config[PARTICLES_DELAY] = start_delay;
    config[PARTICLES_PROPERTIES] = particle_properties;
    node[PARTICLE_PARAMETERS] = config;

    // emission
    node[PARTICLE_EMISSION][RATE] = rate_over_time;

    // emitter
    YAML::Node emitter;
    emitter[EMITTER_TYPE] = static_cast<int>(emitter_type);
    emitter[EMITTER_PROPERTIES] = emitter_properties;
    node[EMITTER] = emitter;

    // texture
    if (texture != nullptr)
    {
        node[PARTICLES_TEXTURE][PARTICLES_TEXTURE_ID] = texture->GetID();
    }
    node[PARTICLES_TEXTURE][TILES] = tiles;
    node[PARTICLES_TEXTURE][FLIP] = flip_texture;

    YAML::Node modules;
    for (const auto& particle_module : particle_modifiers)
    {
        particle_module->Save(modules);
    }
    node[PARTICLE_MODIFIERS] = modules;
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
    start_life = node[PARTICLE_PARAMETERS][PARTICLES_LIFE].as<ParticleSystem::VariableTypeProperty>();
    start_speed = node[PARTICLE_PARAMETERS][PARTICLES_SPEED].as<ParticleSystem::VariableTypeProperty>();
    start_size = node[PARTICLE_PARAMETERS][PARTICLES_SIZE].as<ParticleSystem::VariableTypeProperty>();
    start_rotation = node[PARTICLE_PARAMETERS][PARTICLES_ROTATION].as<ParticleSystem::VariableTypeProperty>();
    start_delay = node[PARTICLE_PARAMETERS][PARTICLES_DELAY].as<ParticleSystem::VariableTypeProperty>();
    particle_properties = node[PARTICLE_PARAMETERS][PARTICLES_PROPERTIES].as<ParticleSystem::ParticleProperties>();

    // emission
    rate_over_time = node[PARTICLE_EMISSION][RATE].as<ParticleSystem::VariableTypeProperty>();

    // emitter
    emitter_type = static_cast<ParticleSystem::Emitter::Type>(node[EMITTER][EMITTER_TYPE].as<int>());
    emitter_properties = node[EMITTER][EMITTER_PROPERTIES].as<ParticleSystem::Emitter::Properties>();

    // texture
    flip_texture = node[PARTICLES_TEXTURE][FLIP].IsDefined() ? node[PARTICLES_TEXTURE][FLIP].as<bool2>() : bool2::False;
    if (node[PARTICLES_TEXTURE][TILES].IsDefined() && !node[PARTICLES_TEXTURE][TILES].IsNull())
    {
        tiles = node[PARTICLES_TEXTURE][TILES].as<float2>();
        factor.x = 1.0f / tiles.x;
        factor.y = 1.0f / tiles.y;
    }

    const UID texture_id = node[PARTICLES_TEXTURE][PARTICLES_TEXTURE_ID].IsDefined() ? node[PARTICLES_TEXTURE][PARTICLES_TEXTURE_ID].as<UID>() : 0;
    if (texture_id)
    {
        texture = static_cast<ResourceTexture*>(App->resources->GetResource(Resource::Type::TEXTURE, texture_id));
    }

    for (const auto& particle_module : particle_modifiers)
    {
        particle_module->Load(node[PARTICLE_MODIFIERS]);
    }
}

const Hachiko::ParticleSystem::VariableTypeProperty& Hachiko::ComponentParticleSystem::GetParticlesLife() const
{
    return start_life;
}

const Hachiko::ParticleSystem::VariableTypeProperty& Hachiko::ComponentParticleSystem::GetParticlesSpeed() const
{
    return start_speed;
}

const Hachiko::ParticleSystem::VariableTypeProperty& Hachiko::ComponentParticleSystem::GetParticlesSize() const
{
    return start_size;
}

const Hachiko::ParticleSystem::Emitter::Properties& Hachiko::ComponentParticleSystem::GetEmitterProperties() const
{
    return emitter_properties;
}

const Hachiko::ParticleSystem::ParticleProperties& Hachiko::ComponentParticleSystem::GetParticlesProperties() const
{
    return particle_properties;
}

const Hachiko::ResourceTexture* Hachiko::ComponentParticleSystem::GetTexture() const
{
    return texture;
}

int Hachiko::ComponentParticleSystem::GetTextureTotalTiles() const
{
    return static_cast<int>(total_tiles);
}

const Hachiko::bool2& Hachiko::ComponentParticleSystem::GetFlipTexture() const
{
    return flip_texture;
}

const float2& Hachiko::ComponentParticleSystem::GetTextureTiles() const
{
    return tiles;
}

const float2& Hachiko::ComponentParticleSystem::GetFactor() const
{
    return factor;
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
    for (const auto& particle_module : particle_modifiers)
    {
        particle_module->Update(particles);
    }
}

void Hachiko::ComponentParticleSystem::UpdateEmitterTimes()
{
    //TODO check state

    time += EngineTimer::delta_time;

    if (!loop)
    {
        if (emitter_elapsed_time < duration)
        {
            emitter_elapsed_time += EngineTimer::delta_time;
        }
        else
        {
            able_to_emit = false;
            return;
        }
    }

    if (time * 1000.0f <= ONE_SEC_IN_MS / rate_over_time.GetValue()) // TODO: Avoid division
    {
        able_to_emit = false;
    }
    else
    {
        time = 0.0f;
        able_to_emit = true;
    }
}

void Hachiko::ComponentParticleSystem::ActivateParticles()
{
    if (!able_to_emit)
    {
        return;
    }

    for (auto& particle : particles)
    {
        if (!particle.IsActive())
        {
            particle.Reset(); // For particles to set current emitter configuration
            particle.Activate();
            return;
        }
    }
}

float3 Hachiko::ComponentParticleSystem::CalculateDirectionFromShape() const
{
    float3 particle_direction = float3::one;
    switch (emitter_type)
    {
    case ParticleSystem::Emitter::Type::CONE:
    {
        const float effective_radius = emitter_properties.radius * (1 - emitter_properties.radius_thickness);
        particle_direction.x = emitter_properties.rotation.x + effective_radius * RandomUtil::RandomSigned();
        particle_direction.z = emitter_properties.rotation.z + effective_radius * RandomUtil::RandomSigned();
        break;
    }
    case ParticleSystem::Emitter::Type::SPHERE:
    {
        // TODO: This is not working perfectly. Emission depends on the size of the radius and it shouldn't
        const float effective_radius = emitter_properties.radius * (1 - emitter_properties.radius_thickness);
        particle_direction.x = emitter_properties.rotation.x + effective_radius * RandomUtil::RandomSigned();
        particle_direction.y = emitter_properties.rotation.y * (RandomUtil::RandomSigned() > 0.0f ? 1.0f : -1.0f);
        particle_direction.z = emitter_properties.rotation.z + effective_radius * RandomUtil::RandomSigned();
        break;
    }
    case ParticleSystem::Emitter::Type::BOX:
    case ParticleSystem::Emitter::Type::CIRCLE:
    case ParticleSystem::Emitter::Type::RECTANGLE:
        break;
    }

    return particle_direction;
}

void Hachiko::ComponentParticleSystem::AddTexture()
{
    const char* title = "Select billboard texture";
    std::string texture_path;
    ResourceTexture* res = nullptr;

    if (ImGui::Button("Add texture", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
    {
        ImGuiFileDialog::Instance()->OpenDialog(
            title,
            "Select Texture",
            ".png,.tif,.jpg,.tga",
            "./assets/textures/",
            1,
            nullptr,
            ImGuiFileDialogFlags_DontShowHiddenFiles | ImGuiFileDialogFlags_DisableCreateDirectoryButton |
            ImGuiFileDialogFlags_HideColumnType | ImGuiFileDialogFlags_HideColumnDate);
    }

    if (ImGuiFileDialog::Instance()->Display(title))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            texture_path = ImGuiFileDialog::Instance()->GetFilePathName();
        }

        ImGuiFileDialog::Instance()->Close();
    }

    texture_path.append(META_EXTENSION);
    if (!std::filesystem::exists(texture_path.c_str()))
    {
        return;
    }

    YAML::Node texture_node = YAML::LoadFile(texture_path);
    res = static_cast<ResourceTexture*>(App->resources->GetResource(Resource::Type::TEXTURE, texture_node[RESOURCES][0][RESOURCE_ID].as<UID>()));

    if (res != nullptr)
    {
        texture = res;
    }
}

void Hachiko::ComponentParticleSystem::RemoveTexture()
{
    texture = nullptr;
}

bool Hachiko::ComponentParticleSystem::IsLoop() const
{
    return loop;
}

void Hachiko::ComponentParticleSystem::Play()
{
    emitter_state = ParticleSystem::Emitter::State::PLAYING;
}

void Hachiko::ComponentParticleSystem::Pause()
{
    emitter_state = ParticleSystem::Emitter::State::PAUSED;
}

void Hachiko::ComponentParticleSystem::Restart()
{
    emitter_elapsed_time = 0.0f;
    emitter_state = ParticleSystem::Emitter::State::PLAYING;
}

void Hachiko::ComponentParticleSystem::Stop()
{
    emitter_elapsed_time = 0.0f;
    emitter_state = ParticleSystem::Emitter::State::STOPPED;
}

void Hachiko::ComponentParticleSystem::DisplayControls()
{
    const auto& pos = App->editor->GetSceneWindow()->GetViewportPosition();
    const auto& viewport_size = App->editor->GetSceneWindow()->GetViewportSize();

    ImGui::SetNextWindowPos(ImVec2(viewport_size.x + pos.x + ImGui::GetStyle().FramePadding.x, pos.y), 0, ImVec2(1.0f, 0.0f));
    ImGui::SetNextWindowBgAlpha(1.0f);
    ImGui::Begin("Particles", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse);
    ImGui::Text("GameObject");
    ImGui::SameLine();
    ImGui::Text(GetGameObject()->GetName().c_str());

    char particlesSpawned[10];
    sprintf_s(particlesSpawned, 10, "%.1f", rate_over_time.values.x);
    ImGui::Text("Particles");
    ImGui::SameLine();
    ImGui::Text(particlesSpawned);

    ImGui::Separator();

    if (emitter_state == ParticleSystem::Emitter::State::PAUSED || emitter_state == ParticleSystem::Emitter::State::STOPPED)
    {
        if (ImGui::Button("Play"))
        {
            Play();
        }
    }
    if (emitter_state == ParticleSystem::Emitter::State::PLAYING)
    {
        if (ImGui::Button("Pause"))
        {
            Pause();
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Restart"))
    {
        Restart();
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop"))
    {
        Stop();
    }
    ImGui::End();
}
