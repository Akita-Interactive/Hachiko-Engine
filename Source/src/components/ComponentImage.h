#pragma once

#include "components/Component.h"
#include "Globals.h"

#include "modules/ModuleTexture.h"

namespace Hachiko
{
    class GameObject;

    class ComponentTransform2D;
    class Program;

    class ComponentImage : public Component
    {
    public:
        ComponentImage(GameObject* container);
        ~ComponentImage() override = default;

        void DrawGui() override;
        void Draw(ComponentTransform2D* transform, Program* program) const;

        void Save(YAML::Node& node) const override;
        void Load(const YAML::Node& node) override;

        void Import(const char* path);


        Texture image;
        Texture hover_image;

        char image_filename_buffer[MAX_PATH] = "Image Filename\0";
        char hover_image_filename_buffer[MAX_PATH] = "Hover Image Filename\0";

        float4 color = float4::one;
        float4 hover_color = float4::one;
        bool use_image = false;
        bool use_hover_image = false;

    public:
        CLONE_COMPONENT(ComponentImage)

        ComponentImage(const ComponentImage& other) = default;
        ComponentImage& operator=(const ComponentImage& other) = default;
    };
} // namespace Hachiko
