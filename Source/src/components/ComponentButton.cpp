#include "core/hepch.h"
#include "ComponentButton.h"

Hachiko::ComponentButton::ComponentButton(GameObject* container) 
	: Component(Type::BUTTON, container)
{
}

void Hachiko::ComponentButton::DrawGui()
{
    ImGui::PushID(this);

    if (ImGuiUtils::CollapsingHeader(this, "Button"))
    {}

    ImGui::PopID();
}

void Hachiko::ComponentButton::Save(YAML::Node& node) const
{
    node.SetTag("button");
}

void Hachiko::ComponentButton::Load(const YAML::Node& node) {}
