#pragma once


namespace Hachiko
{
class ComponentDirLight;
class ComponentCamera;
class Program;

// NOTE: Be careful while using this class as it has destructor defined but
// has no user defined copy ctor, copy assignment operator, move ctor and move
// assignment operator. Any copy will be doing a shallow copy where the fbos
// of both shadow managers will be the same. These special methods were not
// defined because ShadowManager is intended to be a class with a single
// instance in whole engine.
class ShadowManager
{

public:
    ShadowManager();
    ~ShadowManager();

    void GenerateShadowMap();
    void FreeShadowMap() const;
    void BindShadowMapTexture(unsigned location) const; 
    void LazyCalculateLightFrustum();
    void CalculateLightFrustum();
    void BindShadowMapGenerationPassUniforms(const Program* program) const;
    void BindLightingPassUniforms(const Program* program) const;
    void ApplyGaussianBlur(const Program* program) const;
    void BindBufferForDrawing() const;
    static void UnbindBuffer();

    void SetGaussianBlurringEnabled(bool enabled);

    [[nodiscard]] const Frustum& GetDirectionalLightFrustum() const;
    [[nodiscard]] bool IsGaussianBlurringEnabled() const;
    [[nodiscard]] const AABB& GetLightFrustumBoundingBox() const;

private:
    [[nodiscard]] static const ComponentDirLight* GetDirectionalLight();
    void BindCommonUniforms(const Program* program) const;
    bool DetectLightChanges();
    bool CheckCameraAndBoundingRegionChanges();


private:
    // Bounding box that covers the region for current light frustum. When the
    // main camera goes out of bounds, the recalculation of light frustum will
    // be triggered, resulting in this being re-triggered:
    AABB _light_frustum_bounding_box = {};

    // Light frustum related:
    float4x4 _directional_light_projection = float4x4::identity;
    float4x4 _directional_light_view = float4x4::identity;
    Frustum _directional_light_frustum = {};
    
    // Cache variables for checking for changes to avoid constant
    // reconstruction of _directional_light_frustum each frame:
    float3 _directional_light_direction = float3::zero;
    bool _camera_exists = false;
    bool _light_exists = false;
    bool _gaussian_blurring_enabled = true;
    UID _camera_uid = 0;
    float _light_frustum_bounding_box_padding = 0.0f;
    
    // Shadow map texture related:
    unsigned _shadow_map_fbo = 0;
    unsigned _shadow_map_texture = 0;
    unsigned _shadow_map_depth = 0;
    unsigned _shadow_map_fbo_temp = 0;
    unsigned _shadow_map_texture_temp = 0;
    const int _shadow_map_width = 5120;
    const int _shadow_map_height = 5120;
};
} // namespace Hachiko