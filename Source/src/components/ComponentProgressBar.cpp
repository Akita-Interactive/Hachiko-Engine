#include "core/hepch.h"
#include "ComponentProgressBar.h"

Hachiko::ComponentProgressBar::ComponentProgressBar(GameObject* container) : Component(Type::PROGRESS_BAR, container) 
{
	// Get backgroundID;
	// Get foregroundID;
	// Get background;
	// Get foreground;
}

void Hachiko::ComponentProgressBar::Update() 
{
    if (game_object->children.size() < 2)
    {
        // Needs two children to work, early exit
        return;
    }
    // First children background, second fill
    background = game_object->children[0];
    fill = game_object->children[1];
    
    const ComponentTransform2D* bg_transform = background->GetComponent<ComponentTransform2D>();
    ComponentTransform2D* fill_transform = fill->GetComponent<ComponentTransform2D>();
    if (!bg_transform || !fill_transform)
    {
        // Needs both transforms, early exit
        return;
    }

    float fill_pct = (filled_value - min) / (max - min);

    const float3& bg_position = bg_transform->GetPosition();
    const float2& bg_size = bg_transform->GetSize();
    float2 new_size = bg_size;
    float3 new_position = bg_position;

    switch (fill_direction)
    {
    case FillingDirection::LEFT_TO_RIGHT:
        new_position.x -= ((bg_size.x - (bg_size.x * fill_pct))/ 2);
        new_size.x *= fill_pct;        
        break;
    case FillingDirection::RIGHT_TO_LEFT:
        new_position.x += ((bg_size.x - (bg_size.x * fill_pct)) / 2);
        new_size.x *= fill_pct;
        break;
    case FillingDirection::BOTTOM_TO_TOP:
        new_position.y -= ((bg_size.y - (bg_size.y * fill_pct)) / 2);
        new_size.y *= fill_pct;
        break;
    case FillingDirection::TOP_TO_BOTTOM:
        new_position.y += ((bg_size.y - (bg_size.y * fill_pct)) / 2);
        new_size.y *= fill_pct;
        break;
    }
    fill_transform->SetPosition(new_position);
    fill_transform->SetSize(new_size);

}

void Hachiko::ComponentProgressBar::DrawGui()
{
    if (ImGui::CollapsingHeader("Progress Bar", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::DragFloat("Min", &min, 1.0f, -inf, max - 1);
        ImGui::DragFloat("Max", &max, 1.0f, min + 1, inf);
        ImGui::DragFloat("Filled", &filled_value, (max-min) / 100, min, max);

        ImGui::Combo("Filling Direction", &direction_index, filling_directions, IM_ARRAYSIZE(filling_directions));
        fill_direction = static_cast<FillingDirection>(direction_index);
    }
}

void Hachiko::ComponentProgressBar::Save(YAML::Node& node) const
{
    node[PROGRESSBAR_MIN] = min;
    node[PROGRESSBAR_MAX] = max;
    node[PROGRESSBAR_FILLED_VALUE] = filled_value;
    node[PROGRESSBAR_DIRECTION] = static_cast<int>(fill_direction);
}

void Hachiko::ComponentProgressBar::Load(const YAML::Node& node)
{
    SetMin(node[PROGRESSBAR_MIN].as<float>());
    SetMax(node[PROGRESSBAR_MAX].as<float>());
    SetFilledValue(node[PROGRESSBAR_FILLED_VALUE].as<float>());
    SetDirection(static_cast<FillingDirection>(node[PROGRESSBAR_DIRECTION].as<int>()));
}
