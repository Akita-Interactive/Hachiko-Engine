#pragma once

#include "modules/ModuleEvent.h"
#include <imgui.h>
#include <string>

namespace Hachiko::Widgets
{
    inline bool Input(const std::string& label, std::string& value, ImGuiInputTextFlags flags = 0)
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

        const bool value_changed = ImGui::InputText("##", &value, flags);
        CREATE_HISTORY_ENTRY_AFTER_EDIT()

        ImGui::PopItemWidth();
        ImGui::EndTable();
        ImGui::PopStyleVar();
        ImGui::PopID();
        ImGui::PopID();
        return value_changed;
    }
}
