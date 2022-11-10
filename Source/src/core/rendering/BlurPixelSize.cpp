#include "core/hepch.h"
#include "BlurPixelSize.h"


bool Hachiko::BlurPixelSize::IsValidPixelSize(int size)
{
    return size == 5 || size == 7 || size == 9 || size == 11;
}

const char* Hachiko::BlurPixelSize::ToString(Type size)
{
    switch (size)
    {
        case Hachiko::BlurPixelSize::Type::Gaussian5x5:
            return blur_pixel_sizes_strings[0];
        case Hachiko::BlurPixelSize::Type::Gaussian7x7:
            return blur_pixel_sizes_strings[1];
        case Hachiko::BlurPixelSize::Type::Gaussian9x9:
            return blur_pixel_sizes_strings[2];
        case Hachiko::BlurPixelSize::Type::Gaussian11x11:
            return blur_pixel_sizes_strings[3];
        default:
            return "invalid";
    }
}

Hachiko::BlurPixelSize::Type Hachiko::BlurPixelSize::FromString(const std::string& size)
{
    if (size == blur_pixel_sizes_strings[1])
    {
        return Hachiko::BlurPixelSize::Type::Gaussian7x7;
    }
    else if (size == blur_pixel_sizes_strings[2])
    {
        return Hachiko::BlurPixelSize::Type::Gaussian9x9;
    }
    else if (size == blur_pixel_sizes_strings[3])
    {
        return Hachiko::BlurPixelSize::Type::Gaussian11x11;
    }
    else
    {
        return Hachiko::BlurPixelSize::Type::Gaussian5x5;
    }
}

int Hachiko::BlurPixelSize::ToIndex(Hachiko::BlurPixelSize::Type size)
{
    switch (size)
    {
        default:
        case Hachiko::BlurPixelSize::Type::Gaussian5x5:
            return 0;
        case Hachiko::BlurPixelSize::Type::Gaussian7x7:
            return 1;
        case Hachiko::BlurPixelSize::Type::Gaussian9x9:
            return 2;
        case Hachiko::BlurPixelSize::Type::Gaussian11x11:
            return 3;
    }
}

Hachiko::BlurPixelSize::Type Hachiko::BlurPixelSize::FromIndex(int index)
{
    switch (index)
    {
        default:
        case 0:
            return Hachiko::BlurPixelSize::Type::Gaussian5x5;
        case 1:
            return Hachiko::BlurPixelSize::Type::Gaussian7x7;
        case 2:
            return Hachiko::BlurPixelSize::Type::Gaussian9x9;
        case 3:
            return Hachiko::BlurPixelSize::Type::Gaussian11x11;
    }
}

int Hachiko::BlurPixelSize::ToInt(Hachiko::BlurPixelSize::Type size)
{
    return static_cast<int>(size);
}
