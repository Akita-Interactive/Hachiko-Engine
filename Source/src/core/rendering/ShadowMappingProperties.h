#pragma once

#include "BlurPixelSize.h"

namespace Hachiko::ShadowMappingDefaults
{
    constexpr const float GAUSSIAN_FILTER_BLUR_AMOUNT = 0.62f;
    constexpr const float MIN_VARIANCE = 0.001f;
    constexpr const float LIGHT_BLEEDING_REDUCTION_AMOUNT = 0.35f;
    constexpr const float BIAS = 0.00001f;
    constexpr const float EXPONENT = 14.37f;
    constexpr const float SIGMA = 3.5f;
    constexpr const BlurPixelSize::Type BLUR_SIZE = BlurPixelSize::Type::Gaussian11x11;
} // namespace Hachiko::ShadowMappingDefaults

namespace Hachiko
{

class Program;

class ShadowMappingProperties
{
public:
    ShadowMappingProperties();
    ~ShadowMappingProperties() = default;

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
    bool DrawEditorContent();
    
    [[nodiscard]] float GetGaussianFilterBlurAmount() const;
    [[nodiscard]] float GetMinVariance() const;
    [[nodiscard]] float GetLightBleedingReductionAmount() const;
    [[nodiscard]] float GetBias() const;
    [[nodiscard]] float GetExponent() const;
    [[nodiscard]] int GetGaussianBlurSize() const;
    [[nodiscard]] int GetGaussianBlurSigma() const;

private:
    void BindCommon(const Program* program) const;

private:
    float _gaussian_filter_blur_amount = 0.0f;
    float _min_variance = 0.0f;
    float _light_bleeding_reduction_amount = 0.0f;
    float _bias = 0.0f;
    float _exponent = 0.0f;
    BlurPixelSize::Type _gaussian_blur_size = BlurPixelSize::Type::Gaussian5x5;
    float _gaussian_blur_sigma = 0.0f;
};

} // namespace Hachiko
