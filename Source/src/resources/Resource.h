#pragma once

#include "utils/UUID.h"

namespace Hachiko
{
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
            // SHADER = 5,
            // AUDIO = 6,
            ANIMATION = 7,
            SCRIPT = 8,
            VIDEO = 9,
            SKYBOX = 10,
            FONT = 11,
            NAVMESH = 12,
            PREFAB = 13,
            STATE_MACHINE = 14,
            COUNT
        };

        enum class AssetType
        {
            UNKNOWN = 0,
            MATERIAL = 1,
            TEXTURE = 2,
            MODEL = 3,
            SCENE = 4,
            // SHADER = 5,
            // AUDIO = 6,
            VIDEO = 7,
            SKYBOX = 8,
            FONT = 9,
            PREFAB = 10,
            STATE_MACHINE = 11,
        };

        static std::string ResourceTypeString(const Type& t) {
            static const std::map<Type, std::string> string_map = {
                {Type::MATERIAL, "materials"},
                {Type::TEXTURE, "textures"},
                {Type::MESH, "meshes"},
                {Type::SCENE, "scene"},
                {Type::ANIMATION, "animations"},
                {Type::SCRIPT, "scripts"},
                {Type::VIDEO, "video"},
                {Type::SKYBOX, "skybox"},
                {Type::FONT, "fonts"},
                {Type::NAVMESH, "navmesh"},
                {Type::PREFAB, "prefab"},
                {Type::STATE_MACHINE, "state_machine"},
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
                {AssetType::SKYBOX, "skybox"},
                {AssetType::FONT, "fonts"},
                {AssetType::PREFAB, "prefab"},
                {AssetType::STATE_MACHINE, "state_machine"},
                {AssetType::VIDEO, "video"},
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
    };
}
