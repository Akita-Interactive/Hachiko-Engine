#pragma once

namespace Hachiko::Uniforms
{
    namespace DeferredRendering
    {
        constexpr const char* MODE = "mode";
    }

    namespace ShadowMap
    {
        constexpr const char* BIAS = "shadow_bias";
        constexpr const char* EXPONENT = "exponent";
        constexpr const char* MIN_VARIANCE = "min_variance";
        constexpr const char* LIGHT_BLEEDING_REDUCTION_AMOUNT = "light_bleeding_reduction_amount";
        constexpr const char* LIGHT_PROJECTION = "light_projection";
        constexpr const char* LIGHT_VIEW = "light_view";
        constexpr const char* RENDER_MODE = "render_shadows";

    } // namespace ShadowMap

    namespace Filtering
    {
        constexpr const char* GAUSSIAN_BLUR_SCALE = "blur_scale";
        constexpr const char* GAUSSIAN_BLUR_PIXELS = "blur_pixels";
        constexpr const char* GAUSSIAN_BLUR_SIGMA = "blur_sigma";
    } // namespace Filtering

    namespace ImageBasedLighting
    {

    } // namespace ImageBasedLighting

    namespace Fog
    {
        constexpr const char* COLOR = "fog_color";
        constexpr const char* GLOBAL_DENSITY = "fog_global_density";
        constexpr const char* HEIGHT_FALLOFF = "fog_height_falloff";
    }

} // namespace Hachiko::Uniforms
