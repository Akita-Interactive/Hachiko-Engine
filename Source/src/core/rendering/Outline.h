#pragma once

namespace Hachiko::Outline
{
    enum class Type
    {
        PRIMARY = 0,
        SECONDARY = 1,
        // NOTE: Put new outline types here, NONE is also used as Count.
        NONE,
    };

    struct Config
    {
        float thickness;
        float4 color;
    };

    constexpr Type IntToType(int type)
    {
        return static_cast<Type>(type);
    }

    constexpr int TypeToInt(Type type)
    {
        return static_cast<int>(type);
    }

    constexpr size_t GetEnabledTypeCount()
    {
        return TypeToInt(Type::NONE);
    }

    constexpr size_t GetAllTypeCount()
    {
        return TypeToInt(Type::NONE) + 1;
    }

    constexpr const char* TypeToString(Type type)
    {
        switch (type)
        {
            case Type::PRIMARY:
                return "Primary";
            case Type::SECONDARY:
                return "Secondary";
            case Type::NONE:
                return "None";
        }
    }

    constexpr const char* const all_names[TypeToInt(Type::NONE) + 1]
    {
        TypeToString(Type::PRIMARY),
        TypeToString(Type::SECONDARY),
        TypeToString(Type::NONE)
    };
}
