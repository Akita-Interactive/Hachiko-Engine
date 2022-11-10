#pragma once

#include "components/Component.h"
#include "Globals.h"

namespace Hachiko
{
    class GameObject;

    class ComponentCanvas;
    class Program;

    class ComponentCanvasRenderer : public Component
    {
    public:
        ComponentCanvasRenderer(GameObject* container);
        ~ComponentCanvasRenderer() override = default;

        void DrawGui() override;
        void Save(YAML::Node& node) const override;
        void Load(const YAML::Node& node) override;

        ComponentCanvas* FindClosestParentCanvas() const;
        void Render(ComponentCanvas* canvas, Program* img_program, Program* txt_program) const;

    private:
        bool scale_ui = false;
        unsigned original_size_x = 0;
        unsigned original_size_y = 0;
    };
} // namespace Hachiko
