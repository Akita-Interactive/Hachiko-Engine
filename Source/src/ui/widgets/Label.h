#pragma once

#include <imgui.h>
#include <string>

namespace Hachiko::Widgets
{
    inline void Label(const std::string& label, const std::string& value)
    {
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
        ImGui::PushItemWidth(-1);

        ImGui::TextWrapped(value.c_str());

        ImGui::PopItemWidth();
        ImGui::EndTable();
        ImGui::PopStyleVar();
        ImGui::PopID();
        ImGui::PopID();
    }
}