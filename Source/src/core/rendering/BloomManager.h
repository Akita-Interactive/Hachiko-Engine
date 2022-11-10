#pragma once

#include "BlurConfig.h"

namespace Hachiko::BloomDefaultValues
{
constexpr const float INTENSITY = 3.2f;
constexpr const float SIGMA = 2.5f;
constexpr const BlurPixelSize::Type PIXEL_SIZE = BlurPixelSize::Type::Gaussian7x7;
constexpr const unsigned TEXTURE_WIDTH = 800;
constexpr const unsigned TEXTURE_HEIGHT = 600;
}

namespace Hachiko
{

class StandaloneGLTexture;

class BloomManager
{
public:
    BloomManager() = default;
    ~BloomManager();
    
    void Initialize();
    void Resize(unsigned int width, unsigned int height);
    void ApplyBloom(unsigned int texture_to_use) const;
    void BindForReading() const;
    void Dispose();
    void SaveConfig(YAML::Node& node) const;
    void LoadConfig(const YAML::Node& node);
    void DrawEditorContent();

private:
    StandaloneGLTexture* _main_texture = nullptr;
    StandaloneGLTexture* _temp_gaussian_texture = nullptr;

    BlurConfig _blur_config {
        BloomDefaultValues::INTENSITY,
        BloomDefaultValues::SIGMA,
        BloomDefaultValues::PIXEL_SIZE
    };

    bool _initialized = false;
};
    
// User handles the deletion:
StandaloneGLTexture* CreateBloomTexture();

} // namespace Hachiko