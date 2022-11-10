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

        if (ImGui::Checkbox("Scale screen", &scale_ui))
        {
            ComponentCanvas* canvas = FindClosestParentCanvas();
            canvas->GetSize(original_size_x, original_size_y);
        }
    }
}

void Hachiko::ComponentCanvasRenderer::Save(YAML::Node& node) const
{
    node.SetTag("canvas_renderer");
    node[SCALE_UI] = scale_ui;
    node[ORIGINAL_SIZE_X] = original_size_x;
    node[ORIGINAL_SIZE_Y] = original_size_y;
}

void Hachiko::ComponentCanvasRenderer::Load(const YAML::Node& node) 
{
    if (node[SCALE_UI].IsDefined())
    {
        scale_ui = node[SCALE_UI].as<bool>();
    }
    if (node[ORIGINAL_SIZE_X].IsDefined())
    {
        original_size_x = node[ORIGINAL_SIZE_X].as<unsigned>();
    }
    if (node[ORIGINAL_SIZE_Y].IsDefined())
    {
        original_size_y = node[ORIGINAL_SIZE_Y].as<unsigned>();
    }
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

void Hachiko::ComponentCanvasRenderer::Render(ComponentCanvas* canvas, Program* img_program, Program* txt_program) const
{
    ComponentTransform2D* transform = game_object->GetComponent<ComponentTransform2D>();

    float3 original_position = transform->GetPosition();
    float2 original_size = transform->GetSize();
    if (scale_ui)
    {
        unsigned canvas_size_x, canvas_size_y;
        canvas->GetSize(canvas_size_x, canvas_size_y);

        float factor_x = static_cast<float>(canvas_size_x) / original_size_x;
        float factor_y = static_cast<float>(canvas_size_y) / original_size_y;

        transform->SetPosition(float3(original_position.x * factor_x, original_position.y * factor_y, original_position.z)); // TODO: careful with the invalid thing
        transform->SetSize(float2(original_size.x * factor_x, original_size.y * factor_y)); // TODO: careful with the invalid thing
    }

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

    if (scale_ui)
    {
        transform->SetPosition(original_position);
        transform->SetSize(original_size);
    }
}
