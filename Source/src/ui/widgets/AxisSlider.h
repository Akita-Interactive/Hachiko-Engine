#pragma once
#include "Globals.h"
#include "modules/ModuleEvent.h"

#include <string>

#include <imgui_internal.h>
#include <Math/float3.h>

namespace Hachiko::Widgets
{
    struct AxisSliderConfig
    {
        const char* label_x = "X";
        const char* label_y = "Y";
        const char* label_z = "Z";
        float3 speed = float3(0.1f);
        float3 min = float3(std::numeric_limits<float>::lowest());
        float3 max = float3(std::numeric_limits<float>::max());
        const char* format = "%.2f";
        int flags = ImGuiSliderFlags_AlwaysClamp;
    };

    inline bool AxisSlider(const std::string& label, float3& values, const AxisSliderConfig* config = nullptr)
    {
        static const ImVec4 red = ImVec4(0.75f, 0.0f, 0.0f, 1.0f);
        static const ImVec4 green = ImVec4(0.0f, 0.75f, 0.0f, 1.0f);
        static const ImVec4 blue = ImVec4(0.0f, 0.0f, 0.75f, 1.0f);
        static const ImVec4 white = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        const ImGuiWindow* window = ImGui::GetCurrentWindow();
        ImGuiContext& g = *GImGui;
        if (window->SkipItems)
        {
            return false;
        }

        if (!config)
        {
            const AxisSliderConfig g;
            config = &g;
        }

        bool changed = false;

        //this prevents different sliders to be selected at the same time
        //unless they have the same label and refer to the same value
        ImGui::PushID(label.c_str());
        ImGui::PushID(values.ptr());

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 5));
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 2));

        constexpr float item_width = -FLT_MIN;

        const float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        const ImVec2 size = {lineHeight + 3.0f, lineHeight};

        ImGui::BeginTable(label.c_str(), 4, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_NoSavedSettings);
        ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthStretch, 0.25f);
        ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthStretch, 0.25f);
        ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthStretch, 0.25f);
        ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthStretch, 0.25f);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        ImGui::TextWrapped(label.c_str());
        ImGui::Spacing();

        ImGui::TableNextColumn();

        ImGui::PushItemWidth(item_width);
        ImGui::PushID(1);
        ImGui::PushStyleColor(ImGuiCol_Text, white);
        ImGui::PushStyleColor(ImGuiCol_Button, red);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, red);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, red);
        ImGui::Button(config->label_x, size);
        ImGui::PopStyleColor(4);
        ImGui::PopID();

        ImGui::SameLine();
        if (ImGui::DragFloat("##X", &values.x, config->speed.x, config->min.x, config->max.x, config->format, config->flags))
        {
            changed = true;
        }
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::TableNextColumn();
        ImGui::PushItemWidth(item_width);
        CREATE_HISTORY_ENTRY_AFTER_EDIT()

        ImGui::PushID(2);
        ImGui::PushStyleColor(ImGuiCol_Text, white);
        ImGui::PushStyleColor(ImGuiCol_Button, green);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, green);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, green);
        ImGui::Button(config->label_y, size);
        ImGui::PopStyleColor(4);
        ImGui::PopID();

        ImGui::SameLine();
        if (ImGui::DragFloat("##Y", &values.y, config->speed.y, config->min.y, config->max.y, config->format, config->flags))
        {
            changed = true;
        }
        ImGui::PopItemWidth();

        ImGui::TableNextColumn();
        ImGui::PushItemWidth(item_width);
        CREATE_HISTORY_ENTRY_AFTER_EDIT()

        ImGui::PushID(3);
        ImGui::PushStyleColor(ImGuiCol_Text, white);
        ImGui::PushStyleColor(ImGuiCol_Button, blue);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, blue);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, blue);
        ImGui::Button(config->label_z, size);
        ImGui::PopStyleColor(4);
        ImGui::PopID();

        ImGui::SameLine();
        if (ImGui::DragFloat("##Z", &values.z, config->speed.z, config->min.z, config->max.z, config->format, config->flags))
        {
            changed = true;
        }

        ImGui::PopItemWidth();
        CREATE_HISTORY_ENTRY_AFTER_EDIT()

        ImGui::EndTable();
        ImGui::PopStyleVar(2);
        ImGui::PopID();
        ImGui::PopID();
        return changed;
    }
};
