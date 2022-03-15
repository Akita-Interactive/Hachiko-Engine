#include "core/hepch.h"
#include "ComponentPointLight.h"
#include "ComponentTransform.h"

#include <debugdraw.h>

Hachiko::ComponentPointLight::ComponentPointLight(GameObject* conatiner) :
    Component(Type::POINTLIGHT, conatiner)
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
            dd::sphere(transform->GetPosition(), dd::colors::Blue, radius);
        }
    }
}

float3 Hachiko::ComponentPointLight::GetPosition() const
{
    const ComponentTransform* transform = game_object->GetTransform();
    return transform->GetPosition();
}

void Hachiko::ComponentPointLight::Save(YAML::Node& node) const
{
    node[LIGHT_TYPE] = (int)Type::POINTLIGHT;
    node[LIGHT_COLOR] = color;
    node[LIGHT_INTENSITY] = intensity;
    node[LIGHT_RADIUS] = radius;
    node[COMPONENT_ENABLED] = active;
    node[LIGHT_DRAW_SPHERE] = draw_sphere;
}

void Hachiko::ComponentPointLight::Load(const YAML::Node& node)
{
    color = node[LIGHT_COLOR].as<float4>();
    intensity = node[LIGHT_INTENSITY].as<float>();
    radius = node[LIGHT_RADIUS].as<float>();
    active = node[COMPONENT_ENABLED].as<bool>();
    draw_sphere = node[LIGHT_DRAW_SPHERE].as<bool>();
}

void Hachiko::ComponentPointLight::DrawGui()
{
    ImGui::PushID(this);
    if (ImGuiUtils::CollapsingHeader(game_object, this, "Point Light"))
    {
        ImGui::Checkbox("P.Active", &active);
        ImGui::Checkbox("Draw Sphere", &draw_sphere);
        ImGui::PushItemWidth(100.0f);
        ImGui::InputFloat("P.Intensity", &intensity);
        ImGui::InputFloat("P.Radius", &radius);
        ImGui::PopItemWidth();
        ImGuiUtils::CompactColorPicker("Point Color", &color[0]);
    }
    ImGui::PopID();
}
