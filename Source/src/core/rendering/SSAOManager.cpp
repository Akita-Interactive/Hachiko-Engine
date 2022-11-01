#include "core/hepch.h"
#include "SSAOManager.h"
#include "StandaloneGLTexture.h"
#include "modules/ModuleProgram.h"
#include "Uniforms.h"

constexpr const char* const CONFIG_GAUSSIAN_INTENSITY = "ssao_gaussian_intensity";
constexpr const char* const CONFIG_GAUSSIAN_SIGMA = "ssao_gaussian_sigma";
constexpr const char* const CONFIG_GAUSSIAN_BLUR_PIXEL_SIZE = "ssao_gaussian_pixel_size";
constexpr const char* const CONFIG_SSAO_RADIUS = "ssao_radius";
constexpr const char* const CONFIG_SSAO_BIAS = "ssao_bias";
constexpr const char* const CONFIG_SSAO_POWER = "ssao_power";
constexpr const char* const CONFIG_SSAO_BLUR_ENABLED = "ssao_blur_enabled";

Hachiko::SSAOManager::SSAOManager()
{
}

Hachiko::SSAOManager::~SSAOManager()
{
    delete _main_texture;
    delete _temp_texture;
}

void Hachiko::SSAOManager::SetupSSAO(const int width, const int height)
{
    constexpr float kernel_size_inverse = 
        1.0f / static_cast<float>(SSAOConstants::KERNEL_SIZE);

    // We generate random direction vectors (normal oriented hemisphere) to
    // mimic light occlusion for each pixel.
    for (size_t i = 0; i < SSAOConstants::KERNEL_SIZE; ++i)
    {
        float3& current_sample = _kernel[i];

        // Give x and y values between -1.0f and 1.0f:
        current_sample.x = RandomUtil::RandomSigned();
        current_sample.y = RandomUtil::RandomSigned();
        // Give z value between 0.0f and 1.0f so that we have a hemisphere from
        // surface, not a full sphere:
        current_sample.z = RandomUtil::Random();

        // Normalize and apply a random weight:
        current_sample.Normalize();
        current_sample *= RandomUtil::Random();

        // Give more weight to the samples that are closer to the actual
        // fragment:
        float scale = kernel_size_inverse * i;
        scale = math::Lerp(0.1f, 1.0f, scale * scale);
        current_sample *= scale;
    }

    _main_texture = CreateSSAOTexture(width, height);
    _temp_texture = CreateSSAOTexture(width, height);

    // We generate random kernel rotations that we're gonna tile around the
    // screen instead of generating a rotation from scratch for each pixel:
    float3 ssao_noise[SSAOConstants::NOISE_PIXELS];
    for (float3& noise : ssao_noise)
    {
        noise.x = RandomUtil::RandomSigned();
        noise.y = RandomUtil::RandomSigned();
        noise.z = 0.0f;
    }

    // Generate the open gl texture and store the values inside it:
    glGenTextures(1, &_noise_texture);
    glBindTexture(GL_TEXTURE_2D, _noise_texture);
    glTexImage2D(
        GL_TEXTURE_2D, 
        0, 
        GL_RGB32F, 
        SSAOConstants::NOISE_SIZE, 
        SSAOConstants::NOISE_SIZE, 
        0, 
        GL_RGB, 
        GL_FLOAT, 
        &ssao_noise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void Hachiko::SSAOManager::ResizeSSAO(unsigned width, unsigned height) const
{
    _main_texture->Resize(width, height);
    _temp_texture->Resize(width, height);
}

void Hachiko::SSAOManager::BlurSSAO() const
{
    if (!_blur_enabled)
    {
        return;
    }

    unsigned width;
    unsigned height;
    _main_texture->GetSize(width, height);

    App->renderer->ApplyGaussianFilter(
        _main_texture->GetFrameBufferId(),
        _main_texture->GetTextureId(),
        _temp_texture->GetFrameBufferId(),
        _temp_texture->GetTextureId(),
        _blur_config.intensity,
        _blur_config.sigma,
        BlurPixelSize::ToInt(_blur_config.size),
        width,
        height,
        App->program->GetProgram(Program::Programs::GAUSSIAN_FILTERING));
}

void Hachiko::SSAOManager::DrawSSAO(
    const GBuffer& g_buffer, 
    const float4x4& camera_view,
    const float4x4& camera_proj,
    const unsigned fb_width,
    const unsigned fb_height) const
{
    // Bind SSAO fbo for drawing:
    _main_texture->BindBuffer();

    // Activate SSAO shader:
    Program* ssao_program = App->program->GetProgram(Program::Programs::SSAO);
    ssao_program->Activate();

    const float frame_buffer_width = static_cast<float>(fb_width);
    const float frame_buffer_height = static_cast<float>(fb_height);

    // Bind Uniforms:
    // Bind samples array:
    for (size_t i = 0; i < SSAOConstants::KERNEL_SIZE; ++i)
    {
        ssao_program->BindUniformFloat3(
            (Uniforms::SSAO::KERNEL_SAMPLES_PREFIX + 
            std::to_string(i) + 
            Uniforms::SSAO::KERNEL_SAMPLES_SUFFIX).c_str(), 
            _kernel[i].ptr());
    }

    // Bind projection matrix:
    ssao_program->BindUniformFloat4x4(
        Uniforms::SSAO::CAMERA_VIEW,
        camera_view.ptr());

    // Bind projection matrix:
    ssao_program->BindUniformFloat4x4(
        Uniforms::SSAO::CAMERA_PROJ,
        camera_proj.ptr());

    // Bind frame buffer sizes:
    // TODO: Make these uniforms used through variables.
    ssao_program->BindUniformFloat(
        Uniforms::SSAO::FBO_WIDTH, 
        &frame_buffer_width);
    ssao_program->BindUniformFloat(
        Uniforms::SSAO::FBO_HEIGHT, 
        &frame_buffer_height);
    // Bind ssao config related variables:
    ssao_program->BindUniformFloat(Uniforms::SSAO::RADIUS, &_radius);
    ssao_program->BindUniformFloat(Uniforms::SSAO::BIAS, &_bias);
    ssao_program->BindUniformFloat(Uniforms::SSAO::POWER, &_power);
    // Bind the generated noise texture:
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _noise_texture);
    // Bind necessary G-Buffer textures:
    g_buffer.BindSSAOTextures();

    // Render Quad:
    App->renderer->RenderFullScreenQuad();

    // Deactivate SSAO shader:
    Program::Deactivate();

    // Unbind textures:
    g_buffer.UnbindSSAOTextures();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Blur SSAO texture:
    BlurSSAO();
}

void Hachiko::SSAOManager::BindSSAOTexture(unsigned int position) const
{
    _main_texture->BindForReading(position);
}

void Hachiko::SSAOManager::UnbindSSAOTexture(unsigned int position) const
{
    glActiveTexture(GL_TEXTURE0 + position);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Hachiko::SSAOManager::ShowInEditorUI()
{
    Widgets::DragFloatConfig ssao_config;
    ssao_config.min = 0.0f;
    ssao_config.speed = 0.01f;
    Widgets::DragFloat("Radius", _radius, &ssao_config);
    Widgets::DragFloat("Bias", _bias, &ssao_config);
    Widgets::DragFloat("Power", _power, &ssao_config);
    Widgets::Checkbox("Blur enabled", &_blur_enabled);
    if (_blur_enabled)
    {
        BlurConfigUtil::ShowInEditorUI(&_blur_config);
    }
}

void Hachiko::SSAOManager::SaveConfig(YAML::Node& node) const
{
    node[CONFIG_GAUSSIAN_INTENSITY] = _blur_config.intensity;
    node[CONFIG_GAUSSIAN_SIGMA] = _blur_config.sigma;
    node[CONFIG_GAUSSIAN_BLUR_PIXEL_SIZE] = 
        BlurPixelSize::ToIndex(_blur_config.size);
    node[CONFIG_SSAO_BLUR_ENABLED] = _blur_enabled;
    node[CONFIG_SSAO_BIAS] = _bias;
    node[CONFIG_SSAO_RADIUS] = _radius;
    node[CONFIG_SSAO_POWER] = _power;
}

void Hachiko::SSAOManager::LoadConfig(const YAML::Node& node)
{
    const YAML::Node& gaussian_intensity = node[CONFIG_GAUSSIAN_INTENSITY];
    if (gaussian_intensity.IsDefined())
    {
        _blur_config.intensity = gaussian_intensity.as<float>();
    }

    const YAML::Node& gaussian_sigma = node[CONFIG_GAUSSIAN_SIGMA];
    if (gaussian_sigma.IsDefined())
    {
        _blur_config.sigma = gaussian_sigma.as<float>();
    }

    const YAML::Node& gaussian_blur_pixel_size = 
        node[CONFIG_GAUSSIAN_BLUR_PIXEL_SIZE];
    if (gaussian_blur_pixel_size.IsDefined())
    {
        int temp_size = gaussian_blur_pixel_size.as<int>();

        _blur_config.size = BlurPixelSize::IsValidPixelSize(temp_size)
            ? static_cast<BlurPixelSize::Type>(temp_size)
            : BloomDefaultValues::PIXEL_SIZE;
    }

    const YAML::Node& ssao_blur_enabled = node[CONFIG_SSAO_BLUR_ENABLED];
    if (ssao_blur_enabled.IsDefined())
    {
        _blur_enabled = ssao_blur_enabled.as<bool>();
    }

    const YAML::Node& ssao_bias = node[CONFIG_SSAO_BIAS];
    if (ssao_bias.IsDefined())
    {
        _bias = ssao_bias.as<float>();
    }

    const YAML::Node& ssao_radius = node[CONFIG_SSAO_RADIUS];
    if (ssao_radius.IsDefined())
    {
        _radius = ssao_radius.as<float>();
    }

    const YAML::Node& ssao_power = node[CONFIG_SSAO_POWER];
    if (ssao_radius.IsDefined())
    {
        _power = ssao_power.as<float>();
    }
}

Hachiko::StandaloneGLTexture* Hachiko::CreateSSAOTexture(
    const int width,  
    const int height)
{
    return new StandaloneGLTexture(
        width, 
        height, 
        GL_R32F, 
        0, 
        GL_RED, 
        GL_FLOAT, 
        GL_NEAREST, 
        GL_NEAREST, 
        true);
}