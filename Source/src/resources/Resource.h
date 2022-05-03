#pragma once

#include "utils/UUID.h"

namespace Hachiko
{
#define CLONE_RESOURCE(type)            \
    Resource* Clone() const override    \
    {                                   \
        return new type(*this);         \
    }

    class Resource
    {
    public:
        
        enum class Type
        {
            UNKNOWN = 0,
            MATERIAL = 1,
            TEXTURE = 2,
            MESH = 3,
            SCENE = 4,
            SHADER = 5,
            AUDIO = 6,
            ANIMATION = 7,
            SCRIPT = 8,
            VIDEO = 9,
            SKYBOX = 10,
            FONT = 11,
            NAVMESH = 12,
            PREFAB = 13,
            COUNT,
        };

        enum class AssetType
        {
            UNKNOWN = 0,
            MATERIAL = 1,
            TEXTURE = 2,
            MODEL = 3,
            SCENE = 4,
            SHADER = 5,
            AUDIO = 6,
            VIDEO = 7,
            SKYBOX = 8,
            FONT = 9,
            PREFAB = 10,
            COUNT,
        };

        static std::string ResourceTypeString(const Type& t) {
            static const std::map<Type, std::string> string_map = {
                {Type::MATERIAL, "materials"},
                {Type::TEXTURE, "textures"},
                {Type::MESH, "meshes"},
                {Type::SCENE, "scene"},
                {Type::SHADER, "shaders"},
                {Type::AUDIO, "audio"},
                {Type::ANIMATION, "animations"},
                {Type::SCRIPT, "scripts"},
                {Type::VIDEO, "video"},
                {Type::SKYBOX, "skybox"},
                {Type::FONT, "fonts"},
                {Type::NAVMESH, "navmesh"},
                {Type::PREFAB, "prefab"},
            };

            auto it = string_map.find(t);
            if (it != string_map.end())
            {
                return it->second;
            }
            return "";
        }

        static std::string AssetTypeString(const AssetType& t)
        {
            static const std::map<AssetType, std::string> string_map = {
                {AssetType::MATERIAL, "materials"},
                {AssetType::MATERIAL, "textures"},
                {AssetType::MODEL, "models"},
                {AssetType::SCENE, "scene"},
                {AssetType::SHADER, "shaders"},
                {AssetType::AUDIO, "audio"},
                {AssetType::SKYBOX, "skybox"},
                {AssetType::FONT, "fonts"},
                {AssetType::PREFAB, "prefab"},
            };

            auto it = string_map.find(t);
            if (it != string_map.end())
            {
                return it->second;
            }
            return "";
        }

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

    public:
        virtual Resource* Clone() const = 0;

        Resource(const Resource& other) :
            instances(other.instances),
            id(other.id),
            type(other.type)
        {
        }

        Resource& operator=(const Resource& other)
        {
            if (this == &other)
            {
                return *this;
            }
            instances = other.instances;
            id = other.id;
            type = other.type;
            return *this;
        }
    };
}
