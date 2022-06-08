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

    const float3 position = emitter->GetEmitterProperties().position +
                            emitter->GetGameObject()->GetComponent<ComponentTransform>()->GetGlobalPosition();
    SetCurrentPosition(position);

    const float4x4 game_object_model = emitter->GetGameObject()->GetComponent<ComponentTransform>()->GetGlobalMatrix();
    const float3 shape_direction = emitter->CalculateDirectionFromShape();
    const float4x4 emitter_model = float4x4::FromTRS(emitter->GetEmitterProperties().position,
                                                     Quat::FromEulerXYZ(shape_direction.x, shape_direction.y, shape_direction.z),
                                                     emitter->GetEmitterProperties().scale);
    const float4x4 current_model = game_object_model * emitter_model;
    const float3 direction = (current_model.RotatePart() * float3::unitY).Normalized();
    SetCurrentDirection(direction);
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
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    if (emitter->GetParticlesRenderMode() == ParticleSystem::ParticleRenderMode::PARTICLE_ADDITIVE)
    {
        glBlendFunc(GL_ONE, GL_ONE);
    }
    else
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        current_color.w = emitter->GetParticlesAlpha();
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

    glBindVertexArray(App->renderer->GetParticleVao());
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    // Clear
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
}

void Hachiko::Particle::GetModelMatrix(ComponentCamera* camera, float4x4& out_matrix) const
{
    // Vertical (cilindrical) orientation
    const float3 camera_position = camera->GetFrustum()->Pos();
    const float3 camera_direction = (float3(camera_position.x, current_position.y, camera_position.z) - current_position).Normalized();
    out_matrix = float4x4::LookAt(float3::unitZ, camera_direction, float3::unitY, float3::unitY);
    out_matrix = float4x4::FromTRS(current_position, out_matrix.RotatePart(), float3(current_size, 0.0f));
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

bool Hachiko::Particle::HasTexture() const
{
    return emitter->GetTexture() != nullptr;
}

float Hachiko::Particle::GetInitialLife() const
{
    return emitter->GetParticlesLife().values.x;
}

float Hachiko::Particle::GetInitialSpeed() const
{
    return emitter->GetParticlesSpeed().values.x;
}

const float2& Hachiko::Particle::GetInitialSize() const
{
    return emitter->GetParticlesSize().values;
}

const float2& Hachiko::Particle::GetTextureTiles() const
{
    return emitter->GetTextureTiles();
}

int Hachiko::Particle::GetTextureTotalTiles() const
{
    return emitter->GetTextureTotalTiles();
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

const float4& Hachiko::Particle::GetCurrentColor() const
{
    return current_color;
}

void Hachiko::Particle::SetCurrentColor(const float4& color)
{
    this->current_color = color;
}

const float2& Hachiko::Particle::GetCurrentSize() const
{
    return current_size;
}

void Hachiko::Particle::SetCurrentSize(const float2& size)
{
    // Apply same size for both coordinates. Particles are always squares!
    this->current_size.x = size.x;
    this->current_size.y = size.x;
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

void Hachiko::Particle::SetCurrentAnimationFrame(unsigned current_animation_frame)
{
    this->current_animation_frame = current_animation_frame;
}

void Hachiko::Particle::SetEmitter(ComponentParticleSystem* emitter)
{
    this->emitter = emitter;
}
