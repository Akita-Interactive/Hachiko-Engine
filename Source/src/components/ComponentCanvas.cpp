#include "core/hepch.h"
#include "ComponentCanvas.h"
#include "ComponentTransform2D.h"
#include "ComponentCamera.h"
#include "ComponentCanvasRenderer.h"

#include "modules/ModuleCamera.h"

#include "debugdraw.h"

Hachiko::ComponentCanvas::ComponentCanvas(GameObject* container) :
    Component(Type::CANVAS, container)
{
}

Hachiko::ComponentCanvas::~ComponentCanvas()
{
}

void Hachiko::ComponentCanvas::Update()
{
    UpdateSize();
}

void Hachiko::ComponentCanvas::DrawGui()
{
    ImGui::PushID(this);
    if (ImGuiUtils::CollapsingHeader(this, "Canvas"))
    {
        Widgets::Label("Screen size ", StringUtils::Format("%d, %d", size_x, size_y));
    }

    ImGui::PopID();
}

void Hachiko::ComponentCanvas::DebugDraw()
{
}

void Hachiko::ComponentCanvas::Save(YAML::Node& node) const
{
    node.SetTag("canvas");
}

void Hachiko::ComponentCanvas::UpdateSize(bool force)
{
    unsigned width, height;
    App->camera->GetRenderingCamera()->GetResolution(width, height);
    if (force || width != size_x || height != size_y)
    {
        ComponentTransform2D* transform = game_object->GetComponent<ComponentTransform2D>();
        if (transform)
        {
            transform->SetSize(float2(width, height));
            size_x = width;
            size_y = height;
        }
    }
}

bool Hachiko::ComponentCanvas::HasDependentComponents(GameObject* game_object) const
{
    bool found = game_object->GetComponent<ComponentCanvasRenderer>();

    for (GameObject* child : game_object->children)
    {
        found = HasDependentComponents(child);
        if (found)
        {
            break;
        }
    }
    return found;
}
