#include "core/hepch.h"
#include "BlurConfig.h"

bool Hachiko::BlurConfigUtil::ShowInEditorUI(BlurConfig* blur_config)
{
    if (blur_config == nullptr)
    {
        return false;
    }

    bool changes_made = false;

    static Widgets::DragFloatConfig config_1;
    config_1.format = "%.9f";
    config_1.speed = 0.01f;
    config_1.min = 0.0f;
    config_1.max = FLT_MAX;

    if (DragFloat("Gaussian blur intensity", blur_config->intensity, &config_1))
    {
        changes_made = true;
    }

    if (DragFloat("Gaussian blur sigma", blur_config->sigma, &config_1))
    {
        changes_made = true;
    }

    int current_index = BlurPixelSize::ToIndex(blur_config->size);
    if (Widgets::Combo("Gaussian blur pixel size", &current_index, BlurPixelSize::blur_pixel_sizes_strings, BlurPixelSize::number_of_blur_pixel_sizes))
    {
        blur_config->size = BlurPixelSize::FromIndex(current_index);
        changes_made = true;
    }
}

