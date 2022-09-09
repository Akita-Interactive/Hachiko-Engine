#include "core/hepch.h"
#include "core/Scene.h"
#include "core/GameObject.h"

#include "core/rendering/ShadowManager.h"
#include "core/rendering/Program.h"
#include "core/rendering/Uniforms.h"

#include "components/ComponentTransform.h"
#include "components/ComponentCamera.h"
#include "components/ComponentDirLight.h"

#include "modules/ModuleSceneManager.h"
#include "modules/ModuleCamera.h"

Hachiko::ShadowManager::ShadowManager() {}

Hachiko::ShadowManager::~ShadowManager() 
{
    FreeShadowMap();
}

void Hachiko::ShadowManager::GenerateShadowMap() 
{
    // Set the clamped border color of the shadow map texture so that non-covered areas does not 
    // have non-sense shadows:
    float clamp_color[4] = {1.0f, 1.0f, 1.0f, 1.0f};

    // Generate shadow map frame buffer object:
    glGenFramebuffers(1, &_shadow_map_fbo);
    // Generate shadow map texture:
    glGenTextures(1, &_shadow_map_texture);
    glBindTexture(GL_TEXTURE_2D, _shadow_map_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _shadow_map_width, _shadow_map_height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clamp_color);
    glGenerateTextureMipmap(_shadow_map_texture);
    // Bind it to the color attachment 0 of shadow map fbo:
    glBindFramebuffer(GL_FRAMEBUFFER, _shadow_map_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _shadow_map_texture, 0);
    // Generate and bind _shadow_map_depth as the depth buffer of shadow map fbo:
    glGenRenderbuffers(1, &_shadow_map_depth);
    glBindRenderbuffer(GL_RENDERBUFFER, _shadow_map_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _shadow_map_width, _shadow_map_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _shadow_map_depth);

    // Unbind the shadow map frame buffer:
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Generate the necessary stuff for the temporary texture that is identical to the actual shadow
    // map texture configuration which we will use it as the temp texture to blur the shadows.
    // Generate temporary shadow map fbo:
    glGenFramebuffers(1, &_shadow_map_fbo_temp);
    // Generate temporary shadow map texture:
    glGenTextures(1, &_shadow_map_texture_temp);
    glBindTexture(GL_TEXTURE_2D, _shadow_map_texture_temp);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _shadow_map_width, _shadow_map_height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clamp_color);
    // Bind shadow_map_filtered_texture on 0 color attachment of shadow_map_filtered_fbo:
    glBindFramebuffer(GL_FRAMEBUFFER, _shadow_map_fbo_temp);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _shadow_map_texture_temp, 0);

    // Unbind the temporary shadow map frame buffer:
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Hachiko::ShadowManager::FreeShadowMap() 
{
    glDeleteTextures(1, &_shadow_map_texture);
    glDeleteTextures(1, &_shadow_map_depth);
    glDeleteBuffers(1, &_shadow_map_fbo);

    glDeleteTextures(1, &_shadow_map_texture_temp);
    glDeleteBuffers(1, &_shadow_map_fbo_temp);
}

void Hachiko::ShadowManager::BindShadowMapTexture(unsigned location) const 
{
    glActiveTexture(GL_TEXTURE0 + location);
    glBindTexture(GL_TEXTURE_2D, _shadow_map_texture);
}

void Hachiko::ShadowManager::CalculateLightFrustum() 
{
    bool camera_changed = DetectCameraChanges();
    bool light_changed = DetectLightChanges();

    if (!camera_changed && !light_changed)
    {
        return;
    }

    ComponentCamera* camera = App->camera->GetRenderingCamera();

    if (camera == nullptr)
    {
        return;
    }

    constexpr const size_t NUM_CORNERS = 8;
    float3 camera_frustum_corners[NUM_CORNERS];
    camera->GetFrustum().GetCornerPoints(camera_frustum_corners);

    // Calculate sphere center:
    float3 camera_frustum_sphere_center = float3::zero;
    for (size_t i = 0; i < NUM_CORNERS; ++i)
    {
        camera_frustum_sphere_center += camera_frustum_corners[i];
    }
    camera_frustum_sphere_center = camera_frustum_sphere_center / 8.0f;

    // Calculate the sphere radius, it's the maximum of distances between
    // center and frustum corners:
    float camera_frustum_sphere_radius = 0.0f;
    for (size_t i = 0; i < NUM_CORNERS; ++i)
    {
        camera_frustum_sphere_radius = std::max(camera_frustum_sphere_center.Distance(camera_frustum_corners[i]), camera_frustum_sphere_radius);
    }

    // Set frustum kind:
    _directional_light_frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
    // Set the orthographic width and height of the light frustum to be the
    // diameter of enclosing sphere of camera frustum:
    float orthographic_size = camera_frustum_sphere_radius * 2.0f;
    _directional_light_frustum.SetOrthographic(orthographic_size, orthographic_size);
    // Set far plane to be same with orthographic width and height, and set
    // near to be 0:
    _directional_light_frustum.SetViewPlaneDistances(0.0f, orthographic_size);
    // Set the directional light direction:
    float3 direction = _directional_light_direction.Normalized();
    // Calculate right to get up:
    float3 right = direction.Cross(float3::unitY).Normalized();
    // Set position, front and up of the light frustum:
    _directional_light_frustum.SetFrame(
        camera_frustum_sphere_center - direction * camera_frustum_sphere_radius, 
        direction, 
        right.Cross(direction).Normalized());

    // Assign projection and view matrices from the calculated frustum:
    _directional_light_projection = _directional_light_frustum.ComputeProjectionMatrix();
    _directional_light_view = _directional_light_frustum.ComputeViewMatrix();
}

void Hachiko::ShadowManager::BindShadowMapGenerationPassUniforms(const Program* program) const 
{
    const ComponentDirLight* light = GetDirectionalLight();

    if (light == nullptr)
    {
        return;
    }

    BindCommonUniforms(program);

    light->GetShadowProperties().BindForShadowMapGenerationPass(program);
}

void Hachiko::ShadowManager::BindLightingPassUniforms(const Program* program) const
{
    const ComponentDirLight* light = GetDirectionalLight();

    if (light == nullptr)
    {
        return;
    }

    BindCommonUniforms(program);

    light->GetShadowProperties().BindForLightingPass(program);
}

void Hachiko::ShadowManager::ApplyGaussianBlur(const Program* program) const 
{
    if (!_gaussian_blurring_enabled)
    {
        return;
    }

    const ComponentDirLight* light = GetDirectionalLight();

    if (light == nullptr)
    {
        return;
    }

    App->renderer->ApplyGaussianFilter(
        _shadow_map_fbo,
        _shadow_map_texture,
        _shadow_map_fbo_temp,
        _shadow_map_texture_temp, 
        light->GetShadowProperties().GetGaussianFilterBlurAmount(),
        light->GetShadowProperties().GetGaussianBlurSigma(),
        light->GetShadowProperties().GetGaussianBlurSize(),
        _shadow_map_width,
        _shadow_map_height,
        program);
}

void Hachiko::ShadowManager::BindBufferForDrawing() const 
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, _shadow_map_fbo);
    glViewport(0, 0, _shadow_map_width, _shadow_map_height);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void Hachiko::ShadowManager::UnbindBuffer() const 
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Hachiko::ShadowManager::SetGaussianBlurringEnabled(bool enabled) 
{
    _gaussian_blurring_enabled = enabled;
}

const Frustum& Hachiko::ShadowManager::GetDirectionalLightFrustum() const
{
    return _directional_light_frustum;
}

bool Hachiko::ShadowManager::IsGaussianBlurringEnabled() const
{
    return _gaussian_blurring_enabled;
}

const Hachiko::ComponentDirLight* Hachiko::ShadowManager::GetDirectionalLight() const
{
    if (App->scene_manager->GetActiveScene()->dir_lights.size() <= 0)
    {
        return nullptr;
    }

    return App->scene_manager->GetActiveScene()->dir_lights[0];
}

void Hachiko::ShadowManager::BindCommonUniforms(const Program* program) const 
{
    program->BindUniformFloat4x4(Uniforms::ShadowMap::LIGHT_PROJECTION, _directional_light_projection.ptr());
    program->BindUniformFloat4x4(Uniforms::ShadowMap::LIGHT_VIEW, _directional_light_view.ptr());
}

bool Hachiko::ShadowManager::DetectCameraChanges()
{
    ComponentCamera* camera = App->camera->GetRenderingCamera();

    bool changed = false;
    
    if (camera == nullptr)
    {
        changed = (camera != nullptr) != _camera_exists;

        if (!changed)
        {
            return changed;
        }

        _camera_exists = false;
        _camera_transform = float4x4::identity;
        _camera_view = float4x4::identity;
        _camera_projection = float4x4::identity;

        return changed;
    }

    _camera_exists = true;
    
    if (!camera->GetGameObject()->GetTransform()->GetGlobalMatrix().Equals(_camera_transform))
    {
        changed = true;
        _camera_transform = camera->GetGameObject()->GetTransform()->GetGlobalMatrix();
    }

    if (!camera->GetProjectionMatrix().Equals(_camera_projection))
    {
        changed = true;
        _camera_projection = camera->GetProjectionMatrix();
    }

    if (!camera->GetViewMatrix().Equals(_camera_view))
    {
        changed = true;
        _camera_view = camera->GetViewMatrix();
    }

    return changed;
}

bool Hachiko::ShadowManager::DetectLightChanges()
{
    const ComponentDirLight* light = GetDirectionalLight();

    bool changed = false;

    if (light == nullptr)
    {
        changed = (light != nullptr) != _light_exists;

        if (!changed)
        {
            return changed;
        }

        _directional_light_direction = float3::zero;
        _light_exists = false;

        return changed;
    }

    _light_exists = true;

    if (!_directional_light_direction.Equals(light->GetDirection()))
    {
        changed = true;
        _directional_light_direction = light->GetDirection();
    }

    return changed;
}
