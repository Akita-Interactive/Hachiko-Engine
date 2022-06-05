#pragma once

#include <imgui.h>
#include <string>

namespace Hachiko::Widgets
{
    struct DragFloatConfig
    {
        float speed = 0.1f;
        float min = std::numeric_limits<float>::lowest();
        float max = std::numeric_limits<float>::max();
        bool enabled = true;
        const char* format = "%.2f";
        int flags = ImGuiSliderFlags_AlwaysClamp;
    };

    inline bool DragFloat(const std::string& label, float& value, const DragFloatConfig* config = nullptr)
    {
        const ImGuiWindow* window = ImGui::GetCurrentWindow();

        if (window->SkipItems)
        {
            return false;
        }

        if (!config)
        {
            const DragFloatConfig cfg;
            config = &cfg;
        }
        ImGui::PushID(label.c_str());
        ImGui::PushID(&value);
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 2));
        ImGui::BeginTable("", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_NoSavedSettings);
        ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthStretch, 0.25f);
        ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthStretch, 0.75f);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        ImGui::TextWrapped(label.c_str());
        ImGui::Spacing();

        ImGui::TableNextColumn();
        ImGui::PushItemWidth(-FLT_MIN);

        const bool value_changed = ImGui::DragFloat("##value", &value, config->speed, config->min, config->max, config->format, config->flags);

        ImGui::PopItemWidth();
        ImGui::EndTable();
        ImGui::PopStyleVar();
        ImGui::PopID();
        ImGui::PopID();
        return value_changed;
    }
}
