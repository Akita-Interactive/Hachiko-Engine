#pragma once

#include "BlurPixelSize.h"

namespace Hachiko
{
    struct BlurConfig
    {
        float intensity = 1.f;
        float sigma = 1.45f;
        BlurPixelSize::Type size = BlurPixelSize::Type::Gaussian7x7;
    };

    namespace BlurConfigUtil
    {
        bool ShowInEditorUI(BlurConfig* blur_config);
        int GetBlurPixelSizeInt(BlurConfig* blur_config);
    }
}