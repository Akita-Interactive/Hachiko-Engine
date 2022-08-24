#include "core/hepch.h"

#include "Particle.h"

#include "modules/ModuleRender.h"
#include "modules/ModuleProgram.h"

#include "components/ComponentCamera.h"
#include "components/ComponentTransform.h"
#include "components/ComponentParticleSystem.h"

#include "resources/ResourceTexture.h"

void Hachiko::Particle::Update()
{
    current_life -= EngineTimer::delta_time;

    if (current_life <= 0)
    {
        Reset();
    }
}

void Hachiko::Particle::Reset()
{
    Deactivate();
    SetCurrentLife(GetInitialLife());
    SetCurrentSize(GetInitialSize());
    SetCurrentSpeed(GetInitialSpeed());
    SetCurrentRotation(GetInitialRotation());
    SetCurrentPosition(emitter->GetLocalPositionFromShape());

    const float4x4 game_object_model = emitter->GetGameObject()->GetComponent<ComponentTransform>()->GetGlobalMatrix();
    
    // Calculate emitter matrix
    const float3 emitter_rotation = DegToRad(emitter->GetEmitterProperties().rotation);
    const float4x4 emitter_model = float4x4::FromTRS(emitter->GetEmitterProperties().position,
            Quat::FromEulerXYZ(emitter_rotation.x, emitter_rotation.y, emitter_rotation.z).Normalized(),
            emitter->GetEmitterProperties().scale);

    // Calculate particle matrix
    const float3 shape_direction = GetInitialDirection();
    const float3 rotation = DegToRad(shape_direction);
    const float4x4 particle_model = float4x4::FromTRS(current_position,
        Quat::FromEulerXYZ(rotation.x, rotation.y, rotation.z).Normalized(), emitter->GetEmitterProperties().scale);
    
    // Final model matrix
    const float4x4 current_model = game_object_model * emitter_model * particle_model;
    
    // Update rotated position
    SetCurrentPosition(current_model.TranslatePart());

    const float3 direction = (current_model.RotatePart() * shape_direction).Normalized();
    SetCurrentDirection(direction);

    current_animation_frame = 0;
    animation_index = {0.0f, 0.0f};
}

void Hachiko::Particle::Draw(ComponentCamera* camera, const Program* program)
{
    glActiveTexture(GL_TEXTURE0);
    int has_texture = 0;
    const auto texture_resource = emitter->GetTexture();
    if (texture_resource != nullptr)
    {
        ModuleTexture::Bind(texture_resource->GetImageId(), static_cast<int>(ModuleProgram::TextureSlots::DIFFUSE));
        has_texture = 1;
    }

    glDepthMask(GL_FALSE);
    if (emitter->GetParticlesProperties().render_mode == ParticleSystem::ParticleRenderMode::PARTICLE_ADDITIVE)
    {
        ModuleRender::EnableBlending(GL_ONE, GL_ONE, GL_FUNC_ADD);
    }
    else
    {
        ModuleRender::EnableBlending(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_FUNC_ADD);
        current_color.w = emitter->GetParticlesProperties().alpha;
    }

    float4x4 model_matrix = float4x4::identity;
    GetModelMatrix(camera, model_matrix);
    program->BindUniformFloat4x4("model", model_matrix.ptr());
    program->BindUniformFloat4x4("view", &camera->GetViewMatrix()[0][0]);
    program->BindUniformFloat4x4("proj", &camera->GetProjectionMatrix()[0][0]);

    const float x_factor = emitter->GetFactor().x;
    const float y_factor = emitter->GetFactor().y;
    program->BindUniformFloat("x_factor", &x_factor);
    program->BindUniformFloat("y_factor", &y_factor);
    program->BindUniformFloat2("animation_index", animation_index.ptr());

    program->BindUniformFloat4("input_color", current_color.ptr());
    program->BindUniformInts("has_texture", 1, &has_texture);

    const int flip_x = emitter->GetFlipTexture().x ? 1 : 0;
    const int flip_y = emitter->GetFlipTexture().y ? 1 : 0;
    program->BindUniformInts("flip_x", 1, &flip_x);
    program->BindUniformInts("flip_y", 1, &flip_y);
    program->BindUniformFloat("animation_blend", &animation_blend);

    glBindVertexArray(App->renderer->GetFullScreenQuadVAO());
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    // Clear
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDepthMask(GL_TRUE);
    ModuleRender::DisableBlending();
}

void Hachiko::Particle::GetModelMatrix(ComponentCamera* camera, float4x4& out_matrix) const
{
    const auto transform = emitter->GetGameObject()->GetTransform();
    float3 particle_size(current_size, current_size, 0.0f);
    const float3x3 particle_rotation_matrix = float3x3::RotateZ(current_rotation);

    switch (emitter->GetParticlesProperties().orientation)
    {
        case ParticleSystem::ParticleOrientation::NORMAL:
        {
            const Frustum& frustum = camera->GetFrustum();
            float3x3 rotate_part = transform->GetGlobalMatrix().RotatePart();
            float4x4 global_model_matrix = transform->GetGlobalMatrix();
            out_matrix = global_model_matrix.LookAt(rotate_part.Col(2), -frustum.Front(), rotate_part.Col(1), float3::unitY);
            out_matrix = float4x4::FromTRS(current_position, out_matrix.RotatePart() * rotate_part * particle_rotation_matrix, particle_size);
            break;
        }
        case ParticleSystem::ParticleOrientation::HORIZONTAL:
        {
            if (emitter->GetParticlesProperties().orientate_to_direction)
            {
                float3 global_position = transform->GetGlobalPosition();
                float3 direction = current_direction;
                float3 projection = global_position + direction - direction.y * float3::unitY;
                direction = (projection - global_position).Normalized();
                float3 right = Cross(float3::unitY, direction);

                float3x3 new_rotation;
                new_rotation.SetCol(1, right);
                new_rotation.SetCol(2, float3::unitY);
                new_rotation.SetCol(0, direction);

                out_matrix = float4x4::FromTRS(current_position, new_rotation, particle_size);
            }
            else
            {
                out_matrix = float4x4::LookAt(float3::unitZ, float3::unitY, float3::unitY, float3::unitY);
                out_matrix = float4x4::FromTRS(current_position, out_matrix.RotatePart() * particle_rotation_matrix, particle_size);
            }
            break;
        }
        case ParticleSystem::ParticleOrientation::VERTICAL:
        {
            const float3 camera_position = camera->GetFrustum().Pos();
            const float3 camera_direction = (float3(camera_position.x, current_position.y, camera_position.z) - current_position).Normalized();
            out_matrix = float4x4::LookAt(float3::unitZ, camera_direction, float3::unitY, float3::unitY);
            out_matrix = float4x4::FromTRS(current_position, out_matrix.RotatePart() * particle_rotation_matrix, particle_size);
            break;
        }
        case ParticleSystem::ParticleOrientation::STRETCH:
        {
            particle_size.x = particle_size.x * emitter->GetParticlesStretch().values.x;
            particle_size.y = particle_size.y * emitter->GetParticlesStretch().values.y;

            float4x4 particle_model = float4x4::FromTRS(current_position, float3x3::identity, float3::one);
            float3 global_direction = (particle_model.RotatePart() * current_direction).Normalized();

            float3 camera_position = camera->GetFrustum().Pos();
            float3 camera_direction = (float3(camera_position.x, current_position.y, camera_position.z) - current_position).Normalized();
            float3 up_direction = Cross(global_direction, camera_direction);
            float3 new_camera_direction = Cross(global_direction, up_direction);

            float3x3 rotation;
            rotation.SetCol(0, up_direction);
            rotation.SetCol(1, global_direction);
            rotation.SetCol(2, new_camera_direction);

            out_matrix = float4x4::identity * rotation;
            out_matrix = float4x4::FromTRS(current_position, out_matrix.RotatePart(), particle_size);
            break;
        }
    }
}

bool Hachiko::Particle::IsActive() const
{
    return active;
}

void Hachiko::Particle::Activate()
{
    active = true;
}

void Hachiko::Particle::Deactivate()
{
    active = false;
}

float Hachiko::Particle::GetCurrentLifeNormalized() const
{
    return 1 - (current_life / total_life);
}

bool Hachiko::Particle::HasTexture() const
{
    return emitter->GetTexture() != nullptr;
}

float Hachiko::Particle::GetInitialLife()
{
    total_life = emitter->GetParticlesLife().GetValue();
    return total_life;
}

float Hachiko::Particle::GetInitialSpeed() const
{
    return emitter->GetParticlesSpeed().GetValue();
}

float Hachiko::Particle::GetInitialSize() const
{
    return emitter->GetParticlesSize().GetValue();
}

float Hachiko::Particle::GetInitialRotation() const
{
    return emitter->GetParticlesRotation().GetValue();
}

float3 Hachiko::Particle::GetInitialDirection() const
{
    float3 particle_direction = float3::zero;
    const auto& emitter_properties = emitter->GetEmitterProperties();
    switch (emitter->GetEmitterType())
    {
        case ParticleSystem::Emitter::Type::CONE:
        {
            particle_direction.x = current_position.x * (emitter_properties.top - emitter_properties.radius);
            particle_direction.z = current_position.z * (emitter_properties.top - emitter_properties.radius);
            particle_direction.y = 1.0f;

            break;
        }
        case ParticleSystem::Emitter::Type::SPHERE:
        {
            particle_direction = current_position;
            break;
        }
        case ParticleSystem::Emitter::Type::BOX:
        {
            break;
        }
        case ParticleSystem::Emitter::Type::CIRCLE:
        {
            particle_direction.y = 0.0f;
            particle_direction.x = current_position.x;
            particle_direction.z = current_position.z;
            break;
        }
        case ParticleSystem::Emitter::Type::RECTANGLE:
        {
            particle_direction = float3::unitY;
            break;
        }
    }

    return particle_direction;
}

const float2& Hachiko::Particle::GetTextureTiles() const
{
    return emitter->GetTextureTiles();
}

int Hachiko::Particle::GetTextureTotalTiles() const
{
    return emitter->GetTextureTotalTiles();
}

float Hachiko::Particle::GetLife() const
{
    return total_life;
}

float Hachiko::Particle::GetCurrentLife() const
{
    return current_life;
}

void Hachiko::Particle::SetCurrentLife(const float life)
{
    this->current_life = life;
}

float Hachiko::Particle::GetCurrentSpeed() const
{
    return current_speed;
}

void Hachiko::Particle::SetCurrentSpeed(const float speed)
{
    this->current_speed = speed;
}

float Hachiko::Particle::GetCurrentRotation() const
{
    return current_rotation;
}

void Hachiko::Particle::SetCurrentRotation(const float rotation)
{
    this->current_rotation = rotation;
}

const float4& Hachiko::Particle::GetCurrentColor() const
{
    return current_color;
}

void Hachiko::Particle::SetCurrentColor(const float4& color)
{
    this->current_color = color;
}

float Hachiko::Particle::GetCurrentSize() const
{
    return current_size;
}

void Hachiko::Particle::SetCurrentSize(const float size)
{
    this->current_size = size;
}

const float2& Hachiko::Particle::GetAnimationIndex() const
{
    return animation_index;
}

void Hachiko::Particle::SetAnimationIndex(const float2& animation_idx)
{
    this->animation_index = animation_idx;
}

const float3& Hachiko::Particle::GetCurrentPosition() const
{
    return current_position;
}

void Hachiko::Particle::SetCurrentPosition(const float3& position)
{
    this->current_position = position;
}

const float3& Hachiko::Particle::GetCurrentDirection() const
{
    return current_direction;
}

void Hachiko::Particle::SetCurrentDirection(const float3& direction)
{
    this->current_direction = direction;
}

unsigned Hachiko::Particle::GetCurrentAnimationFrame() const
{
    return current_animation_frame;
}

void Hachiko::Particle::SetCurrentAnimationFrame(unsigned frame)
{
    this->current_animation_frame = frame;
}

float Hachiko::Particle::GetAnimationBlend() const
{
    return animation_blend;
}

void Hachiko::Particle::SetAnimationBlend(float blend)
{
    this->animation_blend = blend;
}

void Hachiko::Particle::SetEmitter(ComponentParticleSystem* particle_emitter)
{
    this->emitter = particle_emitter;
}
