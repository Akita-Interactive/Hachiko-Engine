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

        ComponentCanvas* FindClosestParentCanvas() const;
        void Render(Program* img_program, Program* txt_program) const;
    };
} // namespace Hachiko
