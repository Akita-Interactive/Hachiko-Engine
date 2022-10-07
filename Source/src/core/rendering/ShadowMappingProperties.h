#pragma once

#include "BlurPixelSize.h"

namespace Hachiko::ShadowMappingDefaults
{
    constexpr float BIAS = 0.0003f;
    constexpr float LIGHT_BLEEDING_REDUCTION_AMOUNT = 0.5f;
    constexpr float MIN_VARIANCE = 0.001f;
    constexpr float EXPONENT = 7.0f;
    constexpr float GAUSSIAN_FILTER_BLUR_AMOUNT = 0.85f;
    constexpr BlurPixelSize::Type BLUR_SIZE = BlurPixelSize::Type::Gaussian5x5;
    constexpr float SIGMA = 1.45f; // #sigmagrindset
    constexpr float LIGHT_FRUSTUM_BOUNDING_BOX_PADDING = 3.0f;
} // namespace Hachiko::ShadowMappingDefaults

namespace Hachiko
{

class Program;

class ShadowMappingProperties
{
public:
    ShadowMappingProperties();

    void BindForShadowMapGenerationPass(const Program* program) const;
    void BindForLightingPass(const Program* program) const;

    void Save(YAML::Node& node) const;
    void Load(const YAML::Node& node);

    void ResetToDefaults();
    void SetGaussianFilterBlurAmount(float value);
    void SetMinVariance(float value);
    void SetLightBleedingReductionAmount(float value);
    void SetBias(float value);
    void SetExponent(float value);
    void SetGaussianBlurSize(BlurPixelSize::Type value);
    void SetGaussianBlurSigma(float value);
    void SetLightFrustumBoundingBoxPadding(float value);
    bool DrawEditorContent();

    [[nodiscard]] float GetLightFrustumBoundingBoxPadding() const;
    [[nodiscard]] float GetGaussianFilterBlurAmount() const;
    [[nodiscard]] float GetMinVariance() const;
    [[nodiscard]] float GetLightBleedingReductionAmount() const;
    [[nodiscard]] float GetBias() const;
    [[nodiscard]] float GetExponent() const;
    [[nodiscard]] int GetGaussianBlurSize() const;
    [[nodiscard]] float GetGaussianBlurSigma() const;

private:
    void BindCommon(const Program* program) const;

private:
    float _light_frustum_bounding_box_padding = 0.0f;
    float _gaussian_filter_blur_amount = 0.0f;
    float _min_variance = 0.0f;
    float _light_bleeding_reduction_amount = 0.0f;
    float _bias = 0.0f;
    float _exponent = 0.0f;
    BlurPixelSize::Type _gaussian_blur_size = BlurPixelSize::Type::Gaussian5x5;
    float _gaussian_blur_sigma = 0.0f;
};

} // namespace Hachiko
