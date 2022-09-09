#include "core/hepch.h"
#include "ComponentPointLight.h"
#include "ComponentTransform.h"
#include "modules/ModuleEvent.h"

#include <debugdraw.h>

Hachiko::ComponentPointLight::ComponentPointLight(GameObject* container) :
    Component(Type::POINTLIGHT, container)
{
    if (game_object->scene_owner)
    {
        game_object->scene_owner->point_lights.push_back(this);
    }
}

Hachiko::ComponentPointLight::~ComponentPointLight()
{
    if (game_object->scene_owner)
    {
        auto& lights = game_object->scene_owner->point_lights;
        lights.erase(std::remove(lights.begin(), lights.end(), this), lights.end());
    }
}

void Hachiko::ComponentPointLight::DebugDraw()
{
    if (draw_sphere)
    {
        auto* transform = game_object->GetTransform();
        if (transform)
        {
            dd::sphere(transform->GetGlobalPosition(), dd::colors::Blue, radius);
        }
    }
}

float3 Hachiko::ComponentPointLight::GetPosition() const
{
    ComponentTransform* transform = game_object->GetTransform();
    return transform->GetGlobalPosition();
}

void Hachiko::ComponentPointLight::Save(YAML::Node& node) const
{
    node[LIGHT_TYPE] = static_cast<int>(Type::POINTLIGHT);
    node[LIGHT_COLOR] = color;
    node[LIGHT_INTENSITY] = intensity;
    node[LIGHT_RADIUS] = radius;
    node[LIGHT_DRAW_SPHERE] = draw_sphere;
}

void Hachiko::ComponentPointLight::Load(const YAML::Node& node)
{
    color = node[LIGHT_COLOR].as<float4>();
    intensity = node[LIGHT_INTENSITY].as<float>();
    radius = node[LIGHT_RADIUS].as<float>();
    draw_sphere = node[LIGHT_DRAW_SPHERE].as<bool>();
}

void Hachiko::ComponentPointLight::DrawGui()
{
    ImGui::PushID(this);
    if (ImGuiUtils::CollapsingHeader(this, "Point Light"))
    {
        if (Widgets::Checkbox("Active", &active))
        {
            App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
        }
        if (Widgets::Checkbox("Draw sphere", &draw_sphere))
        {
            App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
        }

        Widgets::DragFloat("Intensity", intensity);
        Widgets::DragFloat("Radius", radius);
        ImGuiUtils::CompactColorPicker("Color", &color[0]);
        CREATE_HISTORY_ENTRY_AFTER_EDIT()
    }
    ImGui::PopID();
}
