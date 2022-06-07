#include "core/hepch.h"
#include "ComponentButton.h"
#include "ComponentTransform2D.h"

#include "core/rendering/Program.h"


Hachiko::ComponentButton::ComponentButton(GameObject* container) 
	: Component(Type::BUTTON, container)
{
}


void Hachiko::ComponentButton::DrawGui()
{
    ImGui::PushID(this);

    if (ImGui::CollapsingHeader("Button", ImGuiTreeNodeFlags_DefaultOpen))
    {}

    ImGui::PopID();
}

void Hachiko::ComponentButton::Save(YAML::Node& node) const
{
    node.SetTag("button");
}

void Hachiko::ComponentButton::Load(const YAML::Node& node) {}
