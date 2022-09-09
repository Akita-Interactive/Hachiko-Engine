#pragma once

#include "modules/ModuleEvent.h"
#include <imgui.h>
#include <string>

namespace Hachiko::Widgets
{
    struct SliderFloatConfig
    {
        float power = 1.0f;
        float min = 0.0f;
        float max = 1.0f;
        bool enabled = true;
        const char* format = "%.2f";
    };

    inline bool SliderFloat(const std::string& label, float& value, const SliderFloatConfig* config = nullptr)
    {
        const ImGuiWindow* window = ImGui::GetCurrentWindow();

        if (window->SkipItems)
        {
            return false;
        }

        if (!config)
        {
            const SliderFloatConfig cfg;
            config = &cfg;
        }

        ImGui::BeginDisabled(!config->enabled);

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

        const bool value_changed = ImGui::SliderFloat("##value", &value, config->min, config->max, config->format, config->power);
        CREATE_HISTORY_ENTRY_AFTER_EDIT()
        ImGui::PopItemWidth();
        ImGui::EndTable();
        ImGui::PopStyleVar();
        ImGui::PopID();
        ImGui::PopID();
        ImGui::EndDisabled();
        return value_changed;
    }
}
