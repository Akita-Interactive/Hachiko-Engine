#pragma once

#include "utils/UUID.h"

namespace Hachiko
{
    class Resource
    {
    public:
        enum class Type
        {
            UNKNOWN,
            MATERIAL,
            TEXTURE,
            MODEL,
            MESH,
            SCENE,
            SHADER,
            AUDIO,
            ANIMATION,
            SCRIPT,
            VIDEO,
            SKYBOX,
            COUNT
        };

        Resource(Type type);
        Resource(UID id, Type type);
        virtual ~Resource();

        [[nodiscard]] Type GetType() const
        {
            return type;
        }

        [[nodiscard]] UID GetID() const
        {
            return id;
        }

        unsigned int instances = 0;

    private:
        UID id = 0;
        Type type = Type::UNKNOWN;
    };
}
