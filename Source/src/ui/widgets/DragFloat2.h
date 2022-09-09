#pragma once
#include "Globals.h"
#include "modules/ModuleEvent.h"
#include "utils/Bool2.h"

#include <string>
#include <imgui_internal.h>
#include <Math/float2.h>



namespace Hachiko::Widgets
{
    struct DragFloat2Config
    {
        const char* label_x = "X";
        const char* label_y = "Y";
        float2 speed = float2(0.1f);
        float2 min = float2(std::numeric_limits<float>::lowest());
        float2 max = float2(std::numeric_limits<float>::max());
        bool2 enabled = bool2::True;
        const char* format = "%.2f";
        int flags = ImGuiSliderFlags_AlwaysClamp;
    };

    inline bool DragFloat2(const std::string& label, float2& values, const DragFloat2Config* config = nullptr)
    {
        const ImGuiWindow* window = ImGui::GetCurrentWindow();

        if (window->SkipItems)
        {
            return false;
        }

        if (!config)
        {
            const DragFloat2Config cfg;
            config = &cfg;
        }

        bool changed = false;

        //this prevents different drags to be selected at the same time
        //unless they have the same label and refer to the same value
        ImGui::PushID(label.c_str());
        ImGui::PushID(values.ptr());

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 5));
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 2));

        constexpr float item_width = -FLT_MIN;

        const float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        const ImVec2 size = {lineHeight + 3.0f, lineHeight};

        ImGui::BeginTable(label.c_str(), 3, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_NoSavedSettings);
        ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthStretch, 0.25f);
        ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthStretch, 0.375f);
        ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthStretch, 0.375f);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        ImGui::TextWrapped(label.c_str());
        ImGui::Spacing();

        ImGui::TableNextColumn();

        ImGui::PushItemWidth(item_width);
        ImGui::BeginDisabled(!config->enabled.x);
        if(config->label_x)
        {
            ImGui::Button(config->label_x, size);
            ImGui::SameLine();
        }
        if (ImGui::DragFloat("##X", &values.x, config->speed.x, config->min.x, config->max.x, config->format, config->flags))
        {
            changed = true;
        }
        CREATE_HISTORY_ENTRY_AFTER_EDIT()
        ImGui::PopItemWidth();
        ImGui::EndDisabled();

        ImGui::SameLine();
        ImGui::TableNextColumn();
        ImGui::PushItemWidth(item_width);

        ImGui::BeginDisabled(!config->enabled.y);
        if (config->label_y)
        {
            ImGui::Button(config->label_y, size);
            ImGui::SameLine();
        }

        if (ImGui::DragFloat("##Y", &values.y, config->speed.y, config->min.y, config->max.y, config->format, config->flags))
        {
            changed = true;
        }
        CREATE_HISTORY_ENTRY_AFTER_EDIT()
        ImGui::EndDisabled();
        ImGui::PopItemWidth();

        ImGui::EndTable();
        ImGui::PopStyleVar(2);
        ImGui::PopID();
        ImGui::PopID();
        return changed;
    }
};
