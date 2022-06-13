#pragma once
#include "core/particles/ParticleSystem.h"

#include "modules/ModuleEvent.h"

#include <imgui.h>
#include <string>

namespace Hachiko::Widgets
{
    inline bool MultiTypeSelector(const std::string& label, ParticleSystem::VariableTypeProperty& variable_type_property, const DragFloatConfig* config = nullptr)
    {
        if (!config)
        {
            const DragFloatConfig cfg;
            config = &cfg;
        }
        ImGui::PushID(label.c_str());
        ImGui::PushID(&variable_type_property);

        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 2));

        const float spacing = ImGui::GetStyle().IndentSpacing + 1;
        const int offset = !variable_type_property.constant_enabled;
        if (!variable_type_property.constant_enabled && variable_type_property.selected_option == ParticleSystem::Selection::CONSTANT)
        {
            variable_type_property.selected_option = ParticleSystem::Selection::BETWEEN_VALUES;
        }

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
            ImGui::DragFloat("##x", &variable_type_property.values.x, config->speed, config->min, config->max, config->format, config->flags);
            ImGui::PopItemWidth();

            if (variable_type_property.selected_option == ParticleSystem::Selection::BETWEEN_VALUES)
            {
                ImGui::SameLine();
                ImGui::PushItemWidth(-FLT_MIN);
                ImGui::DragFloat("##y", &variable_type_property.values.y, config->speed, config->min, config->max, config->format, config->flags);
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
            if (ImGui::CurveThumbnail(
                ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetCurrentContext()->FontSize + 4),
                variable_type_property.curve,
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

        const int size = 1 + static_cast<int>(variable_type_property.constant_enabled) + static_cast<int>(variable_type_property.curve_enabled);
        const char* items[3];

        if (variable_type_property.constant_enabled)
        {
            items[0] = "Constant";
            items[1] = "Random between two values";
            items[2] = "Curve";
        }
        else
        {
            items[0] = "Random between two values";
            items[1] = "Curve";
        }

        if (ImGui::BeginCombo("###", items[static_cast<int>(variable_type_property.selected_option) - offset], ImGuiComboFlags_NoPreview))
        {
            for (int i = 0; i < size; i++)
            {
                const bool is_selected = static_cast<int>(variable_type_property.selected_option) - offset == i;
                if (ImGui::Selectable(items[i], is_selected))
                {
                    variable_type_property.selected_option = static_cast<ParticleSystem::Selection>(i + offset);
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
