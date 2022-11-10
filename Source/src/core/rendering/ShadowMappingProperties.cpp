#include "core/hepch.h"
#include "core/rendering/ShadowMappingProperties.h"
#include "core/rendering/Uniforms.h"
#include "core/rendering/Program.h"

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
    node[Uniforms::Filtering::GAUSSIAN_BLUR_SCALE] = _blur_config.intensity;
    node[Uniforms::Filtering::GAUSSIAN_BLUR_PIXELS] = GetGaussianBlurSize();
    node[Uniforms::Filtering::GAUSSIAN_BLUR_SIGMA] = _blur_config.sigma;
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
        _blur_config.intensity = node[Uniforms::Filtering::GAUSSIAN_BLUR_SCALE].as<float>();
    }

    if (node[Uniforms::Filtering::GAUSSIAN_BLUR_PIXELS].IsDefined())
    {
        int temp_size = node[Uniforms::Filtering::GAUSSIAN_BLUR_PIXELS].as<int>();
        
        _blur_config.size = BlurPixelSize::IsValidPixelSize(temp_size) 
            ? static_cast<BlurPixelSize::Type>(temp_size) 
            : ShadowMappingDefaults::BLUR_SIZE;
    }

    if (node[Uniforms::Filtering::GAUSSIAN_BLUR_SIGMA].IsDefined())
    {
        _blur_config.sigma = node[Uniforms::Filtering::GAUSSIAN_BLUR_SIGMA].as<float>();
    }

    if (node[LIGHT_FRUSTUM_BOUNDING_BOX_PADDING].IsDefined())
    {
        _light_frustum_bounding_box_padding = 
            node[LIGHT_FRUSTUM_BOUNDING_BOX_PADDING].as<float>();
    }
}

void Hachiko::ShadowMappingProperties::ResetToDefaults() 
{
    _blur_config.intensity = 
        ShadowMappingDefaults::GAUSSIAN_FILTER_BLUR_AMOUNT;
    _min_variance = ShadowMappingDefaults::MIN_VARIANCE;
    _light_bleeding_reduction_amount = 
        ShadowMappingDefaults::LIGHT_BLEEDING_REDUCTION_AMOUNT;
    _bias = ShadowMappingDefaults::BIAS;
    _exponent = ShadowMappingDefaults::EXPONENT;
    _blur_config.sigma = ShadowMappingDefaults::SIGMA;
    _blur_config.size = ShadowMappingDefaults::BLUR_SIZE;
    _light_frustum_bounding_box_padding = 
        ShadowMappingDefaults::LIGHT_FRUSTUM_BOUNDING_BOX_PADDING;
}

void Hachiko::ShadowMappingProperties::SetGaussianFilterBlurAmount(const float value)
{
    _blur_config.intensity = value;

    std::clamp(_blur_config.intensity, 0.0f, FLT_MAX);
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
    _blur_config.size = value;
}

void Hachiko::ShadowMappingProperties::SetGaussianBlurSigma(const float value) 
{
    _blur_config.sigma = value;

    std::clamp(_blur_config.sigma, 0.0f, FLT_MAX);
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

    changes_made = changes_made || BlurConfigUtil::ShowInEditorUI(&_blur_config);

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
    return _blur_config.intensity;
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
    return BlurPixelSize::ToInt(_blur_config.size);
}

float Hachiko::ShadowMappingProperties::GetGaussianBlurSigma() const
{
    return _blur_config.sigma;
}

void Hachiko::ShadowMappingProperties::BindCommon(const Program* program) const 
{
    program->BindUniformFloat(Uniforms::ShadowMap::EXPONENT, &_exponent);
}