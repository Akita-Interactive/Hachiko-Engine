#pragma once

#include "components/Component.h"

namespace Hachiko
{
    class ResourceMaterial;

    class ComponentMaterial : public Component
    {
    public:
        ComponentMaterial(GameObject* container);
        ~ComponentMaterial() override = default;

        static Type GetType()
        {
            return Type::MATERIAL;
        }

        [[nodiscard]] const ResourceMaterial* GetMaterial() const
        {
            return material;
        }

        void SetResourceMaterial(ResourceMaterial* res)
        {
            delete material;
            material = res;
        }

        [[nodiscard]] const std::string& GetModelName() const
        {
            return model_name;
        }

        void SetModelName(const std::string& name)
        {
            model_name = name;
        }

        void DrawGui() override;
        void LoadMaterial(const std::string& material_path);

        void Save(YAML::Node& node) const override;
        void Load(const YAML::Node& node) override;
        
    private:
        ResourceMaterial* material{};
        std::string model_name;
    };
}
