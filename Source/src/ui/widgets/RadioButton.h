#pragma once

#include "modules/ModuleEvent.h"
#include <imgui.h>
#include <string>

namespace Hachiko::Widgets
{
    inline bool RadioButton(const std::string& label, bool active)
    {
        ImGui::PushID(label.c_str());
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 2));
        ImGui::BeginTable("", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_NoSavedSettings);
        ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthStretch, 0.25f);
        ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthStretch, 0.75f);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        ImGui::TextWrapped(label.c_str());
        ImGui::Spacing();

        ImGui::TableNextColumn();

        const bool value_changed = ImGui::RadioButton("##", active);
        CREATE_HISTORY_ENTRY_AFTER_EDIT()

        ImGui::EndTable();
        ImGui::PopStyleVar();
        ImGui::PopID();
        return value_changed;
    }
}
