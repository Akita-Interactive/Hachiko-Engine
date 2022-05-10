#pragma once

#include "Component.h"
#include "Globals.h"

#include "modules/ModuleTexture.h"
#include "FTLabel.h"

namespace Hachiko
{
    class GameObject;

    class ComponentTransform2D;
    class ResourceFont;
    class Program;

    class ComponentText : public Component
    {
    public:
        ComponentText(GameObject* container);
        ~ComponentText() override = default;

        void DrawGui() override;
        void Draw(ComponentTransform2D* transform, Program* program);

        void Save(YAML::Node& node) const override;
        void Load(const YAML::Node& node) override;
        

        void Invalidate()
        {
            dirty = true;
        }

    private:
        void LoadFont(UID id);
        void RefreshLabel(ComponentTransform2D* transform);
        void BuildLabel(ComponentTransform2D* transform);
        
        bool dirty = true;        

        ResourceFont* font = nullptr;
        std::unique_ptr<FTLabel> label = nullptr;

        std::string label_text = "Sample Text";


        float4 font_color = float4::one;
        float font_size = 28.f;

        char font_filename_buffer[MAX_PATH] = "Image Filename\0";
    };
} // namespace Hachiko
