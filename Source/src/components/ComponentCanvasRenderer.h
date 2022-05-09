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

        static Type GetType();

        void DrawGui() override;


        ComponentCanvas* FindClosestParentCanvas() const;
        void Render(Program* img_program, Program* txt_program) const;
    };
} // namespace Hachiko

inline Hachiko::Component::Type Hachiko::ComponentCanvasRenderer::GetType()
{
    return Type::CANVAS_RENDERER;
}
