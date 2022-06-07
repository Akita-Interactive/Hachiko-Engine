#pragma once

#include <imgui.h>
#include <string>

namespace Hachiko::Widgets
{
    inline bool Checkbox(const std::string& label, bool* value)
    {
        ImGui::PushID(label.c_str());
        ImGui::PushID(value);
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 2));
        ImGui::BeginTable("", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_NoSavedSettings);
        ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthStretch, 0.25f);
        ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthStretch, 0.75f);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        ImGui::TextWrapped(label.c_str());
        ImGui::Spacing();

        ImGui::TableNextColumn();

        const bool value_changed = ImGui::Checkbox("##", value);

        ImGui::EndTable();
        ImGui::PopStyleVar();
        ImGui::PopID();
        ImGui::PopID();
        return value_changed;
    }
}
