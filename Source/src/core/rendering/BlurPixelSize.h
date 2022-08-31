#pragma once

namespace Hachiko::BlurPixelSize
{
    enum class Type
    {
        Gaussian5x5 = 5,
        Gaussian7x7 = 7,
        Gaussian9x9 = 9,
        Gaussian11x11 = 11,
    };

    constexpr const int number_of_blur_pixel_sizes = 4; 
    constexpr const char* blur_pixel_sizes_strings[number_of_blur_pixel_sizes] = 
    {
        "5x5", 
        "7x7", 
        "9x9", 
        "11x11"
    };

    bool IsValidPixelSize(int size);

    const char* ToString(Type size);

    Type FromString(const std::string& size);

    int ToIndex(Type size);

    Type FromIndex(int index);
}