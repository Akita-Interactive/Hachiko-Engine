#pragma once
#include "core/particles/ParticleSystem.h"

#include "modules/ModuleEvent.h"

#include <imgui.h>
#include <string>

namespace Hachiko::Widgets
{
    inline bool MultiTypeSelector(const std::string& label, ParticleSystem::VariableTypeProperty& variable_type_property)
    {
        ImGui::PushID(label.c_str());
        ImGui::PushID(&variable_type_property);

        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 2));

        const float spacing = ImGui::GetStyle().IndentSpacing + 1;

        ImGui::BeginTable("", 3, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_NoBordersInBody);
        ImGui::TableSetupColumn("##1", ImGuiTableColumnFlags_WidthStretch, 0.25f);
        ImGui::TableSetupColumn("##2", ImGuiTableColumnFlags_WidthStretch, 0.725f);
        ImGui::TableSetupColumn("##3", ImGuiTableColumnFlags_WidthFixed, spacing);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        ImGui::TextWrapped(label.c_str());
        ImGui::Spacing();

        ImGui::TableNextColumn();

        if (variable_type_property.selected_option != ParticleSystem::Selection::CURVE)
        {
            const float item_width = variable_type_property.selected_option == ParticleSystem::Selection::CONSTANT ? -FLT_MIN : -ImGui::GetContentRegionAvail().x * 0.5f;

            ImGui::PushItemWidth(item_width);
            ImGui::DragFloat("##x", &variable_type_property.values.x, 1, 0,FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::PopItemWidth();

            if (variable_type_property.selected_option == ParticleSystem::Selection::BETWEEN_VALUES)
            {
                ImGui::SameLine();
                ImGui::PushItemWidth(-FLT_MIN);
                ImGui::DragFloat("##y", &variable_type_property.values.y, 1, 0,FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp);
                ImGui::PopItemWidth();
            }
        }
        else
        {
            const bool selected = variable_type_property.selected;
            if (selected)
            {
                ImGui::PushStyleColor(ImGuiCol_PlotLines, IM_COL32(255, 0, 0, 255));
            }
            if (ImGui::Curve(
                ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetCurrentContext()->FontSize + 4),
                reinterpret_cast<ImVec2*>(variable_type_property.curve),
                ParticleSystem::CURVE_TICKS - 1))
            {
                Event evt(Event::Type::CURVE_EDITOR);
                evt.SetEventData<CurveEditorEventPayload>(&variable_type_property, label);
                App->event->Publish(evt);
            }
            if (selected)
            {
                ImGui::PopStyleColor();
            }
        }
        ImGui::TableNextColumn();

        const char* items[] = {"Constant", "Random between two values", "Curve"};
        const int size = 2 + static_cast<int>(variable_type_property.curve_enabled);

        if (ImGui::BeginCombo("###", items[static_cast<int>(variable_type_property.selected_option)], ImGuiComboFlags_NoPreview))
        {
            for (int i = 0; i < size; i++)
            {
                const bool is_selected = static_cast<int>(variable_type_property.selected_option) == i;
                if (ImGui::Selectable(items[i], is_selected))
                {
                    variable_type_property.selected_option = static_cast<ParticleSystem::Selection>(i);
                }
                if (is_selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        ImGui::EndTable();
        ImGui::PopStyleVar();

        ImGui::PopID();
        ImGui::PopID();
        return false;
    }
}
