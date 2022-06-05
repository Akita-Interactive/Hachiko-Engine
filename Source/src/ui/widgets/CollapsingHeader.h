#pragma once

#include <imgui_internal.h>
#include "modules/ModuleEditor.h"

namespace Hachiko::Widgets
{
    enum class CollapsibleHeaderType
    {
        None = 0,
        Space,
        Checkbox,
        Default
    };

    inline bool CollapsingHeader(const char* label, bool* active, CollapsibleHeaderType type = CollapsibleHeaderType::None, ImGuiTreeNodeFlags flags_ = 0)
    {
        //Copied from ImGui::TreeNodeBehavior

        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
        {
            return false;
        }

        ImGuiID id = window->GetID(label);
        const ImGuiTreeNodeFlags flags = flags_ | ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_AllowItemOverlap;
        const char* label_end = ImGui::FindRenderedTextEnd(label);

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const bool display_frame = (flags & ImGuiTreeNodeFlags_Framed) != 0;
        const ImVec2 padding = (display_frame || (flags & ImGuiTreeNodeFlags_FramePadding))
                                   ? style.FramePadding
                                   : ImVec2(style.FramePadding.x, ImMin(window->DC.CurrLineTextBaseOffset, style.FramePadding.y));

        const ImVec2 label_size = ImGui::CalcTextSize(label, label_end, false);

        const float frame_height = ImMax(ImMin(window->DC.CurrLineSize.y, g.FontSize + style.FramePadding.y * 2), label_size.y + padding.y * 2);
        ImRect frame_bb;
        frame_bb.Min.x = (flags & ImGuiTreeNodeFlags_SpanFullWidth) ? window->WorkRect.Min.x : window->DC.CursorPos.x;
        frame_bb.Min.y = window->DC.CursorPos.y;
        frame_bb.Max.x = window->WorkRect.Max.x;
        frame_bb.Max.y = window->DC.CursorPos.y + frame_height;
        if (display_frame)
        {
            frame_bb.Min.x -= IM_FLOOR(window->WindowPadding.x * 0.5f - 1.0f);
            frame_bb.Max.x += IM_FLOOR(window->WindowPadding.x * 0.5f);
        }

        const float text_offset_x = g.FontSize + (display_frame ? padding.x * 3 : padding.x * 2); // Collapser arrow width + Spacing
        const float text_offset_y = ImMax(padding.y, window->DC.CurrLineTextBaseOffset); // Latch before ItemSize changes it
        const float text_width = g.FontSize + (label_size.x > 0.0f ? label_size.x + padding.x * 2 : 0.0f); // Include collapser
        ImVec2 text_pos(window->DC.CursorPos.x + text_offset_x, window->DC.CursorPos.y + text_offset_y);
        ImGui::ItemSize(ImVec2(text_width, frame_height), padding.y);

        ImRect interact_bb = frame_bb;
        if (!display_frame && (flags & (ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth)) == 0)
        {
            interact_bb.Max.x = frame_bb.Min.x + text_width + style.ItemSpacing.x * 2.0f;
        }

        const bool is_leaf = (flags & ImGuiTreeNodeFlags_Leaf) != 0;
        bool is_open = ImGui::TreeNodeBehaviorIsOpen(id, flags);
        if (is_open && !g.NavIdIsAlive && (flags & ImGuiTreeNodeFlags_NavLeftJumpsBackHere) && !(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
        {
            window->DC.TreeJumpToParentOnPopMask |= (1 << window->DC.TreeDepth);
        }

        const bool item_add = ImGui::ItemAdd(interact_bb, id);
        g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HasDisplayRect;
        g.LastItemData.DisplayRect = frame_bb;

        if (!item_add)
        {
            if (is_open && !(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
            {
                ImGui::TreePushOverrideID(id);
            }
            return is_open;
        }

        ImGuiButtonFlags button_flags = ImGuiTreeNodeFlags_None;
        if (flags & ImGuiTreeNodeFlags_AllowItemOverlap)
        {
            button_flags |= ImGuiButtonFlags_AllowItemOverlap;
        }
        if (!is_leaf)
        {
            button_flags |= ImGuiButtonFlags_PressedOnDragDropHold;
        }

        const float arrow_hit_x1 = (text_pos.x - text_offset_x) - style.TouchExtraPadding.x;
        const float arrow_hit_x2 = (text_pos.x - text_offset_x) + (g.FontSize + padding.x * 2.0f) + style.TouchExtraPadding.x;
        const bool is_mouse_x_over_arrow = (g.IO.MousePos.x >= arrow_hit_x1 && g.IO.MousePos.x < arrow_hit_x2);
        if (window != g.HoveredWindow || !is_mouse_x_over_arrow)
        {
            button_flags |= ImGuiButtonFlags_NoKeyModifiers;
        }

        if (is_mouse_x_over_arrow)
        {
            button_flags |= ImGuiButtonFlags_PressedOnClick;
        }
        else if (flags & ImGuiTreeNodeFlags_OpenOnDoubleClick)
        {
            button_flags |= ImGuiButtonFlags_PressedOnClickRelease | ImGuiButtonFlags_PressedOnDoubleClick;
        }
        else
        {
            button_flags |= ImGuiButtonFlags_PressedOnClickRelease;
        }

        bool selected = (flags & ImGuiTreeNodeFlags_Selected) != 0;
        const bool was_selected = selected;

        bool hovered, held;
        bool pressed = ImGui::ButtonBehavior(interact_bb, id, &hovered, &held, button_flags);
        bool toggled = false;
        if (!is_leaf)
        {
            if (pressed && g.DragDropHoldJustPressedId != id)
            {
                if ((flags & (ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick)) == 0 || (g.NavActivateId == id))
                {
                    toggled = true;
                }
                if (flags & ImGuiTreeNodeFlags_OpenOnArrow)
                {
                    toggled |= is_mouse_x_over_arrow && !g.NavDisableMouseHover; // Lightweight equivalent of IsMouseHoveringRect() since ButtonBehavior() already did the job
                }
                if ((flags & ImGuiTreeNodeFlags_OpenOnDoubleClick) && g.IO.MouseClickedCount[0] == 2)
                {
                    toggled = true;
                }
            }
            else if (pressed && g.DragDropHoldJustPressedId == id)
            {
                IM_ASSERT(button_flags & ImGuiButtonFlags_PressedOnDragDropHold);
                if (!is_open)
                {
                    // When using Drag and Drop "hold to open" we keep the node highlighted after opening, but never close it again.
                    toggled = true;
                }
            }

            if (g.NavId == id && g.NavMoveDir == ImGuiDir_Left && is_open)
            {
                toggled = true;
                ImGui::NavMoveRequestCancel();
            }
            if (g.NavId == id && g.NavMoveDir == ImGuiDir_Right && !is_open) // If there's something upcoming on the line we may want to give it the priority?
            {
                toggled = true;
                ImGui::NavMoveRequestCancel();
            }

            if (toggled)
            {
                is_open = !is_open;
                window->DC.StateStorage->SetInt(id, is_open);
                g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_ToggledOpen;
            }
        }
        if (flags & ImGuiTreeNodeFlags_AllowItemOverlap)
        {
            ImGui::SetItemAllowOverlap();
        }

        if (selected != was_selected)
        {
            g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_ToggledSelection;
        }

        // Render
        const ImU32 text_col = ImGui::GetColorU32(ImGuiCol_Text);
        ImGuiNavHighlightFlags nav_highlight_flags = ImGuiNavHighlightFlags_TypeThin;
        if (display_frame)
        {
            // Framed type
            const ImU32 bg_col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header);
            ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, bg_col, true, style.FrameRounding);
            ImGui::RenderNavHighlight(frame_bb, id, nav_highlight_flags);
            if (flags & ImGuiTreeNodeFlags_Bullet)
            {
                ImGui::RenderBullet(window->DrawList, ImVec2(text_pos.x - text_offset_x * 0.60f, text_pos.y + g.FontSize * 0.5f), text_col);
            }
            else if (type == CollapsibleHeaderType::Default)
            {
                ImGui::RenderArrow(window->DrawList, ImVec2(text_pos.x - text_offset_x + padding.x, text_pos.y), text_col, is_open ? ImGuiDir_Down : ImGuiDir_Right, 1.0f);
            }
            else
            {
                text_pos.x -= text_offset_x;
            }
            if (flags & ImGuiTreeNodeFlags_ClipLabelForTrailingButton)
            {
                frame_bb.Max.x -= g.FontSize + style.FramePadding.x;
            }

            if (type == CollapsibleHeaderType::Space)
            {
                text_pos.x += text_offset_x;
            }

            if (type == CollapsibleHeaderType::Checkbox)
            {
                ImGui::PushID(active);
                const float spacing = padding.x - 1;
                ImGui::SameLine();
                ImGui::Unindent(spacing);
                ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, style.FramePadding.y - 4));

                window->DC.CursorPos.x += 4;
                window->DC.CursorPos.y += 4;
                if (App->editor->GetTheme() == Editor::Theme::Type::DARK)
                {
                    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 0.7f));
                    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
                    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.2f, 0.2f, 0.2f, 0.9f));
                    ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.9f, 0.9f, 0.9f, 1.00f));
                }
                ImGui::Checkbox("##", active);
                if (App->editor->GetTheme() == Editor::Theme::Type::DARK)
                {
                    ImGui::PopStyleColor(4);
                }

                ImGui::PopStyleVar(2);

                ImGui::Indent(spacing);
                text_pos.x += text_offset_x;
                ImGui::PopID();
            }

            ImGui::RenderTextClipped(text_pos, frame_bb.Max, label, label_end, &label_size);
        }

        if (is_open && ! (flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
        {
            ImGui::TreePushOverrideID(id);
        }
        return is_open;
    }
}
