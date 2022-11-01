#pragma once
#include "BlurConfig.h"

namespace Hachiko
{
    class StandaloneGLTexture;

    namespace SSAOConstants
    {
        constexpr int KERNEL_SIZE = 64;
        constexpr float KERNEL_SIZE_INVERSE = 
            1.0f / static_cast<float>(SSAOConstants::KERNEL_SIZE);
        constexpr int NOISE_SIZE = 4;
        constexpr int NOISE_PIXELS = NOISE_SIZE * NOISE_SIZE;
        constexpr float DEFAULT_RADIUS = 3.31f;
        constexpr float DEFAULT_BIAS = 0.76f;
        constexpr float DEFAULT_POWER = 4.48f;
    } // namespace SSAOConstants

    class SSAOManager
    {
    public:
        SSAOManager();
        ~SSAOManager();

        void SetupSSAO(int width, int height);
        void ResizeSSAO(unsigned int width, unsigned int height) const;
        void BlurSSAO() const;
        void DrawSSAO(
            const GBuffer& g_buffer, 
            const float4x4& camera_view, 
            const float4x4& camera_proj, 
            unsigned fb_width, 
            unsigned fb_height) const;
        void BindSSAOTexture(unsigned int position) const;
        void UnbindSSAOTexture(unsigned int position) const;
        void ShowInEditorUI();
        void SaveConfig(YAML::Node& node) const;
        void LoadConfig(const YAML::Node& node);

    private:
        float _radius = SSAOConstants::DEFAULT_RADIUS;
        float _bias = SSAOConstants::DEFAULT_BIAS;
        float _power = SSAOConstants::DEFAULT_POWER;
        bool _blur_enabled = true;
        BlurConfig _blur_config;
        StandaloneGLTexture* _main_texture = nullptr;
        StandaloneGLTexture* _temp_texture = nullptr;
        unsigned int _noise_texture = 0;
        float3 _kernel[SSAOConstants::KERNEL_SIZE];
    };

    // Creates SSAO Texture with desired configs, user is responsible for
    // deallocation.
    StandaloneGLTexture* CreateSSAOTexture(int width, int height);
} // namespace Hachiko