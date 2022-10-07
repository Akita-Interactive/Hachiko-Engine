#include "core/hepch.h"
#include "core/rendering/ShadowMappingProperties.h"
#include "core/rendering/Uniforms.h"
#include "core/rendering/Program.h"
#include "ui/widgets/Widgets.h"

constexpr const char* LIGHT_FRUSTUM_BOUNDING_BOX_PADDING = 
    "light_frustum_bounding_box_padding";

Hachiko::ShadowMappingProperties::ShadowMappingProperties()
{
    ResetToDefaults();
}

void Hachiko::ShadowMappingProperties::BindForShadowMapGenerationPass(const Program* program) const 
{
    BindCommon(program);
}

void Hachiko::ShadowMappingProperties::BindForLightingPass(const Program* program) const 
{
    BindCommon(program);

    program->BindUniformFloat(
        Uniforms::ShadowMap::LIGHT_BLEEDING_REDUCTION_AMOUNT, 
        &_light_bleeding_reduction_amount);
    program->BindUniformFloat(Uniforms::ShadowMap::MIN_VARIANCE, &_min_variance);
    program->BindUniformFloat(Uniforms::ShadowMap::BIAS, &_bias);
}

void Hachiko::ShadowMappingProperties::Save(YAML::Node& node) const 
{
    node[Uniforms::ShadowMap::LIGHT_BLEEDING_REDUCTION_AMOUNT] = _light_bleeding_reduction_amount;
    node[Uniforms::ShadowMap::BIAS] = _bias;
    node[Uniforms::ShadowMap::MIN_VARIANCE] = _min_variance;
    node[Uniforms::ShadowMap::EXPONENT] = _exponent;
    node[Uniforms::Filtering::GAUSSIAN_BLUR_SCALE] = _gaussian_filter_blur_amount;
    node[Uniforms::Filtering::GAUSSIAN_BLUR_PIXELS] = GetGaussianBlurSize();
    node[Uniforms::Filtering::GAUSSIAN_BLUR_SIGMA] = _gaussian_blur_sigma;
    node[LIGHT_FRUSTUM_BOUNDING_BOX_PADDING] = _light_frustum_bounding_box_padding;
}

void Hachiko::ShadowMappingProperties::Load(const YAML::Node& node) 
{
    // We already set the default values from constructor, no needs to do 
    // unnecessary re-assigning.

    if (node[Uniforms::ShadowMap::LIGHT_BLEEDING_REDUCTION_AMOUNT].IsDefined())
    {
        _light_bleeding_reduction_amount = node
            [Uniforms::ShadowMap::LIGHT_BLEEDING_REDUCTION_AMOUNT].as<float>();
    }

    if (node[Uniforms::ShadowMap::BIAS].IsDefined())
    {
        _bias = node[Uniforms::ShadowMap::BIAS].as<float>();
    }

    if (node[Uniforms::ShadowMap::MIN_VARIANCE].IsDefined())
    {
        _min_variance = node[Uniforms::ShadowMap::MIN_VARIANCE].as<float>();
    }

    if (node[Uniforms::ShadowMap::EXPONENT].IsDefined())
    {
        _exponent = node[Uniforms::ShadowMap::EXPONENT].as<float>();
    }

    if (node[Uniforms::Filtering::GAUSSIAN_BLUR_SCALE].IsDefined())
    {
        _gaussian_filter_blur_amount = node[Uniforms::Filtering::GAUSSIAN_BLUR_SCALE].as<float>();
    }

    if (node[Uniforms::Filtering::GAUSSIAN_BLUR_PIXELS].IsDefined())
    {
        int temp_size = node[Uniforms::Filtering::GAUSSIAN_BLUR_PIXELS].as<int>();
        
        _gaussian_blur_size = BlurPixelSize::IsValidPixelSize(temp_size) 
            ? static_cast<BlurPixelSize::Type>(temp_size) 
            : ShadowMappingDefaults::BLUR_SIZE;
    }

    if (node[Uniforms::Filtering::GAUSSIAN_BLUR_SIGMA].IsDefined())
    {
        _gaussian_blur_sigma = node[Uniforms::Filtering::GAUSSIAN_BLUR_SIGMA].as<float>();
    }

    if (node[LIGHT_FRUSTUM_BOUNDING_BOX_PADDING].IsDefined())
    {
        _light_frustum_bounding_box_padding = 
            node[LIGHT_FRUSTUM_BOUNDING_BOX_PADDING].as<float>();
    }
}

void Hachiko::ShadowMappingProperties::ResetToDefaults() 
{
    _gaussian_filter_blur_amount = 
        ShadowMappingDefaults::GAUSSIAN_FILTER_BLUR_AMOUNT;
    _min_variance = ShadowMappingDefaults::MIN_VARIANCE;
    _light_bleeding_reduction_amount = 
        ShadowMappingDefaults::LIGHT_BLEEDING_REDUCTION_AMOUNT;
    _bias = ShadowMappingDefaults::BIAS;
    _exponent = ShadowMappingDefaults::EXPONENT;
    _gaussian_blur_sigma = ShadowMappingDefaults::SIGMA;
    _gaussian_blur_size = ShadowMappingDefaults::BLUR_SIZE;
    _light_frustum_bounding_box_padding = 
        ShadowMappingDefaults::LIGHT_FRUSTUM_BOUNDING_BOX_PADDING;
}

void Hachiko::ShadowMappingProperties::SetGaussianFilterBlurAmount(const float value)
{
    _gaussian_filter_blur_amount = value;

    std::clamp(_gaussian_filter_blur_amount, 0.0f, FLT_MAX);
}

void Hachiko::ShadowMappingProperties::SetMinVariance(const float value) 
{
    _min_variance = value;

    std::clamp(_min_variance, 0.0f, 1.0f);
} 

void Hachiko::ShadowMappingProperties::SetLightBleedingReductionAmount(const float value) 
{
    _light_bleeding_reduction_amount = value;

    std::clamp(_light_bleeding_reduction_amount, 0.0f, 1.0f);
}

void Hachiko::ShadowMappingProperties::SetBias(const float value) 
{
    _bias = value;

    std::clamp(_bias, 0.0f, 1.0f);
}

void Hachiko::ShadowMappingProperties::SetExponent(const float value) 
{
    _exponent = value;

    std::clamp(_exponent, 0.0f, FLT_MAX);
}

void Hachiko::ShadowMappingProperties::SetGaussianBlurSize(const BlurPixelSize::Type value) 
{
    _gaussian_blur_size = value;
}

void Hachiko::ShadowMappingProperties::SetGaussianBlurSigma(const float value) 
{
    _gaussian_blur_sigma = value;

    std::clamp(_gaussian_blur_sigma, 0.0f, FLT_MAX);
}

void Hachiko::ShadowMappingProperties::SetLightFrustumBoundingBoxPadding(const float value)
{
    _light_frustum_bounding_box_padding = std::clamp(value, 0.0f, FLT_MAX);
}

bool Hachiko::ShadowMappingProperties::DrawEditorContent() 
{
    Widgets::DragFloatConfig config_1;
    config_1.format = "%.9f";
    config_1.speed = 0.0001f;
    config_1.min = 0.0f;

    Widgets::DragFloatConfig config_2 = config_1;
    config_2.max = 1.0f;

    bool changes_made = false;

    if (DragFloat("Bias", _bias, &config_2))
    {
        changes_made = true;
    }

    if (DragFloat("Light bleeding reduction", _light_bleeding_reduction_amount, &config_2)) 
    {
        changes_made = true;
    }

    if (DragFloat("Minimum variance", _min_variance, &config_2))
    {
        changes_made = true;
    }

    if (DragFloat("Exponent", _exponent, &config_1))
    {
        changes_made = true;
    }

    if (DragFloat("Gaussian blur amount", _gaussian_filter_blur_amount, &config_1))
    {
        changes_made = true;
    }

    int current_index = ToIndex(_gaussian_blur_size);
    if (Widgets::Combo("Gaussian blur pixel size", &current_index, BlurPixelSize::blur_pixel_sizes_strings, BlurPixelSize::number_of_blur_pixel_sizes))
    {
        changes_made = true;
        _gaussian_blur_size = BlurPixelSize::FromIndex(current_index);
    }

    if (DragFloat("Gaussian blur sigma", _gaussian_blur_sigma, &config_1))
    {
        changes_made = true;
    }

    if (DragFloat("Light frustum bounding box padding", _light_frustum_bounding_box_padding, &config_1))
    {
        SetLightFrustumBoundingBoxPadding(_light_frustum_bounding_box_padding);
        changes_made = true;
    }

    if (ImGui::Button("Reset to defaults", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
    {
        ResetToDefaults();
        changes_made = true;
    }

    return changes_made;
}

float Hachiko::ShadowMappingProperties::GetLightFrustumBoundingBoxPadding() const
{
    return _light_frustum_bounding_box_padding;
}

float Hachiko::ShadowMappingProperties::GetGaussianFilterBlurAmount() const
{
    return _gaussian_filter_blur_amount;
}

float Hachiko::ShadowMappingProperties::GetMinVariance() const
{
    return _min_variance;
}

float Hachiko::ShadowMappingProperties::GetLightBleedingReductionAmount() const
{
    return _light_bleeding_reduction_amount;
}

float Hachiko::ShadowMappingProperties::GetBias() const
{
    return _bias;
}

float Hachiko::ShadowMappingProperties::GetExponent() const
{
    return _exponent;
}

int Hachiko::ShadowMappingProperties::GetGaussianBlurSize() const
{
    return static_cast<int>(_gaussian_blur_size);
}

float Hachiko::ShadowMappingProperties::GetGaussianBlurSigma() const
{
    return _gaussian_blur_sigma;
}

void Hachiko::ShadowMappingProperties::BindCommon(const Program* program) const 
{
    program->BindUniformFloat(Uniforms::ShadowMap::EXPONENT, &_exponent);
}