#include "core/hepch.h"
#include "ComponentCanvasRenderer.h"
#include "ComponentImage.h"
#include "ComponentButton.h"
#include "ComponentCanvas.h"
#include "ComponentTransform2D.h"
#include "ComponentText.h"

#include "core/rendering/Program.h"

Hachiko::ComponentCanvasRenderer::ComponentCanvasRenderer(GameObject* container) : Component(Type::CANVAS_RENDERER, container) {}

void Hachiko::ComponentCanvasRenderer::DrawGui()
{
    if (ImGuiUtils::CollapsingHeader(this, "Canvas renderer"))
    {
        Widgets::Label("Active", StringUtils::ToString(active));
    }
}

void Hachiko::ComponentCanvasRenderer::Save(YAML::Node& node) const
{
    node.SetTag("canvas_renderer");
}

Hachiko::ComponentCanvas* Hachiko::ComponentCanvasRenderer::FindClosestParentCanvas() const
{
    ComponentCanvas* canvas = nullptr;
    GameObject* parent = game_object->parent;
    while (parent)
    {
        canvas = parent->GetComponent<ComponentCanvas>();
        if (canvas)
        {
            break;
        }
        parent = parent->parent;
    }
    return canvas;
}

void Hachiko::ComponentCanvasRenderer::Render(Program* img_program, Program* txt_program) const
{
    ComponentCanvas* canvas = FindClosestParentCanvas();
    ComponentTransform2D* transform = game_object->GetComponent<ComponentTransform2D>();

    if (canvas && transform)
    {
        // TODO: Make draw calls for all ui components of current game object
        ComponentImage* image = game_object->GetComponent<ComponentImage>();
        if (image && image->IsActive())
        {
            image->Draw(transform, img_program);
        }

        ComponentText* text = game_object->GetComponent<ComponentText>();
        if (text && text->IsActive())
        {
            text->Draw(transform, txt_program);
        }
    }
}
