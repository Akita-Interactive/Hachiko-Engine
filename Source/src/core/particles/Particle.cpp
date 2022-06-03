#include "core/hepch.h"
#include "Particle.h"

#include "modules/ModuleRender.h"
#include "components/ComponentCamera.h"
#include "components/ComponentTransform.h"
#include "components/ComponentParticleSystem.h"

using namespace Hachiko;

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
    current_life = emitter->GetParticlesLife().values.x;
    current_position = emitter->GetGameObject()->GetTransform()->GetGlobalPosition();
    current_size = emitter->GetParticlesSize().values;
}

void Particle::Draw(ComponentCamera* camera, Program* program) const
{
    glActiveTexture(GL_TEXTURE0);
    int glTexture = 0;
    //has_texture = 0;
    //if (texture != nullptr)
    //{
    //    glTexture = texture->GetImageId();
    //    Hachiko::ModuleTexture::Bind(glTexture, static_cast<int>(Hachiko::ModuleProgram::TextureSlots::DIFFUSE));
    //    has_texture = 1;
    //}

    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    if (render_mode == ParticleRenderMode::PARTICLE_ADDITIVE)
    {
        glBlendFunc(GL_ONE, GL_ONE);
    }
    else
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    float4x4 model_matrix = float4x4::identity;
    GetModelMatrix(camera, model_matrix);
    program->BindUniformFloat4x4("model", model_matrix.ptr());
    program->BindUniformFloat4x4("view", &camera->GetViewMatrix()[0][0]);
    program->BindUniformFloat4x4("proj", &camera->GetProjectionMatrix()[0][0]);

    //program->BindUniformFloat("x_factor", &x_factor);
    //program->BindUniformFloat("y_factor", &y_factor);
    //program->BindUniformFloat("current_frame", &current_frame);
    //program->BindUniformFloat2("animation_index", animation_index.ptr());

    program->BindUniformFloat4("input_color", current_color.ptr());
    //program->BindUniformInts("has_texture", 1, &has_texture);

    //int flip_x = has_flip_x ? 1 : 0;
    //int flip_y = has_flip_y ? 1 : 0;
    //program->BindUniformInts("flip_x", 1, &flip_x);
    //program->BindUniformInts("flip_y", 1, &flip_y);

    glBindVertexArray(App->renderer->GetParticleVao());
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    // Clear
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    return;
}

void Hachiko::Particle::GetModelMatrix(ComponentCamera* camera, float4x4& out_matrix) const
{
    // Vertical (cilindrical) orientation
    float3 cameraPos = camera->GetFrustum()->Pos();
    float3 cameraDir = (float3(cameraPos.x, current_position.y, cameraPos.z) - current_position).Normalized();
    out_matrix = float4x4::LookAt(float3::unitZ, cameraDir, float3::unitY, float3::unitY);
    out_matrix = float4x4::FromTRS(current_position, out_matrix.RotatePart(), float3(current_size, 0.0f));
}

float Particle::GetInitialLife() const
{
    return initial_life;
}

void Particle::SetInitialLife(const float initial_life)
{
    this->initial_life = initial_life;
}

float Particle::GetInitialSpeed() const
{
    return initial_speed;
}

void Particle::SetInitialSpeed(const float initial_speed)
{
    this->initial_speed = initial_speed;
}

const float4& Particle::GetInitialColor() const
{
    return initial_color;
}

void Particle::SetInitialColor(const float4& initial_color)
{
    this->initial_color = initial_color;
}

const float2& Particle::GetInitialSize() const
{
    return initial_size;
}

void Particle::SetInitialSize(const float2& initial_size)
{
    this->initial_size = initial_size;
}

const float3& Particle::GetInitialPosition() const
{
    return initial_position;
}

void Particle::SetInitialPosition(const float3& initial_position)
{
    this->initial_position = initial_position;
}

float Particle::GetCurrentLife() const
{
    return current_life;
}

void Particle::SetCurrentLife(const float current_life)
{
    this->current_life = current_life;
}

float Particle::GetCurrentSpeed() const
{
    return current_speed;
}

void Particle::SetCurrentSpeed(const float current_speed)
{
    this->current_speed = current_speed;
}

const float4& Particle::GetCurrentColor() const
{
    return current_color;
}

void Particle::SetCurrentColor(const float4& current_color)
{
    this->current_color = current_color;
}

const float2& Particle::GetCurrentSize() const
{
    return current_size;
}

void Particle::SetCurrentSize(const float2& current_size)
{
    this->current_size = current_size;
}

const float3& Particle::GetCurrentPosition() const
{
    return current_position;
}

void Particle::SetCurrentPosition(const float3& current_position)
{
    this->current_position = current_position;
}

ParticleRenderMode Particle::GetRenderMode() const
{
    return render_mode;
}

void Particle::SetRenderMode(const ParticleRenderMode render_mode)
{
    this->render_mode = render_mode;
}

void Hachiko::Particle::SetEmitter(ComponentParticleSystem* emitter) 
{
    this->emitter = emitter;
}

const ComponentParticleSystem* Hachiko::Particle::GetEmitter()
{
    return emitter;
}


