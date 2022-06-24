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

Hachiko::ComponentBillboard::ComponentBillboard(GameObject* container) 
	: Component(Component::Type::BILLBOARD, container) 
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
    if (state == ParticleSystem::Emitter::State::STOPPED)
    {
        return;
    }

    glActiveTexture(GL_TEXTURE0);
    int has_texture = 0;
    if (texture != nullptr)
    {
        const int gl_texture = texture->GetImageId();
        ModuleTexture::Bind(gl_texture, static_cast<int>(Hachiko::ModuleProgram::TextureSlots::DIFFUSE));
        has_texture = 1;
    }

    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    float4 color = float4::one;
    if (properties.render_mode == ParticleSystem::ParticleRenderMode::PARTICLE_ADDITIVE)
    {
        glBlendFunc(GL_ONE, GL_ONE);
    }
    else
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        color.w = properties.alpha;
    }

    float4x4 model_matrix;
    GetOrientationMatrix(camera, model_matrix);
    program->BindUniformFloat4x4("model", model_matrix.ptr());
    program->BindUniformFloat4x4("view", &camera->GetViewMatrix()[0][0]);
    program->BindUniformFloat4x4("proj", &camera->GetProjectionMatrix()[0][0]);

    program->BindUniformFloat("x_factor", &factor.x);
    program->BindUniformFloat("y_factor", &factor.y);
    program->BindUniformFloat("current_frame", &current_frame);
    program->BindUniformFloat2("animation_index", animation_index.ptr());

    if (color_section)
    {
        gradient->getColorAt(color_frame, color.ptr());
    }

    program->BindUniformFloat4("input_color", color.ptr());
    program->BindUniformInts("has_texture", 1, &has_texture);

    int flip_x(flip_texture.x);
    int flip_y(flip_texture.y);
    program->BindUniformInts("flip_x", 1, &flip_x);
    program->BindUniformInts("flip_y", 1, &flip_y);
    program->BindUniformFloat("animation_blend", &blend_factor);
    
    glBindVertexArray(App->renderer->GetParticleVao());
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    
    // Clear
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
}

void Hachiko::ComponentBillboard::DrawGui()
{
    ImGui::PushID(this);
    if (ImGuiUtils::CollapsingHeader(game_object, this, "Billboard"))
    {
        const char* particle_render_modes[] = {"Additive", "Transparent"};
        const char* billboards[] = {"Normal", "Vertical", "Horizontal", "Stretch"};
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
        }

        if (CollapsingHeader("Renderer", &renderer_section, Widgets::CollapsibleHeaderType::Checkbox))
        {
            ImGui::BeginDisabled(!renderer_section);
            int orientation = static_cast<int>(properties.orientation);
            if (Widgets::Combo("Particle Orientations", &orientation, billboards, IM_ARRAYSIZE(billboards)))
            {
                properties.orientation = static_cast<ParticleSystem::ParticleOrientation>(orientation);
                App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
            }

            int render_mode = static_cast<int>(properties.render_mode);
            if (Widgets::Combo("Particle Render Mode", &render_mode, particle_render_modes, IM_ARRAYSIZE(particle_render_modes)))
            {
                properties.render_mode = static_cast<ParticleSystem::ParticleRenderMode>(render_mode);
                App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
            }

            Widgets::DragFloatConfig alpha_config;
            alpha_config.format = "%.2f";
            alpha_config.speed = 0.01f;
            alpha_config.min = 0.00f;
            alpha_config.max = 1.00f;
            alpha_config.enabled = (properties.render_mode == ParticleSystem::ParticleRenderMode::PARTICLE_TRANSPARENT);
            DragFloat("Alpha channel", properties.alpha, &alpha_config);
            ImGui::EndDisabled();
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
        
        if (CollapsingHeader("Animation", &animation_section, Widgets::CollapsibleHeaderType::Checkbox))
        {
            ImGui::BeginDisabled(!animation_section);
            Widgets::DragFloatConfig cfg;
            cfg.speed = 0.01f;
            Widgets::DragFloat("Animation speed", animation_speed, &cfg);
            cfg.enabled = true;
            cfg.speed = 0.05f;
            cfg.min  = 0.0f;
            cfg.max  = 1.0f;
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
            Widgets::MultiTypeSelector("Size over time" , size_over_time, &cfg);
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

    ImGui::SetNextWindowPos(ImVec2(viewport_size.x + pos.x + ImGui::GetStyle().FramePadding.x, pos.y), 0, ImVec2(1.0f, 0.0f));
    ImGui::SetNextWindowBgAlpha(1.0f);
    ImGui::Begin("Particles", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse);
    ImGui::Text("GameObject");
    ImGui::SameLine();
    ImGui::Text(GetGameObject()->GetName().c_str());

    char elapsed_time_display[10];
    sprintf_s(elapsed_time_display, 10, "%.1f", elapsed_time);
    ImGui::Text("Elapsed time");
    ImGui::SameLine();
    ImGui::Text(elapsed_time_display);

    ImGui::Separator();

    if (state == ParticleSystem::Emitter::State::PAUSED || state == ParticleSystem::Emitter::State::STOPPED)
    {
        if (ImGui::Button("Play"))
        {
            Play();
        }
    }
    if (state == ParticleSystem::Emitter::State::PLAYING)
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

void Hachiko::ComponentBillboard::Start()
{
    PublishIntoScene();

    if (play_on_awake)
    {
        state = ParticleSystem::Emitter::State::PLAYING;
    }
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
        time += EngineTimer::delta_time;
        
        // Delay
        if (elapsed_time < start_delay.GetValue())
        {
            return;
        }

        if (!loop && time >= duration)
        {
            state = ParticleSystem::Emitter::State::STOPPED;
        }
        else if (loop && time >= duration)
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
    time = 0.0f;
    elapsed_time = 0.0f;
    size = start_size.GetValue();
}

void Hachiko::ComponentBillboard::Save(YAML::Node& node) const
{
    node.SetTag("billboard");
    node[BILLBOARD_PLAY_ON_AWAKE] = play_on_awake;
    if (texture != nullptr)
    {
        node[BILLBOARD_TEXTURE_ID] = texture->GetID();
    }
    node[TILES] = tiles;
    node[FLIP] = flip_texture;
    node[BILLBOARD_LIFETIME] = duration;
    node[BILLBOARD_PROPERTIES] = properties;
    node[ANIMATION_SPEED] = animation_speed;
    node[TOTAL_TILES] = total_tiles;
    node[ANIMATION_LOOP] = loop;
    node[COLOR_CYCLES] = color_cycles;
    node[COLOR_GRADIENT] = *gradient;
    node[ANIMATION_SECTION] = animation_section;
    node[COLOR_SECTION] = color_section;
    node[START_DELAY] = start_delay;
    node[START_SIZE] = start_size;
    node[START_ROTATION] = start_rotation;
    node[BLEND_FACTOR] = blend_factor;

}

void Hachiko::ComponentBillboard::Load(const YAML::Node& node) 
{
    UID texture_id = node[BILLBOARD_TEXTURE_ID].IsDefined() ? 
        node[BILLBOARD_TEXTURE_ID].as<UID>() : 0;
    if (texture_id)
    {
        texture = static_cast<ResourceTexture*>(
            App->resources->GetResource(Resource::Type::TEXTURE, texture_id));
    }

    properties = node[BILLBOARD_PROPERTIES].IsDefined() 
        ? node[BILLBOARD_PROPERTIES].as<ParticleSystem::ParticleProperties>()
        : properties;
    
    play_on_awake = node[BILLBOARD_PLAY_ON_AWAKE].IsDefined() ? 
        node[BILLBOARD_PLAY_ON_AWAKE].as<bool>() : false;

    tiles = float2(1.0f, 1.0f);
    if (node[TILES].IsDefined() && !node[TILES].IsNull())
    {
        tiles = node[TILES].as<float2>();
        factor.x = 1 / tiles.x;
        factor.y = 1 / tiles.y;
    }

    duration = node[BILLBOARD_LIFETIME].IsDefined() ?
        node[BILLBOARD_LIFETIME].as<float>() : 0.0f;

    animation_speed = node[ANIMATION_SPEED].IsDefined() ?
        node[ANIMATION_SPEED].as<float>() : 0;

    total_tiles = node[TOTAL_TILES].IsDefined() ?
        node[TOTAL_TILES].as<float>() : 0;

    loop = node[ANIMATION_LOOP].IsDefined() ?
        node[ANIMATION_LOOP].as<bool>() : false;

    flip_texture = node[FLIP].IsDefined() ?
        node[FLIP].as<bool2>() : flip_texture;

    color_cycles = node[COLOR_CYCLES].IsDefined() ? 
        node[COLOR_CYCLES].as<int>() : 0;
    
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

    animation_section = node[ANIMATION_SECTION].IsDefined() ?
        node[ANIMATION_SECTION].as<bool>() : animation_section;

    color_section = node[COLOR_SECTION].IsDefined() ?
        node[COLOR_SECTION].as<bool>() : color_section;

    start_delay = node[START_DELAY].IsDefined() ?
        node[START_DELAY].as<ParticleSystem::VariableTypeProperty>() : start_delay;

    start_size = node[START_SIZE].IsDefined() ?
        node[START_SIZE].as<ParticleSystem::VariableTypeProperty>() : start_size;

    start_rotation = node[START_ROTATION].IsDefined() ?
        node[START_ROTATION].as<ParticleSystem::VariableTypeProperty>() : start_rotation;

    blend_factor = node[BLEND_FACTOR].IsDefined() ?
        node[BLEND_FACTOR].as<float>() : blend_factor;
}

void Hachiko::ComponentBillboard::AddTexture()
{
    const char* title = "Select billboard texture";
    std::string texture_path;
    ResourceTexture* res = nullptr;

    if (ImGui::Button("Add texture", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
    {
        ImGuiFileDialog::Instance()->OpenDialog(
            title,"Select Texture", ".png,.tif,.jpg,.tga", "./assets/textures/", 1, nullptr,
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

void Hachiko::ComponentBillboard::RemoveTexture()
{
    texture = nullptr;
}

inline void Hachiko::ComponentBillboard::UpdateAnimationData() 
{
    if (!HasTexture() || !animation_section)
    {
        return;
    }
    
    if (time <= animation_speed)
    {
        return;
    }

    if (animation_index.x < tiles.x - 1)
    {
        animation_index.x += 1.0f;
        return;
    }
    else if (animation_index.y < tiles.y - 1)
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

    float time_mod = fmod(time, duration / color_cycles);
    color_frame = time_mod / duration * color_cycles;

    if (time > duration)
    {
        time = 0.0f;
        color_frame = 0.0f;
    }
}

void Hachiko::ComponentBillboard::UpdateRotationOverLifetime()
{
    if (!rotation_section)
    {
        return;
    }

    rotation += rotation_over_time.GetValue();
}

void Hachiko::ComponentBillboard::UpdateSizeOverLifetime()
{
    if (!size_section)
    {
        return;
    }

    size += size_over_time.GetValue();
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
    float3 position = transform->GetGlobalPosition();
    float3 scale = transform->GetGlobalScale() * size;
    float3 camera_position = camera->GetFrustum()->Pos();
    float3x3 rotation_matrix = float3x3::identity.RotateZ(rotation);

    switch (properties.orientation)
    {
        case ParticleSystem::ParticleOrientation::NORMAL:
        {
            Frustum* frustum = camera->GetFrustum();
            float3x3 rotate_part = transform->GetGlobalMatrix().RotatePart();
            float4x4 global_model_matrix = transform->GetGlobalMatrix();
            model_matrix = global_model_matrix.LookAt(rotate_part.Col(2), -frustum->Front(), rotate_part.Col(1), float3::unitY);
            model_matrix = float4x4::FromTRS(position, model_matrix.RotatePart() * rotate_part * rotation_matrix, scale);
            break;
        }
        case ParticleSystem::ParticleOrientation::HORIZONTAL:
        {
                if (properties.orientate_to_direction)
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
    return texture != nullptr;
}