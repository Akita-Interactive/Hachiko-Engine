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
    PublishIntoScene();
}

Hachiko::ComponentBillboard::~ComponentBillboard() 
{
    DetachFromScene();
    delete gradient;
}

void Hachiko::ComponentBillboard::Draw(ComponentCamera* camera, Program* program)
{
    glActiveTexture(GL_TEXTURE0);
    int glTexture = 0;
    has_texture = 0;
    if (texture != nullptr)
    {
        glTexture = texture->GetImageId();
        Hachiko::ModuleTexture::Bind(glTexture, static_cast<int>(Hachiko::ModuleProgram::TextureSlots::DIFFUSE));
        has_texture = 1;
    }

    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    if (render_mode == BillboardRenderMode::B_ADDITIVE)
    {
        glBlendFunc(GL_ONE, GL_ONE);
    }
    else
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }


    float4x4 model_matrix;
    GetOrientationMatrix(camera, model_matrix);
    program->BindUniformFloat4x4("model", model_matrix.ptr());
    program->BindUniformFloat4x4("view", &camera->GetViewMatrix()[0][0]);
    program->BindUniformFloat4x4("proj", &camera->GetProjectionMatrix()[0][0]);

    program->BindUniformFloat("x_factor", &x_factor);
    program->BindUniformFloat("y_factor", &y_factor);
    program->BindUniformFloat("current_frame", &current_frame);
    program->BindUniformFloat2("animation_index", animation_index.ptr());
    float4 color = float4::one;
    if (has_color_gradient)
    {
        gradient->getColorAt(color_frame, color.ptr());
    }

    program->BindUniformFloat4("input_color", color.ptr());
    program->BindUniformInts("has_texture", 1, &has_texture);

    int flip_x = has_flip_x ? 1 : 0;
    int flip_y = has_flip_y ? 1 : 0;
    program->BindUniformInts("flip_x", 1, &flip_x);
    program->BindUniformInts("flip_y", 1, &flip_y);
    
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
        ImGui::Separator();
        ImGui::Indent();
        ImGui::PushItemWidth(150);

        if (ImGui::Button("Play"))
        {
            Play();
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop"))
        {
            Stop();
        }
        ImGui::DragFloat("Duration", &billboard_lifetime, 1.0f, 0, inf);
        ImGui::DragInt("Skip Frames", &skip_frames, 1.0f, 0, inf);
        ImGui::Checkbox("Play On Awake", &play_on_awake);

        const char* billboard_type_combo[] = {"LookAt", "Horitzontal", "Vertical"};
        const char* billboard_type_combo_current = billboard_type_combo[static_cast<int>(type)];
        ImGui::Text("Type:");
        if (ImGui::BeginCombo("##Type", billboard_type_combo_current))
        {
            for (int n = 0; n < IM_ARRAYSIZE(billboard_type_combo); ++n)
            {
                bool is_selected = (billboard_type_combo_current == billboard_type_combo[n]);
                if (ImGui::Selectable(billboard_type_combo[n], is_selected))
                {
                    type = (BillboardType)n;
                }
                if (is_selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        if (type == BillboardType::HORIZONTAL)
        {
            ImGui::Indent();
            ImGui::Checkbox("Orientate to direction", &is_horizontal);
            ImGui::Unindent();
        }

        const char* render_mode_combo[] = {"Additive", "Transparent"};
        const char* render_mode_combo_current = render_mode_combo[static_cast<int>(render_mode)];
        if (ImGui::BeginCombo("Render Mode##", render_mode_combo_current))
        {
            for (int n = 0; n < IM_ARRAYSIZE(render_mode_combo); ++n)
            {
                bool is_selected = (render_mode_combo_current == render_mode_combo[n]);
                if (ImGui::Selectable(render_mode_combo[n], is_selected))
                {
                    render_mode = (BillboardRenderMode)n;
                }
                if (is_selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        ImGui::NewLine();

        // Texture Sheet Animation
        if (ImGui::CollapsingHeader("Texture Sheet Animation"))
        {
            ImGui::Checkbox("Loop##animation_loop", &animation_loop);
            static const ImGuiTreeNodeFlags texture_flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;

            if (ImGui::TreeNodeEx((void*)&texture, texture_flags, "Texture"))
            {
                if (texture != nullptr)
                {
                    ImGui::Image(reinterpret_cast<void*>(texture->GetImageId()), ImVec2(80, 80));
                    ImGui::SameLine();
                    ImGui::BeginGroup();
                    ImGui::Text("%dx%d", texture->width, texture->height);
                    ImGui::Text("Path: %s", texture->path.c_str());
                    if (ImGui::Button("Remove Texture##remove_texture"))
                    {
                        RemoveTexture();
                    }

                    ImGui::EndGroup();
                }
                else
                {
                    AddTexture();
                }
                ImGui::TreePop();
            }
            if (ImGui::DragScalar("X tiles", ImGuiDataType_U32, &x_tiles))
            {
                if (x_tiles)
                {
                    x_factor = 1 / (float)x_tiles;
                    HE_LOG("x_factor: %f", x_factor);
                }
            }
            if (ImGui::DragScalar("Y tiles", ImGuiDataType_U32, &y_tiles))
            {
                if (y_tiles)
                {
                    y_factor = 1 / (float)y_tiles;
                    HE_LOG("y_factor: %f", y_factor);
                }
            }

            ImGui::Checkbox("FlipX##animation_loop", &has_flip_x);
            ImGui::SameLine();
            ImGui::Checkbox("FlipY##animation_loop", &has_flip_y);
        }

        // Color Over Lifetime
        if (ImGui::CollapsingHeader("Color over lifetime"))
        {
            ImGui::Checkbox("Activate Gradient##color_over_lifetime", &has_color_gradient);
            if (has_color_gradient)
            {
                ImGui::Checkbox("Loop##color_loop", &color_loop);
                ImGui::PushItemWidth(200);
                ImGui::GradientEditor(gradient, dragging_gradient, selected_gradient);
                ImGui::PushItemWidth(150);
                ImGui::NewLine();
                ImGui::DragInt("Cycles over lifetime##color_cycles", &color_cycles, 1, 1, inf);
            }
        }
    }
    ImGui::PopID();
}

void Hachiko::ComponentBillboard::Start()
{
    is_playing = play_on_awake;
}

void Hachiko::ComponentBillboard::Update()
{
    if (!in_scene)
    {
        PublishIntoScene();
    }

    time += EngineTimer::delta_time;
    if (frame_counter++ < skip_frames)
    {
        return;
    }

    if (!is_playing)
    {
        Reset();
        return;
    }

    if (time > billboard_lifetime)
    {
        play_animation = false;
        play_color_gradient = false;
    }

    if (animation_loop || play_animation)
    {
        UpdateAnimationData();
    }

    if (color_loop || play_color_gradient)
    {
        UpdateColorOverLifetime();
    }

    frame_counter = 0;
}

inline void Hachiko::ComponentBillboard::Play() 
{
    Reset();
    is_playing = true;
    play_animation = true;
    play_color_gradient = true;
}

inline void Hachiko::ComponentBillboard::Stop()
{
    is_playing = false;
    play_animation = false;
    play_color_gradient = false;
}

inline void Hachiko::ComponentBillboard::Reset()
{
    is_playing = false;
    time = 0.0f;
    color_frame = 1.0f;
    play_animation = false;
    play_color_gradient = false;
}

void Hachiko::ComponentBillboard::Save(YAML::Node& node) const
{
    node[BILLBOARD_TYPE] = static_cast<int>(type);
    node[BILLBOARD_PLAY_ON_AWAKE] = play_on_awake;
    if (texture != nullptr)
    {
        node[BILLBOARD_TEXTURE_ID] = texture->GetID();
    }
    node[X_TILES] = x_tiles;
    node[Y_TILES] = y_tiles;
    node[FLIP_X] = has_flip_x;
    node[FLIP_Y] = has_flip_y;
    node[BILLBOARD_LIFETIME] = billboard_lifetime;
    node[SKIP_FRAMES] = skip_frames;
    node[ANIMATION_LOOP] = animation_loop;
    node[HAS_COLOR_GRADIENT] = has_color_gradient;
    node[COLOR_CYCLES] = color_cycles;
    node[COLOR_LOOP] = color_loop;
    node[COLOR_GRADIENT] = *gradient;
    node[BILLBOARD_RENDER_MODE] = static_cast<int>(render_mode);
}

void Hachiko::ComponentBillboard::Load(const YAML::Node& node) 
{
    UID texture_id = node[BILLBOARD_TEXTURE_ID].IsDefined() ? 
        node[BILLBOARD_TEXTURE_ID].as<UID>() : 0;
    if (texture_id)
    {
        texture = static_cast<ResourceTexture*>(
            App->resources->GetResource(Resource::Type::TEXTURE, texture_id));
        has_texture = 1;
    }

    type = node[BILLBOARD_TYPE].IsDefined() 
        ? static_cast<BillboardType>(node[BILLBOARD_TYPE].as<int>()) 
        : BillboardType::HORIZONTAL;
    
    play_on_awake = node[BILLBOARD_PLAY_ON_AWAKE].IsDefined() ? 
        node[BILLBOARD_PLAY_ON_AWAKE].as<bool>() : false;

    x_tiles = 1;
    if (node[X_TILES].IsDefined() && !node[X_TILES].IsNull())
    {
        x_tiles = node[X_TILES].as<int>();
        x_factor = 1 / (float)x_tiles;
    }

    y_tiles = 1;
    if (node[Y_TILES].IsDefined() && !node[Y_TILES].IsNull())
    {
        y_tiles = node[Y_TILES].as<int>();
        y_factor = 1 / (float)y_tiles;
    }

    billboard_lifetime = node[BILLBOARD_LIFETIME].IsDefined() ?
        node[BILLBOARD_LIFETIME].as<float>() : 0.0f;

    skip_frames = node[SKIP_FRAMES].IsDefined() ?
        node[SKIP_FRAMES].as<int>() : 0;

    animation_loop = node[ANIMATION_LOOP].IsDefined() ?
        node[ANIMATION_LOOP].as<bool>() : false;

    has_flip_x = node[FLIP_X].IsDefined() ?
        node[FLIP_X].as<bool>() : false;

    has_flip_y = node[FLIP_Y].IsDefined() ?
        node[FLIP_Y].as<bool>() : false;

    has_color_gradient = node[HAS_COLOR_GRADIENT].IsDefined() ?
        node[HAS_COLOR_GRADIENT].as<bool>() : false;

    color_cycles = node[COLOR_CYCLES].IsDefined() ? 
        node[COLOR_CYCLES].as<int>() : 0;
    
    color_loop = node[COLOR_LOOP].IsDefined() ?
        node[COLOR_LOOP].as<bool>() : false;

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

    render_mode = node[BILLBOARD_RENDER_MODE].IsDefined() ? 
        static_cast<BillboardRenderMode>(node[BILLBOARD_RENDER_MODE].as<int>()) :
        render_mode;
}

void Hachiko::ComponentBillboard::AddTexture()
{
    constexpr char* title = "Select billboard texture ";
    std::string texture_path{};
    ResourceTexture* res = nullptr;

    if (ImGui::Button("Add Texture"))
    {
        ImGuiFileDialog::Instance()->OpenDialog(title,
            "Select Texture", ".png,.tif,.jpg,.tga", "./assets/textures/", 1, nullptr,
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
    res = static_cast<ResourceTexture*>(App->resources->GetResource
    (Resource::Type::TEXTURE, texture_node[RESOURCES][0][RESOURCE_ID].as<UID>()));

    if (res != nullptr)
    {
        texture = res;
        textureID = texture->GetID();
        has_texture = 1;
    }
}

void Hachiko::ComponentBillboard::RemoveTexture()
{
    texture = nullptr;
    textureID = 0;
}

inline void Hachiko::ComponentBillboard::UpdateAnimationData() 
{
    if (!has_texture)
    {
        return;
    }

    if (animation_index.x < x_tiles - 1)
    {
        animation_index.x += 1.0f;
        return;
    }
    else if (animation_index.y < y_tiles - 1)
    {
        animation_index.x = 0.0f;
        animation_index.y += 1.0f;
        return;
    }

    animation_index = {0.0f, 0.0f};
}

inline void Hachiko::ComponentBillboard::UpdateColorOverLifetime()
{
    float time_mod = fmod(time, billboard_lifetime / color_cycles);
    color_frame = time_mod / billboard_lifetime * color_cycles;

    if (color_loop && time > billboard_lifetime)
    {
        time = 0.0f;
        color_frame = 0.0f;
    }
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
    float3 scale = transform->GetGlobalScale();
    float3 camera_position = camera->GetFrustum()->Pos();

    switch (type)
    {
    case BillboardType::NORMAL:
        {
            Frustum* frustum = camera->GetFrustum();
            float3x3 rotate_part = transform->GetGlobalMatrix().RotatePart();
            float4x4 global_model_matrix = transform->GetGlobalMatrix();
            model_matrix = global_model_matrix.LookAt(rotate_part.Col(2), -frustum->Front(), rotate_part.Col(1), float3::unitY);
            model_matrix = float4x4::FromTRS(position, model_matrix.RotatePart() * rotate_part, scale);
            break;
        }
    case BillboardType::HORIZONTAL:
        {
            if (is_horizontal)
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
                model_matrix = float4x4::FromTRS(position, model_matrix.RotatePart(), scale);
            }
            break;
        }
    case BillboardType::VERTICAL:
        {
            float3 camera_direction = (float3(camera_position.x, position.y, camera_position.z) - position).Normalized();
            model_matrix = float4x4::LookAt(float3::unitZ, camera_direction, float3::unitY, float3::unitY);
            model_matrix = float4x4::FromTRS(position, model_matrix.RotatePart(), scale);
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
