#pragma once

#include "Component.h"
#include "Globals.h"

#include "resources/Resource.h"

class FTLabel;

namespace Hachiko
{
    class GameObject;
    class ComponentTransform2D;
    class Program;
    class ResourceFont;

    class ComponentText : public Component
    {
        friend class ComponentTransform2D;
    public:
        ComponentText(GameObject* container);
        ~ComponentText() override;

        void DrawGui() override;
        void Draw(ComponentTransform2D* transform, Program* program);

        void Save(YAML::Node& node) const override;
        void Load(const YAML::Node& node) override;
        static void CollectResources(const YAML::Node& node, std::map<Resource::Type, std::set<UID>>& resources);

        HACHIKO_API void SetText(const char* new_text);
        HACHIKO_API void SetFontSize(int new_size);
        HACHIKO_API void SetFontColor(const float4& new_color);

    private:
        void Invalidate()
        {
            dirty = true;
        }
        void LoadFont(UID id);
        void RefreshLabel(ComponentTransform2D* transform);
        void BuildLabel(ComponentTransform2D* transform);

        bool build_font = true;
        bool dirty = true;

        ResourceFont* font = nullptr;
        std::unique_ptr<FTLabel> label = nullptr;

        std::string label_text = "Sample Text";

        float4 font_color = float4::one;
        int font_size = 28;

        char font_filename_buffer[MAX_PATH] = "Image Filename\0";
    };
} // namespace Hachiko
