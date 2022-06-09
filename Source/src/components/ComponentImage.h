#pragma once

#include "components/Component.h"
#include "Globals.h"

namespace Hachiko
{
    class GameObject;

    class ComponentTransform2D;
    class Program;
    class ResourceTexture;

    class ComponentImage : public Component
    {
    public:
        ComponentImage(GameObject* container);
        ~ComponentImage() override = default;

        void DrawGui() override;
        void Draw(ComponentTransform2D* transform, Program* program) const;
        void Update() override;

        void Save(YAML::Node& node) const override;
        void Load(const YAML::Node& node) override;

        [[nodiscard]] const float4& GetColor() const 
        {
            return color;
        }
        
        [[nodiscard]] const float4& GetHoverColor() const 
        {
            return hover_color;
        }

        void SetColor(float4 new_color) 
        {
            color = new_color;
        }

        void SetHoverColor(float4 new_hover_color) 
        {
            hover_color = new_hover_color;
        }

    private:
        void LoadImageResource(UID image_uid, bool is_hover = false);

        void UpdateSize();

        ResourceTexture* image = nullptr;
        ResourceTexture* hover_image = nullptr;

        float4 color = float4::one;
        float4 hover_color = float4::one;
        bool use_image = false;
        bool use_hover_image = false;
        bool fill_window = false;

        // TODO: Make properly after bs
        unsigned size_x = 0;
        unsigned size_y = 0;
    };
} // namespace Hachiko
