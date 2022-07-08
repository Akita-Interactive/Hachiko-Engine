#pragma once

namespace Hachiko::ShadowMappingDefaults
{
    constexpr const float GAUSSIAN_FILTER_BLUR_AMOUNT = 0.62f;
    constexpr const float MIN_VARIANCE = 0.001f;
    constexpr const float LIGHT_BLEEDING_REDUCTION_AMOUNT = 0.25f;
    constexpr const float BIAS = 0.00001f;
    constexpr const float EXPONENT = 14.37f;
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
    bool DrawEditorContent();
    
    [[nodiscard]] float GetGaussianFilterBlurAmount() const;
    [[nodiscard]] float GetMinVariance() const;
    [[nodiscard]] float GetLightBleedingReductionAmount() const;
    [[nodiscard]] float GetBias() const;
    [[nodiscard]] float GetExponent() const;

private:
    void BindCommon(const Program* program) const;

private:
    float _gaussian_filter_blur_amount = 0.0f;
    float _min_variance = 0.0f;
    float _light_bleeding_reduction_amount = 0.0f;
    float _bias = 0.0f;
    float _exponent = 0.0f;
};

} // namespace Hachiko
