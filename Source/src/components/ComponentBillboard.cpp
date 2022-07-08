#include "core/hepch.h"

#include "ComponentBillboard.h"

#include <imgui_color_gradient.h>

#include "modules/ModuleProgram.h"
#include "modules/ModuleResources.h"
#include "modules/ModuleRender.h"
#include "modules/ModuleCamera.h"
#include "modules/ModuleSceneManager.h"
#include "resources/ResourceTexture.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"

Hachiko::ComponentBillboard::ComponentBillboard(GameObject* container) :
    Component(Component::Type::BILLBOARD, container)
{
    gradient = new ImGradient();
}

Hachiko::ComponentBillboard::~ComponentBillboard()
{
    DetachFromScene();
    delete gradient;
}

void Hachiko::ComponentBillboard::Draw(ComponentCamera* camera, Program* program)
{
    if (state == ParticleSystem::Emitter::State::STOPPED || !game_object->IsActive())
    {
        return;
    }

    glActiveTexture(GL_TEXTURE0);
    int has_texture = 0;
    if (texture_properties.GetTexture() != nullptr)
    {
        const int gl_texture = texture_properties.GetTexture()->GetImageId();
        ModuleTexture::Bind(gl_texture, static_cast<int>(Hachiko::ModuleProgram::TextureSlots::DIFFUSE));
        has_texture = 1;
    }

    glDepthMask(GL_FALSE);
    float4 color = float4::one;
    if (render_properties.render_mode == ParticleSystem::ParticleRenderMode::PARTICLE_ADDITIVE)
    {
        ModuleRender::EnableBlending(GL_ONE, GL_ONE, GL_FUNC_ADD);
    }
    else
    {
        ModuleRender::EnableBlending(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_FUNC_ADD);
        color.w = render_properties.alpha;
    }

    float4x4 model_matrix;
    GetOrientationMatrix(camera, model_matrix);
    program->BindUniformFloat4x4("model", model_matrix.ptr());
    program->BindUniformFloat4x4("view", &camera->GetViewMatrix()[0][0]);
    program->BindUniformFloat4x4("proj", &camera->GetProjectionMatrix()[0][0]);

    program->BindUniformFloat("x_factor", &texture_properties.GetFactor().x);
    program->BindUniformFloat("y_factor", &texture_properties.GetFactor().y);
    program->BindUniformFloat("current_frame", &current_frame);
    program->BindUniformFloat2("animation_index", animation_index.ptr());

    if (color_section)
    {
        gradient->getColorAt(color_frame, color.ptr());
    }

    program->BindUniformFloat4("input_color", color.ptr());
    program->BindUniformInts("has_texture", 1, &has_texture);

    int flip_x(texture_properties.GetFlip().x);
    int flip_y(texture_properties.GetFlip().y);
    program->BindUniformInts("flip_x", 1, &flip_x);
    program->BindUniformInts("flip_y", 1, &flip_y);
    program->BindUniformFloat("animation_blend", &blend_factor);

    glBindVertexArray(App->renderer->GetParticleVao());
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    // Clear
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDepthMask(GL_TRUE);
    ModuleRender::DisableBlending();
}

void Hachiko::ComponentBillboard::DrawGui()
{
    ImGui::PushID(this);
    if (ImGuiUtils::CollapsingHeader(game_object, this, "Billboard"))
    {
        const char* particle_render_modes[] = {"Additive", "Transparent"};
        const char* billboards[] = {"Normal", "Vertical", "Horizontal", "Stretch", "World"};
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
            Widgets::Checkbox("Play On Awake", &play_on_awake);
            Widgets::MultiTypeSelector("Start delay", start_delay);

            Widgets::DragFloatConfig params_cfg;
            params_cfg.speed = 0.05f;
            params_cfg.min = 0.0f;
            MultiTypeSelector("Start size", start_size, &params_cfg);
            Widgets::MultiTypeSelector("Start rotation", start_rotation);

            Widgets::DragFloat3("Position", emitter_properties.position);
        }

        if (CollapsingHeader("Renderer", &renderer_section, Widgets::CollapsibleHeaderType::Checkbox))
        {
            ImGui::BeginDisabled(!renderer_section);
            int orientation = static_cast<int>(render_properties.orientation);
            if (Widgets::Combo("Particle Orientations", &orientation, billboards, IM_ARRAYSIZE(billboards)))
            {
                render_properties.orientation = static_cast<ParticleSystem::ParticleOrientation>(orientation);
                App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
            }

            int render_mode = static_cast<int>(render_properties.render_mode);
            if (Widgets::Combo("Particle Render Mode", &render_mode, particle_render_modes, IM_ARRAYSIZE(particle_render_modes)))
            {
                render_properties.render_mode = static_cast<ParticleSystem::ParticleRenderMode>(render_mode);
                App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
            }

            Widgets::Checkbox("Orientate to direction", &render_properties.orientate_to_direction);

            Widgets::DragFloatConfig alpha_config;
            alpha_config.format = "%.2f";
            alpha_config.speed = 0.01f;
            alpha_config.min = 0.00f;
            alpha_config.max = 1.00f;
            alpha_config.enabled = (render_properties.render_mode == ParticleSystem::ParticleRenderMode::PARTICLE_TRANSPARENT);
            DragFloat("Alpha channel", render_properties.alpha, &alpha_config);
            ImGui::EndDisabled();
        }

        if (CollapsingHeader("Texture", &texture_section, Widgets::CollapsibleHeaderType::Checkbox))
        {
            ImGui::BeginDisabled(!texture_section);
            if (texture_properties.GetTexture() != nullptr)
            {
                ImGui::Image(reinterpret_cast<void*>(texture_properties.GetTexture()->GetImageId()), ImVec2(80, 80));
                ImGui::SameLine();
                ImGui::BeginGroup();
                ImGui::Text("%dx%d", texture_properties.GetTexture()->width, texture_properties.GetTexture()->height);
                ImGui::TextWrapped("Path: %s", texture_properties.GetTexture()->path.c_str());
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

                float2 tiles_editor = texture_properties.GetTiles();
                if (DragFloat2("Tiles", tiles_editor, &cfg))
                {
                    texture_properties.SetTiles(tiles_editor);
                }

                Widgets::DragFloatConfig tile_config;
                tile_config.format = "%.f";
                tile_config.speed = 1.0f;
                tile_config.min = 0.0f;
                DragFloat("Total tiles", total_tiles, &tile_config);

                bool2 flip_texture_editor = texture_properties.GetFlip();
                Widgets::Checkbox("Flip X", &flip_texture_editor.x);
                Widgets::Checkbox("Flip Y", &flip_texture_editor.y);
                texture_properties.SetFlip(flip_texture_editor);
            }
            else
            {
                AddTexture();
            }

            ImGui::EndDisabled();
        }

        if (CollapsingHeader("Animation", &animation_section, Widgets::CollapsibleHeaderType::Checkbox))
        {
            ImGui::BeginDisabled(!animation_section);
            Widgets::DragFloatConfig cfg;
            cfg.speed = 0.01f;
            Widgets::DragFloat("Animation speed", animation_speed, &cfg);
            cfg.enabled = true;
            cfg.speed = 0.05f;
            cfg.min = 0.0f;
            cfg.max = 1.0f;
            Widgets::DragFloat("Blend factor", blend_factor, &cfg);
            ImGui::EndDisabled();
        }

        if (CollapsingHeader("Color", &color_section, Widgets::CollapsibleHeaderType::Checkbox))
        {
            ImGui::BeginDisabled(!color_section);
            ImGui::PushItemWidth(200);
            ImGui::GradientEditor(gradient, dragging_gradient, selected_gradient);
            ImGui::PushItemWidth(150);
            ImGui::NewLine();
            ImGui::DragInt("Cycles over lifetime##color_cycles", &color_cycles, 1, 1, inf);
            ImGui::EndDisabled();
        }

        if (CollapsingHeader("Size", &size_section, Widgets::CollapsibleHeaderType::Checkbox))
        {
            ImGui::BeginDisabled(!size_section);
            Widgets::DragFloatConfig cfg;
            cfg.speed = 0.01f;
            Widgets::MultiTypeSelector("Size over time", size_over_time, &cfg);
            ImGui::EndDisabled();
        }

        if (CollapsingHeader("Rotation", &rotation_section, Widgets::CollapsibleHeaderType::Checkbox))
        {
            ImGui::BeginDisabled(!rotation_section);
            Widgets::DragFloatConfig cfg;
            cfg.speed = 0.01f;
            Widgets::MultiTypeSelector("Rotation over time", rotation_over_time, &cfg);
            ImGui::EndDisabled();
        }

        ImGui::PopStyleVar();
        ImGui::Unindent();
    }
    ImGui::PopID();

#ifndef PLAY_BUILD
    DisplayControls();
#endif
}

void Hachiko::ComponentBillboard::DisplayControls()
{
    const auto& pos = App->editor->GetSceneWindow()->GetViewportPosition();
    const auto& viewport_size = App->editor->GetSceneWindow()->GetViewportSize();

    ImGui::SetNextWindowPos(ImVec2(pos.x + ImGui::GetStyle().FramePadding.x * 3, pos.y));
    ImGui::SetNextWindowBgAlpha(1.0f);

    const std::string go_label = StringUtils::Concat("GameObject: ", GetGameObject()->GetName().c_str());
    const float min_size_x = std::min(ImGui::CalcTextSize(go_label.c_str()).x + ImGui::GetFontSize() * 2, viewport_size.x / 4.0f);
    ImGui::SetNextWindowSize(ImVec2(std::max(min_size_x, 200.0f), 0));
    ImGui::Begin("Billboard", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse);

    ImGui::TextWrapped(go_label.c_str());
    ImGui::Text("Elapsed time: %.1f s", elapsed_time);

    ImGui::Separator();

    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));
    ImGui::BeginTable("##", 3, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_NoSavedSettings);
    ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthStretch, 0.33f);
    ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthStretch, 0.34f);
    ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthStretch, 0.33f);
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);

    if (state == ParticleSystem::Emitter::State::PLAYING)
    {
        if (ImGui::Button("Pause", ImVec2(-1, 0)))
        {
            Pause();
        }
    }
    else
    {
        if (ImGui::Button("Play", ImVec2(-1, 0)))
        {
            Play();
        }
    }
    ImGui::TableNextColumn();
    if (ImGui::Button("Restart", ImVec2(-1, 0)))
    {
        Restart();
    }
    ImGui::TableNextColumn();
    if (ImGui::Button("Stop", ImVec2(-1, 0)))
    {
        Stop();
    }
    ImGui::EndTable();
    ImGui::PopStyleVar();
    ImGui::End();
}

void Hachiko::ComponentBillboard::Start()
{
    if (!in_scene)
    {
        PublishIntoScene();
    }

    if (play_on_awake)
    {
        state = ParticleSystem::Emitter::State::PLAYING;
    }

    Restart();
}

void Hachiko::ComponentBillboard::Update()
{
#ifndef PLAY_BUILD
    if (!in_scene)
    {
        Start();
    }
#endif //PLAY_BUILD

    if (state == ParticleSystem::Emitter::State::STOPPED)
    {
        Reset();
        return;
    }
    else if (state == ParticleSystem::Emitter::State::PLAYING)
    {
        elapsed_time += EngineTimer::delta_time;

        // Delay
        if (elapsed_time < start_delay.GetValue())
        {
            return;
        }

        if (!loop && elapsed_time >= duration)
        {
            state = ParticleSystem::Emitter::State::STOPPED;
        }
        else if (loop && elapsed_time >= duration)
        {
            Reset();
        }

        UpdateAnimationData();
        UpdateColorOverLifetime();
        UpdateSizeOverLifetime();
        UpdateRotationOverLifetime();
    }
}

inline void Hachiko::ComponentBillboard::Play()
{
    state = ParticleSystem::Emitter::State::PLAYING;
}

void Hachiko::ComponentBillboard::Pause()
{
    state = ParticleSystem::Emitter::State::PAUSED;
}

void Hachiko::ComponentBillboard::Restart()
{
    color_time = 0.0f;
    animation_time = 0.0f;
    Reset();
    Play();
}

inline void Hachiko::ComponentBillboard::Stop()
{
    color_frame = 0.0f;
    animation_index = float2(0.0f, 0.0f);
    state = ParticleSystem::Emitter::State::STOPPED;
}

inline void Hachiko::ComponentBillboard::Reset()
{
    elapsed_time = 0.0f;
    current_frame = 0.0f;
    animation_index = {0.0f, 0.0f};
    size = start_size.GetValue();
}

void Hachiko::ComponentBillboard::Save(YAML::Node& node) const
{
    node.SetTag("billboard");
    node[BILLBOARD_PLAY_ON_AWAKE] = play_on_awake;
    if (texture_properties.GetTexture() != nullptr)
    {
        node[BILLBOARD_TEXTURE_ID] = texture_properties.GetTexture()->GetID();
    }
    node[TILES] = texture_properties.GetTiles();
    node[FLIP] = texture_properties.GetFlip();
    node[BILLBOARD_LIFETIME] = duration;
    node[BILLBOARD_PROPERTIES] = render_properties;
    node[ANIMATION_SPEED] = animation_speed;
    node[TOTAL_TILES] = total_tiles;
    node[ANIMATION_LOOP] = loop;
    node[COLOR_CYCLES] = color_cycles;
    node[COLOR_GRADIENT] = *gradient;
    node[ANIMATION_SECTION] = animation_section;
    node[COLOR_SECTION] = color_section;
    node[SIZE_SECTION] = size_section;
    node[SIZE_OVERTIME] = size_over_time;
    node[ROTATION_SECTION] = rotation_section;
    node[ROTATION_OVERTIME] = rotation_over_time;
    node[START_DELAY] = start_delay;
    node[START_SIZE] = start_size;
    node[START_ROTATION] = start_rotation;
    node[BLEND_FACTOR] = blend_factor;
}

void Hachiko::ComponentBillboard::Load(const YAML::Node& node)
{
    UID texture_id = node[BILLBOARD_TEXTURE_ID].IsDefined() ? node[BILLBOARD_TEXTURE_ID].as<UID>() : 0;
    if (texture_id)
    {
        texture_properties.SetTexture(static_cast<ResourceTexture*>(
            App->resources->GetResource(Resource::Type::TEXTURE, texture_id)));
    }

    render_properties = node[BILLBOARD_PROPERTIES].IsDefined()
                     ? node[BILLBOARD_PROPERTIES].as<ParticleSystem::ParticleProperties>()
                     : render_properties;

    play_on_awake = node[BILLBOARD_PLAY_ON_AWAKE].IsDefined() ? node[BILLBOARD_PLAY_ON_AWAKE].as<bool>() : false;

    texture_properties.SetTiles(float2(1.0f, 1.0f));
    if (node[TILES].IsDefined() && !node[TILES].IsNull())
    {
        texture_properties.SetTiles(node[TILES].as<float2>());
    }

    duration = node[BILLBOARD_LIFETIME].IsDefined() ? node[BILLBOARD_LIFETIME].as<float>() : 0.0f;

    animation_speed = node[ANIMATION_SPEED].IsDefined() ? node[ANIMATION_SPEED].as<float>() : 0;

    total_tiles = node[TOTAL_TILES].IsDefined() ? node[TOTAL_TILES].as<float>() : 0;

    loop = node[ANIMATION_LOOP].IsDefined() ? node[ANIMATION_LOOP].as<bool>() : false;

    texture_properties.SetFlip(node[FLIP].IsDefined() ? node[FLIP].as<bool2>() : bool2::False);

    color_cycles = node[COLOR_CYCLES].IsDefined() ? node[COLOR_CYCLES].as<int>() : 0;

    if (node[COLOR_GRADIENT].IsDefined())
    {
        gradient->clearMarks();
        for (int i = 0; i < node[COLOR_GRADIENT].size(); ++i)
        {
            auto mark = node[COLOR_GRADIENT][i].as<ImGradientMark>();
            gradient->addMark(mark.position,
                              ImColor(mark.color[0], mark.color[1], mark.color[2], mark.color[3]));
        }
    }

    animation_section = node[ANIMATION_SECTION].IsDefined() ? node[ANIMATION_SECTION].as<bool>() : animation_section;

    color_section = node[COLOR_SECTION].IsDefined() ? node[COLOR_SECTION].as<bool>() : color_section;

    size_section = node[SIZE_SECTION].IsDefined() ? node[SIZE_SECTION].as<bool>() : size_section;
    size_over_time = node[SIZE_OVERTIME].IsDefined() ? node[SIZE_OVERTIME].as<ParticleSystem::VariableTypeProperty>() : size_over_time;

    rotation_section = node[ROTATION_SECTION].IsDefined() ? node[ROTATION_SECTION].as<bool>() : rotation_section;
    rotation_over_time = node[ROTATION_OVERTIME].IsDefined() ? node[ROTATION_OVERTIME].as<ParticleSystem::VariableTypeProperty>() : rotation_over_time;

    start_delay = node[START_DELAY].IsDefined() ? node[START_DELAY].as<ParticleSystem::VariableTypeProperty>() : start_delay;

    start_size = node[START_SIZE].IsDefined() ? node[START_SIZE].as<ParticleSystem::VariableTypeProperty>() : start_size;

    start_rotation = node[START_ROTATION].IsDefined() ? node[START_ROTATION].as<ParticleSystem::VariableTypeProperty>() : start_rotation;

    blend_factor = node[BLEND_FACTOR].IsDefined() ? node[BLEND_FACTOR].as<float>() : blend_factor;
}

void Hachiko::ComponentBillboard::AddTexture()
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
            ImGuiFileDialogFlags_DontShowHiddenFiles | ImGuiFileDialogFlags_DisableCreateDirectoryButton
            | ImGuiFileDialogFlags_HideColumnType | ImGuiFileDialogFlags_HideColumnDate);
    }

    if (ImGuiFileDialog::Instance()->Display(title))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            texture_path = ImGuiFileDialog::Instance()->GetFilePathName();
        }

        ImGuiFileDialog::Instance()->Close();
    }

    texture_properties.SetTexture(texture_path);
}

void Hachiko::ComponentBillboard::RemoveTexture()
{
    texture_properties.SetTexture(nullptr);
}

inline void Hachiko::ComponentBillboard::UpdateAnimationData()
{
    if (!HasTexture() || !animation_section)
    {
        return;
    }

    animation_time += EngineTimer::delta_time;

    if (animation_time <= animation_speed)
    {
        return;
    }

    animation_time = 0.0f;

    if (animation_index.x < texture_properties.GetTiles().x - 1)
    {
        animation_index.x += 1.0f;
        return;
    }
    else if (animation_index.y < texture_properties.GetTiles().y - 1)
    {
        animation_index.x = 0.0f;
        animation_index.y += 1.0f;
        return;
    }

    animation_index = {0.0f, 0.0f};
}

inline void Hachiko::ComponentBillboard::UpdateColorOverLifetime()
{
    if (!color_section)
    {
        return;
    }

    color_time += EngineTimer::delta_time;
    float time_mod = fmod(color_time, duration / color_cycles);
    color_frame = time_mod / duration * color_cycles;

    if (color_time > duration)
    {
        color_time = 0.0f;
        color_frame = 0.0f;
    }
}

void Hachiko::ComponentBillboard::UpdateRotationOverLifetime()
{
    if (!rotation_section)
    {
        return;
    }

    rotation += rotation_over_time.GetValue() * EngineTimer::delta_time;
}

void Hachiko::ComponentBillboard::UpdateSizeOverLifetime()
{
    if (!size_section)
    {
        return;
    }

    size += size_over_time.GetValue() * EngineTimer::delta_time;
}

void Hachiko::ComponentBillboard::PublishIntoScene()
{
    auto scene = game_object->scene_owner;

    if (scene)
    {
        scene->AddParticleComponent(this);
        in_scene = true;
    }
}

void Hachiko::ComponentBillboard::DetachFromScene()
{
    auto scene = game_object->scene_owner;

    if (scene)
    {
        scene->RemoveParticleComponent(GetID());
        in_scene = false;
    }
}

void Hachiko::ComponentBillboard::GetOrientationMatrix(ComponentCamera* camera, float4x4& model_matrix)
{
    ComponentTransform* transform = GetGameObject()->GetComponent<ComponentTransform>();
    float3 position = transform->GetGlobalPosition() + emitter_properties.position;
    float3 scale = transform->GetGlobalScale() * size;
    float3 camera_position = camera->GetFrustum().Pos();
    float3x3 rotation_matrix = float3x3::identity.RotateZ(rotation);

    switch (render_properties.orientation)
    {
        case ParticleSystem::ParticleOrientation::NORMAL:
        {
            const Frustum& frustum = camera->GetFrustum();
            if (render_properties.orientate_to_direction)
            {
                float3 forward = transform->GetGlobalRotation().WorldZ();
                float3 right = Cross(-frustum.Front(), forward).Normalized();
                forward = Cross(right, -frustum.Front()).Normalized();

                float3x3 new_rotation;
                new_rotation.SetCol(1, right);
                new_rotation.SetCol(2, -frustum.Front());
                new_rotation.SetCol(0, forward);

                model_matrix = float4x4::FromTRS(position, new_rotation, scale);
            }
            else
            {
                float3x3 rotate_part = transform->GetGlobalMatrix().RotatePart();
                float4x4 global_model_matrix = transform->GetGlobalMatrix();
                model_matrix = global_model_matrix.LookAt(rotate_part.Col(2), -frustum.Front(), rotate_part.Col(1), float3::unitY);
                model_matrix = float4x4::FromTRS(position, model_matrix.RotatePart() * rotate_part * rotation_matrix, scale);
            }
            break;
        }
        case ParticleSystem::ParticleOrientation::HORIZONTAL:
        {
            if (render_properties.orientate_to_direction)
            {
                float3 direction = transform->GetGlobalRotation().WorldZ();
                float3 projection = position + direction - direction.y * float3::unitY;
                direction = (projection - position).Normalized();
                float3 right = Cross(float3::unitY, direction);

                float3x3 new_rotation;
                new_rotation.SetCol(1, right);
                new_rotation.SetCol(2, float3::unitY);
                new_rotation.SetCol(0, direction);

                model_matrix = float4x4::FromTRS(position, new_rotation, scale);
            }
            else
            {
                model_matrix = float4x4::LookAt(float3::unitZ, float3::unitY, float3::unitY, float3::unitY);
                model_matrix = float4x4::FromTRS(position, model_matrix.RotatePart() * rotation_matrix, scale);
            }
            break;
        }
        case ParticleSystem::ParticleOrientation::VERTICAL:
        {
            float3 camera_direction = (float3(camera_position.x, position.y, camera_position.z) - position).Normalized();
            model_matrix = float4x4::LookAt(float3::unitZ, camera_direction, float3::unitY, float3::unitY);
            model_matrix = float4x4::FromTRS(position, model_matrix.RotatePart() * rotation_matrix, scale);
            break;
        }
        case ParticleSystem::ParticleOrientation::WORLD:
        {
            model_matrix = game_object->GetTransform()->GetGlobalMatrix();
            break;
        }
    }
    // TODO: To add
    //case BillboardType::STRETCH:
    //{
    //    float3 camera_direction = (camera_position - position).Normalized();
    //    float3 up_dir = Cross(direction, camera_direction);
    //    float3 new_camera_dir = Cross(direction, up_dir);

    //    float3x3 new_rotation;
    //    new_rotation.SetCol(0, up_dir);
    //    new_rotation.SetCol(1, direction);
    //    new_rotation.SetCol(2, new_camera_dir);

    //    model_matrix = float4x4::FromTRS(position, new_rotation * model_stretch, scale);
    //}
}

bool Hachiko::ComponentBillboard::HasTexture()
{
    return texture_properties.GetTexture() != nullptr;
}

// Texture Properties:

Hachiko::ComponentBillboard::TextureProperties::TextureProperties() {}

void Hachiko::ComponentBillboard::TextureProperties::SetFlip(const bool2& new_flip_texture)
{
    flip_texture = new_flip_texture;
}

void Hachiko::ComponentBillboard::TextureProperties::SetTiles(const float2 new_tiles)
{
    tiles = new_tiles;

    factor.x = (tiles.x != 0.0f) ? (1.0f / tiles.x) : 1.0f;
    factor.y = (tiles.y != 0.0f) ? (1.0f / tiles.y) : 1.0f;
}

void Hachiko::ComponentBillboard::TextureProperties::SetTexture(const std::string& path)
{
    std::string texture_path = path;
    texture_path.append(META_EXTENSION);

    if (!std::filesystem::exists(texture_path.c_str()))
    {
        return;
    }

    YAML::Node texture_node = YAML::LoadFile(texture_path);
    ResourceTexture* res = static_cast<ResourceTexture*>(App->resources->GetResource(Resource::Type::TEXTURE, texture_node[RESOURCES][0][RESOURCE_ID].as<UID>()));

    if (res != nullptr)
    {
        texture = res;
    }
}

void Hachiko::ComponentBillboard::TextureProperties::SetTexture(ResourceTexture* resource)
{
    texture = resource;
}

const Hachiko::bool2& Hachiko::ComponentBillboard::TextureProperties::GetFlip() const
{
    return flip_texture;
}

const float2& Hachiko::ComponentBillboard::TextureProperties::GetTiles() const
{
    return tiles;
}

const float2& Hachiko::ComponentBillboard::TextureProperties::GetFactor() const
{
    return factor;
}

const Hachiko::ResourceTexture* Hachiko::ComponentBillboard::TextureProperties::GetTexture() const
{
    return texture;
}
