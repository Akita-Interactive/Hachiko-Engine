#include "core/hepch.h"
#include "ComponentSpotLight.h"
#include "ComponentTransform.h"

#include <debugdraw.h>

Hachiko::ComponentSpotLight::ComponentSpotLight(GameObject* conatiner) :
    Component(Type::SPOTLIGHT, conatiner)
{
    if (game_object->scene_owner)
    {
        game_object->scene_owner->spot_lights.push_back(this);
    }
}

Hachiko::ComponentSpotLight::~ComponentSpotLight()
{
    if (game_object->scene_owner)
    {
        auto& lights = game_object->scene_owner->spot_lights;
        lights.erase(std::remove(lights.begin(), lights.end(), this), lights.end());
    }
}

void Hachiko::ComponentSpotLight::DebugDraw()
{
    if (draw_cone)
    {
        auto* transform = game_object->GetTransform();
        if (transform)
        {
            dd::cone(transform->GetGlobalPosition(), transform->GetFront().Mul(radius), dd::colors::Blue, inner, 0.f);
            dd::cone(transform->GetGlobalPosition(), transform->GetFront().Mul(radius), dd::colors::Green, outer, 0.f);
        }
    }
}

float3 Hachiko::ComponentSpotLight::GetPosition() const
{
    ComponentTransform* transform = game_object->GetTransform();
    return transform->GetGlobalPosition();
}

float3 Hachiko::ComponentSpotLight::GetDirection() const
{
    ComponentTransform* transform = game_object->GetTransform();
    return transform->GetFront();
}

void Hachiko::ComponentSpotLight::Save(YAML::Node& node) const
{
    node[LIGHT_TYPE] = static_cast<int>(Type::SPOTLIGHT);
    node[LIGHT_COLOR] = color;
    node[LIGHT_INNER] = inner;
    node[LIGHT_OUTER] = outer;
    node[LIGHT_INTENSITY] = intensity;
    node[LIGHT_RADIUS] = radius;
    node[LIGHT_DRAW_CONE] = draw_cone;
}

void Hachiko::ComponentSpotLight::Load(const YAML::Node& node)
{
    color = node[LIGHT_COLOR].as<float4>();
    inner = node[LIGHT_INNER].as<float>();
    outer = node[LIGHT_OUTER].as<float>();
    intensity = node[LIGHT_INTENSITY].as<float>();
    radius = node[LIGHT_RADIUS].as<float>();
    draw_cone = node[LIGHT_DRAW_CONE].as<bool>();
}

void Hachiko::ComponentSpotLight::DrawGui()
{
    ImGui::PushID(this);
    if (ImGuiUtils::CollapsingHeader(game_object, this, "Spot Light"))
    {
        ImGui::PushItemWidth(100.0f);
        ImGui::Checkbox("S.Active", &active);
        ImGui::Checkbox("Draw Cone", &draw_cone);
        ImGui::InputFloat("S.Intensity", &intensity);
        ImGui::InputFloat("S.Radius", &radius);
        ImGui::InputFloat("Inner Angle", &inner);
        ImGui::InputFloat("Outer Angle", &outer);
        ImGui::PopItemWidth();
        ImGuiUtils::CompactColorPicker("Spot Color", &color[0]);
    }
    ImGui::PopID();
}
