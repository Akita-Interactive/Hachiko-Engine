#include "core/hepch.h"
#include "BloomManager.h"
#include "StandaloneGLTexture.h"

#include "Application.h"
#include "modules/ModuleProgram.h"
#include "modules/ModuleRender.h"

constexpr const char* const CONFIG_GAUSSIAN_INTENSITY = "bloom_gaussian_intensity";
constexpr const char* const CONFIG_GAUSSIAN_SIGMA = "bloom_gaussian_sigma";
constexpr const char* const CONFIG_GAUSSIAN_BLUR_PIXEL_SIZE = "bloom_gaussian_pixel_size";

Hachiko::BloomManager::~BloomManager() 
{
    Dispose();
}

void Hachiko::BloomManager::Initialize() 
{
    if (_initialized)
    {
        return;
    }

    _main_texture = CreateBloomTexture();
    _temp_gaussian_texture = CreateBloomTexture();

    _initialized = true;
}

void Hachiko::BloomManager::Resize(unsigned int width, unsigned int height)
{
    _main_texture->Resize(width, height);
    _temp_gaussian_texture->Resize(width, height);
}

void Hachiko::BloomManager::ApplyBloom(unsigned int texture_to_use) const
{
    // Copy texture_to_use to _main_texture:
    // Bind frame buffer attached to main texture:
    _main_texture->BindBuffer(true);
    App->program->GetProgram(Program::Programs::TEXTURE_COPY)->Activate();
    // Bind the texture to copy:
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_to_use);
    // Render the texture_to_used to a quad in full NDC:
    App->renderer->RenderFullScreenQuad();
    glBindVertexArray(0);
    // Deactivate copy texture shader:
    Program::Deactivate();
    // Unbind buffers and textures:
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Blur the texture, so we have a bloom effect:
    const int blur_pixel_size_integer = BlurPixelSize::ToInt(_blur_config.size);
    unsigned int width, height;
    _main_texture->GetSize(width, height);
    App->renderer->ApplyGaussianFilter(_main_texture->GetFrameBufferId(),
                        _main_texture->GetTextureId(),
                        _temp_gaussian_texture->GetFrameBufferId(),
                        _temp_gaussian_texture->GetTextureId(),
                        _blur_config.intensity,
                        _blur_config.sigma,
                        blur_pixel_size_integer,
                        width,
                        height,
                        App->program->GetProgram(Program::Programs::GAUSSIAN_FILTERING));
}

void Hachiko::BloomManager::BindForReading() const
{
    _main_texture->BindForReading(6);
}

void Hachiko::BloomManager::Dispose() 
{
    if (!_initialized)
    {
        return;
    }

    _initialized = false;
    delete _main_texture;
    delete _temp_gaussian_texture;
}

void Hachiko::BloomManager::SaveConfig(YAML::Node& node) const 
{
    node[CONFIG_GAUSSIAN_INTENSITY] = _blur_config.intensity;
    node[CONFIG_GAUSSIAN_SIGMA] = _blur_config.sigma; // #sigmagrindset.
    node[CONFIG_GAUSSIAN_BLUR_PIXEL_SIZE] = BlurPixelSize::ToIndex(_blur_config.size);
}

void Hachiko::BloomManager::LoadConfig(const YAML::Node& node) 
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

    const YAML::Node& gaussian_blur_pixel_size = node[CONFIG_GAUSSIAN_BLUR_PIXEL_SIZE];
    if (gaussian_blur_pixel_size.IsDefined())
    {
        int temp_size = gaussian_blur_pixel_size.as<int>();

        _blur_config.size = BlurPixelSize::IsValidPixelSize(temp_size) 
            ? static_cast<BlurPixelSize::Type>(temp_size) 
            : BloomDefaultValues::PIXEL_SIZE;
    }
}

void Hachiko::BloomManager::DrawEditorContent() 
{
    BlurConfigUtil::ShowInEditorUI(&_blur_config);
}

Hachiko::StandaloneGLTexture* Hachiko::CreateBloomTexture()
{
    return new StandaloneGLTexture(BloomDefaultValues::TEXTURE_WIDTH,
                                   BloomDefaultValues::TEXTURE_HEIGHT,
                                   GBuffer::GetEmissiveTextureInternalFormat(),
                                   0,
                                   GBuffer::GetEmissiveTextureFormat(),
                                   GBuffer::GetEmissiveTextureType(),
                                   GL_LINEAR,
                                   GL_LINEAR);
}
