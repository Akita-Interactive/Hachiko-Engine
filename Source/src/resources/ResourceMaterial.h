#pragma once
#include "Resource.h"

#include "resources/ResourceTexture.h"

namespace Hachiko
{
    class ResourceMaterial : public Resource
    {
    public:
        ResourceMaterial(UID uid);
        ~ResourceMaterial() override;

        void DrawGui();

        [[nodiscard]] bool HasDiffuse() const
        {
            return diffuse != nullptr;
        }

        [[nodiscard]] bool HasSpecular() const
        {
            return specular != nullptr;
        }

        [[nodiscard]] bool HasNormal() const
        {
            return normal != nullptr;
        }

        [[nodiscard]] bool HasMetalness() const
        {
            return metalness != nullptr;
        }

        [[nodiscard]] bool HasEmissive() const
        {
            return emissive != nullptr;
        }

        [[nodiscard]] unsigned GetDiffuseId() const
        {
            if (diffuse != nullptr)
            {
                return diffuse->GetImageId();
            }
            return 0;
        }

        [[nodiscard]] unsigned GetSpecularId() const
        {
            if (specular != nullptr)
            {
                return specular->GetImageId();
            }
            return 0;
        }

        [[nodiscard]] unsigned GetNomalId() const
        {
            if (normal != nullptr)
            {
                return normal->GetImageId();
            }
            return 0;
        }

        [[nodiscard]] unsigned GetMetalnessId() const
        {
            if (metalness != nullptr)
            {
                return metalness->GetImageId();
            }
            return 0;
        }

        [[nodiscard]] unsigned GetEmissiveId() const
        {
            if (emissive != nullptr)
            {
                return emissive->GetImageId();
            }
            return 0;
        }

        [[nodiscard]] std::string GetName() const
        {
            return name;
        }

        void SetName(const std::string& new_name)
        {
            name = new_name;
        }

        void SetTexture(ResourceTexture* res, ResourceTexture::Type type);

        std::string TypeToString(ResourceTexture::Type type);

        ResourceTexture* diffuse = nullptr;
        ResourceTexture* specular = nullptr;
        ResourceTexture* normal = nullptr;
        ResourceTexture* metalness = nullptr;
        ResourceTexture* emissive = nullptr;
        float4 diffuse_color = float4::one;
        float4 specular_color = float4::zero;
        float4 emissive_color = float4::zero;
        float smoothness = 0.5f;
        float metalness_value = 0.5f;
        unsigned is_metallic = 0;
        unsigned smoothness_alpha = 0;
        unsigned is_transparent = 0;

    private:
        void AddTexture(ResourceTexture::Type type);
        void RemoveTexture(ResourceTexture::Type type);
        void UpdateMaterial();
        std::string name;

        std::vector<std::string> transparency = {"Opaque", "Transparent"};
        std::vector<std::string> material_types = {"Specular", "Metallic"};
        std::vector<std::string> alpha_channels = {"Diffuse", ""}; // The empty channel will have the selected name on materialTypes
    };
}
