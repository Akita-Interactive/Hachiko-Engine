#include "core/hepch.h"

#include "ComponentDirLight.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"
#include "modules/ModuleEvent.h"

#include "core/rendering/Uniforms.h"

#include <debugdraw.h>

Hachiko::ComponentDirLight::ComponentDirLight(GameObject* conatiner) 
    : Component(Type::DIRLIGHT, conatiner)
    , shadow_properties(ShadowMappingProperties())
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

    shadow_properties.Save(node);
}

void Hachiko::ComponentDirLight::Load(const YAML::Node& node)
{
    color = node[LIGHT_COLOR].as<float4>();
    intensity = node[LIGHT_INTENSITY].as<float>();
    draw_direction = node[LIGHT_DRAW_DIRECTION].as<bool>();

    shadow_properties.Load(node);
}

void Hachiko::ComponentDirLight::DrawGui()
{
    ImGui::PushID(this);
    if (ImGuiUtils::CollapsingHeader(game_object, this, "Directional Light"))
    {
        if (ImGui::Checkbox("Active", &active))
        {
            App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
        }
        if (ImGui::Checkbox("Draw Direction", &draw_direction))
        {
            App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
        }

        ImGui::PushItemWidth(100.0f);
        ImGui::InputFloat("Intensity", &intensity);
        CREATE_HISTORY_ENTRY_AFTER_EDIT()
        ImGui::PopItemWidth();
        ImGuiUtils::CompactColorPicker("Color", &color[0]);
        CREATE_HISTORY_ENTRY_AFTER_EDIT()

        ImGui::NewLine();

        ImGui::Text("Shadow Properties");
        ImGui::Separator();
        ImGui::NewLine();

        // Draw shadow properties editor content, return true if any value has
        // been manipulated through the editor:
        bool shadow_properties_changed = shadow_properties.DrawEditorContent();

        if (shadow_properties_changed)
        {
            App->event->Publish(Event::Type::CREATE_EDITOR_HISTORY_ENTRY);
        }
    }
    ImGui::PopID();
}