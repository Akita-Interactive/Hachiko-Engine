#include "core/hepch.h"

#include "ComponentDirLight.h"
#include "ComponentTransform.h"

#include <debugdraw.h>

Hachiko::ComponentDirLight::ComponentDirLight(GameObject* conatiner) :
    Component(Type::DIRLIGHT, conatiner)
{
    if (game_object->scene_owner)
    {
        game_object->scene_owner->dir_lights.push_back(this);
    }
}

Hachiko::ComponentDirLight::~ComponentDirLight()
{
    if (game_object->scene_owner)
    {
        auto& lights = game_object->scene_owner->dir_lights;
        lights.erase(std::remove(lights.begin(), lights.end(), this), lights.end());
    }
}

void Hachiko::ComponentDirLight::DebugDraw()
{
    if (draw_direction)
    {
        auto* transform = game_object->GetTransform();
        if (transform)
        {
            dd::arrow(transform->GetGlobalPosition(), transform->GetFront().Mul(3.f) + transform->GetGlobalPosition(), dd::colors::Blue, 1.f);
        }
    }
}

float3 Hachiko::ComponentDirLight::GetDirection() const
{
    ComponentTransform* transform = game_object->GetTransform();
    return transform->GetFront();
}

void Hachiko::ComponentDirLight::Save(YAML::Node& node) const
{
    node[LIGHT_TYPE] = static_cast<int>(Type::DIRLIGHT);
    node[LIGHT_COLOR] = color;
    node[LIGHT_INTENSITY] = intensity;
    node[LIGHT_DRAW_DIRECTION] = draw_direction;
}

void Hachiko::ComponentDirLight::Load(const YAML::Node& node)
{
    color = node[LIGHT_COLOR].as<float4>();
    intensity = node[LIGHT_INTENSITY].as<float>();
    draw_direction = node[LIGHT_DRAW_DIRECTION].as<bool>();
}

void Hachiko::ComponentDirLight::DrawGui()
{
    ImGui::PushID(this);
    if (ImGuiUtils::CollapsingHeader(game_object, this, "Dir Light"))
    {
        ImGui::Checkbox("D.Active", &active);
        ImGui::Checkbox("Draw Direction", &draw_direction);
        ImGui::PushItemWidth(100.0f);
        ImGui::InputFloat("D.Intensity", &intensity);
        ImGui::PopItemWidth();
        ImGuiUtils::CompactColorPicker("Dir Color", &color[0]);
    }
    ImGui::PopID();
}
