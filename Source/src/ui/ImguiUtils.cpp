#include "core/hepch.h"
#include "ImGuiUtils.h"
#include <IconsFontAwesome5.h>

#include <imgui_internal.h>

#include "core/GameObject.h"
#include "components/Component.h"
#include "modules/ModuleEditor.h"

bool Hachiko::ImGuiUtils::IconButton(const char* icon, const char* tooltip)
{
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_WindowBg]);
    const bool res = ImGui::SmallButton(icon);
    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("%s", tooltip);
    }
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    return res;
}

void Hachiko::ImGuiUtils::VSplitter(const char* str_id, ImVec2* size)
{
    const ImVec2 screen_pos = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton(str_id, ImVec2(3, -1));
    const ImVec2 end_pos = screen_pos + ImGui::GetItemRectSize();
    const ImGuiWindow* win = ImGui::GetCurrentWindow();
    const ImVec4* colors = ImGui::GetStyle().Colors;
    const ImU32 color = ImGui::GetColorU32(ImGui::IsItemActive() || ImGui::IsItemHovered() ? colors[ImGuiCol_ButtonActive] : colors[ImGuiCol_Button]);
    win->DrawList->AddRectFilled(screen_pos, end_pos, color);
    if (ImGui::IsItemActive())
    {
        size->x = ImMax(1.0f, ImGui::GetIO().MouseDelta.x + size->x);
    }
}

void Hachiko::ImGuiUtils::Rect(float w, float h, ImU32 color)
{
    const ImGuiWindow* win = ImGui::GetCurrentWindow();
    const ImVec2 screen_pos = ImGui::GetCursorScreenPos();
    const ImVec2 end_pos = screen_pos + ImVec2(w, h);
    const ImRect total_bb(screen_pos, end_pos);
    ImGui::ItemSize(total_bb);
    if (!ImGui::ItemAdd(total_bb, 0))
    {
        return;
    }
    win->DrawList->AddRectFilled(screen_pos, end_pos, color);
}

bool Hachiko::ImGuiUtils::CompactColorPicker(const char* name, float* color)
{
    constexpr ImGuiColorEditFlags flags = ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoLabel;
    ImGui::PushItemWidth(150.0f);
    const bool ret = ImGui::ColorPicker4(name, color, flags);
    ImGui::PopItemWidth();
    return ret;
}

bool Hachiko::ImGuiUtils::CollapsingHeader(GameObject* game_object, const Component* component, const char* header_name)
{
    ImGui::PushID(component);
    bool open = ImGui::CollapsingHeader(header_name, ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen);
    const float spacing = ImGui::GetStyle().IndentSpacing + ImGui::GetStyle().FramePadding.x + 1;
    ImGui::SameLine();

    ImGui::Indent(ImGui::GetWindowContentRegionMax().x - spacing);

    if (ImGui::Button(std::string(ICON_FA_TIMES).c_str()))
    {
        ImGui::OpenPopup(header_name);
    }

    ImGui::Unindent(ImGui::GetWindowContentRegionMax().x - spacing);

    if (ImGui::BeginPopup(header_name))
    {
        App->editor->to_remove = component;
        
        ImGui::EndPopup();
        return false;
    }
    ImGui::PopID();

    return open;
}

bool Hachiko::ImGuiUtils::ToolbarButton(ImFont* const font, const char* font_icon, bool active, const char* tooltip_desc, const bool enabled)
{
    const ImVec4 col_active = ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];
    ImVec4 bg_color = active ? col_active : ImGui::GetStyle().Colors[ImGuiCol_Text];
    ImGui::SameLine();
    const auto frame_padding = ImGui::GetStyle().FramePadding;
    // If the button is not enabled we lower its alpha channel and disable the widget
    if (!enabled)
    {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        bg_color.w = 0.5f;
    }

    const ImVec4 color{0, 0, 0, 0};
    ImGui::PushStyleColor(ImGuiCol_FrameBg, color);
    ImGui::PushStyleColor(ImGuiCol_Text, bg_color);
    ImGui::PushStyleColor(ImGuiCol_Button, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, ImGui::GetStyle().FramePadding.y));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, frame_padding);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);

    ImGui::PushFont(font);
    active = ImGui::Button(font_icon);
    ImGui::PopFont();
    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar(3);

    if (!enabled)
    {
        ImGui::PopItemFlag();
    }
    else
    {
        ImGui::SameLine();
        DisplayTooltip(tooltip_desc);
    }

    return active;
}

void Hachiko::ImGuiUtils::DisplayTooltip(const char* desc)
{
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * TOOLTIP_TEXT_SIZE);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void Hachiko::ImGuiUtils::Combo(const char* label, std::vector<std::string> list, unsigned& index)
{
    if (ImGui::BeginCombo(label, list[index].c_str()))
    {
        for (int n = 0; n < list.size(); n++)
        {
            const bool is_selected = (index == n);
            if (ImGui::Selectable(list[n].c_str(), is_selected))
            {
                index = n;
            }

            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
}
