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
    App->program->GetProgram(Program::PROGRAMS::TEXTURE_COPY)->Activate();
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
    const int blur_pixel_size_integer = static_cast<int>(_gaussian_blur_pixel_size);
    unsigned int width, height;
    _main_texture->GetSize(width, height);
    App->renderer->ApplyGaussianFilter(_main_texture->GetFrameBufferId(),
                        _main_texture->GetTextureId(),
                        _temp_gaussian_texture->GetFrameBufferId(),
                        _temp_gaussian_texture->GetTextureId(),
                        _gaussian_intensity,
                        _gaussian_sigma,
                        blur_pixel_size_integer,
                        width,
                        height,
                        App->program->GetProgram(Program::PROGRAMS::GAUSSIAN_FILTERING));
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
    node[CONFIG_GAUSSIAN_INTENSITY] = _gaussian_intensity;
    node[CONFIG_GAUSSIAN_SIGMA] = _gaussian_sigma; // #sigmagrindset.
    node[CONFIG_GAUSSIAN_BLUR_PIXEL_SIZE] = static_cast<int>(_gaussian_blur_pixel_size);
}

void Hachiko::BloomManager::LoadConfig(const YAML::Node& node) 
{
    const YAML::Node& gaussian_intensity = node[CONFIG_GAUSSIAN_INTENSITY];
    if (gaussian_intensity.IsDefined())
    {
        _gaussian_intensity = gaussian_intensity.as<float>();
    }

    const YAML::Node& gaussian_sigma = node[CONFIG_GAUSSIAN_SIGMA];
    if (gaussian_sigma.IsDefined())
    {
        _gaussian_intensity = gaussian_sigma.as<float>();
    }

    const YAML::Node& gaussian_blur_pixel_size = node[CONFIG_GAUSSIAN_BLUR_PIXEL_SIZE];
    if (gaussian_blur_pixel_size.IsDefined())
    {
        int temp_size = gaussian_blur_pixel_size.as<int>();

        _gaussian_blur_pixel_size = BlurPixelSize::IsValidPixelSize(temp_size) 
            ? static_cast<BlurPixelSize::Type>(temp_size) 
            : BloomDefaultValues::PIXEL_SIZE;
    }
}

void Hachiko::BloomManager::DrawEditorContent() 
{
    static Widgets::DragFloatConfig config_1;
    config_1.format = "%.9f";
    config_1.speed = 0.01f;
    config_1.min = 0.0f;
    config_1.max = FLT_MAX;

    DragFloat("Gaussian blur intensity", _gaussian_intensity, &config_1);
    DragFloat("Gaussian blur sigma", _gaussian_sigma, &config_1);
    
    int current_index = ToIndex(_gaussian_blur_pixel_size);
    if (Widgets::Combo("Gaussian blur pixel size", &current_index, BlurPixelSize::blur_pixel_sizes_strings, BlurPixelSize::number_of_blur_pixel_sizes))
    {
        _gaussian_blur_pixel_size = BlurPixelSize::FromIndex(current_index);
    }
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
