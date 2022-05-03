#pragma once
#include "Resource.h"

#include "modules/ModuleTexture.h"

namespace Hachiko
{
    class ResourceTexture : public Resource
    {
    public:
        ResourceTexture();
        ResourceTexture(UID id);
        ~ResourceTexture() override;

        enum class Type
        {
            DIFFUSE = 0,
            SPECULAR,
            NORMALS,
            METALNESS,
            EMISSIVE
        };

        void GenerateBuffer();

        void DrawGui();

        [[nodiscard]] unsigned GetId() const
        {
            return id;
        }

        [[nodiscard]] std::string GetAssetPath() const
        {
            return path;
        }

        void SetAssetPath(const std::string& new_path)
        {
            path = new_path;
        }

        [[nodiscard]] std::string GetName() const
        {
            return name;
        }

        void SetName(const std::string& new_name)
        {
            name = new_name;
        }

        unsigned id = 0;
        unsigned width = 0;
        unsigned height = 0;
        unsigned format = 0;
        unsigned bpp = 0;

        // TODO: Config will be lost in batching
        unsigned min_filter = 0x2703;
        unsigned mag_filter = 0x2601;
        unsigned wrap = 0x2900;

        ::byte* data{};
        unsigned data_size = 0;

        std::string path;
        std::string name = std::string();

    public:
        CLONE_RESOURCE(ResourceTexture)

        //TODO check if byte* data is being copied
        ResourceTexture(const ResourceTexture& other) = default;

        ResourceTexture& operator=(const ResourceTexture& other)
        {
            if (this == &other)
            {
                return *this;
            }
            Resource::operator =(other);
            id = other.id;
            width = other.width;
            height = other.height;
            format = other.format;
            bpp = other.bpp;
            min_filter = other.min_filter;
            mag_filter = other.mag_filter;
            wrap = other.wrap;
            data = other.data;
            data_size = other.data_size;
            path = other.path;
            name = other.name;
            return *this;
        }
    };
} // namespace Hachiko
