#pragma once
#include <string>

namespace Hachiko::Editor
{
    class Theme final
    {
    public:
        enum class Type
        {
            DARK = 0,
            LIGHT,
            PINK,
            CLASSIC,
        };

        static const char* ToString(Type theme)
        {
            switch (theme)
            {
            case Type::LIGHT:
                return "light";
            case Type::PINK:
                return "pink";
            case Type::DARK: 
                return "dark";
            default:
            case Type::CLASSIC:
                return "classic";
            }
        }

        static Type FromString(const std::string& theme)
        {
            if (theme == "light")
            {
                return Type::LIGHT;
            }
            if (theme == "pink")
            {
                return Type::PINK;
            }
            if (theme == "dark")
            {
                return Type::DARK;
            }

            return Type::CLASSIC;
        }

        Theme() = delete;
    };
}
