#pragma once


namespace Hachiko
{
class ComponentDirLight;
class ComponentCamera;
class Program;

class ShadowManager
{

public:
    ShadowManager();
    ~ShadowManager();

    void GenerateShadowMap();
    void FreeShadowMap();
    void BindShadowMapTexture(unsigned location) const; 
    void CalculateLightFrustum();
    void BindShadowMapGenerationPassUniforms(const Program* program) const;
    void BindLightingPassUniforms(const Program* program) const;
    void ApplyGaussianBlur(const Program* program) const;
    void BindBufferForDrawing() const;
    void UnbindBuffer() const;

    void SetGaussianBlurringEnabled(bool enabled);

    [[nodiscard]] const Frustum& GetDirectionalLightFrustum() const;
    [[nodiscard]] bool IsGaussianBlurringEnabled() const;

private:
    const ComponentDirLight* GetDirectionalLight() const;
    void BindCommonUniforms(const Program* program) const;
    bool DetectCameraChanges();
    bool DetectLightChanges();

private:
    // Light frustum related:
    float4x4 _directional_light_projection = float4x4::identity;
    float4x4 _directional_light_view = float4x4::identity;
    Frustum _directional_light_frustum = {};
    
    // Cache variables for checking for changes to avoid constant reconstruction
    // of _directional_light_frustum each frame:
    float3 _directional_light_direction = float3::zero;
    float4x4 _camera_transform = float4x4::identity;
    float4x4 _camera_projection = float4x4::identity;
    float4x4 _camera_view = float4x4::identity;
    bool _camera_exists = false;
    bool _light_exists = false;
    bool _gaussian_blurring_enabled = true;
    
    // Shadow map texture related:
    unsigned _shadow_map_fbo = 0;
    unsigned _shadow_map_texture = 0;
    unsigned _shadow_map_depth = 0;
    unsigned _shadow_map_fbo_temp = 0;
    unsigned _shadow_map_texture_temp = 0;
    const unsigned _shadow_map_width = 4096;
    const unsigned _shadow_map_height = 4096;
};

} // namespace Hachiko