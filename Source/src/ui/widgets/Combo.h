#pragma once

#include <imgui.h>
#include <string>

namespace Hachiko::Widgets
{
    inline bool Combo(const std::string& label, int* current_item, const char* const items[], int items_count)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 2));
        ImGui::BeginTable("", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_NoSavedSettings);
        ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthStretch, 0.25f);
        ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthStretch, 0.75f);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        ImGui::TextUnformatted(label.c_str());

        ImGui::TableNextColumn();
        ImGui::PushItemWidth(-FLT_MIN);

        const bool value_changed = ImGui::Combo("##", current_item, items, items_count);

        ImGui::PopItemWidth();
        ImGui::EndTable();
        ImGui::PopStyleVar();
        return value_changed;
    }
}
