#include "core/hepch.h"
#include "WindowConsole.h"
#include "modules/ModuleEditor.h"

Hachiko::WindowConsole::WindowConsole() :
    Window("Console", true),
    imgui_logger(new ImGuiLogger())
{
    logger.Register(imgui_logger);
    auto lambda = [&](std::string&& str, LogLevel& logLevel) {
        Add(str, logLevel);
    };
    imgui_logger->SetCallBack(lambda);
}

Hachiko::WindowConsole::~WindowConsole()
{
    logger.Unregister(imgui_logger);
    delete imgui_logger;
    lines.clear();
}

void Hachiko::WindowConsole::Add(std::string& str, LogLevel& logLevel)
{
    if (lines.size() >= DISPLAY_THRESHOLD)
    {
        lines.erase(lines.begin());
    }
    lines.emplace_back(str, logLevel);
}

void Hachiko::WindowConsole::Update()
{
    ImGui::SetNextWindowDockID(App->editor->dock_down_id, ImGuiCond_FirstUseEver);
    if (!ImGui::Begin((std::string(ICON_FA_TERMINAL " ") + name).c_str(), &active, ImGuiWindowFlags_NoNavInputs))
    {
        ImGui::End();
        return;
    }
    if (ImGui::BeginPopup("Options"))
    {
        ImGui::Checkbox("Auto-scroll", &autoscroll);
        ImGui::EndPopup();
    }

    if (ImGui::Button("Options"))
    {
        ImGui::OpenPopup("Options");
    }

    ImGui::SameLine();
    const bool clear = ImGui::Button("Clear");
    ImGui::SameLine();
    const bool copy = ImGui::Button("Copy");
    ImGui::SameLine();
    filter.Draw("Filter", -100.0f);

    ImGui::Separator();
    if (ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoNavInputs))
    {
        if (clear)
        {
            Clear();
        }
        if (copy)
        {
            ImGui::LogToClipboard();
        }

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        auto filtered_lines = std::vector<std::pair<std::string, LogLevel>> {};
        if (filter.IsActive())
        {
            for (const auto& [str, logLevel] : lines)
            {
                if (filter.PassFilter(str.c_str()))
                {
                    filtered_lines.emplace_back(str, logLevel);
                }
            }
        }
        else
        {
            filtered_lines = lines;
        }

        for (const auto& [str, logLevel] : filtered_lines)
        {
            if (logLevel == LogLevel::Error)
            {
                ImVec4 color(1.0f, 0.4f, 0.4f, 1.0f);
                ImGui::PushStyleColor(ImGuiCol_Text, color);
            }
            ImGui::TextUnformatted(str.c_str());
            if (logLevel == LogLevel::Error)
            {
                ImGui::PopStyleColor();
            }
        }

        filtered_lines.clear();

        ImGui::PopStyleVar();

        if (autoscroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        {
            ImGui::SetScrollHereY(1.0f);
        }

        ImGui::EndChild();
    }
    ImGui::End();
}

void Hachiko::WindowConsole::Clear()
{
    lines.clear();
}

void Hachiko::WindowConsole::CleanUp()
{
    Clear();
    filter.Clear();
}
