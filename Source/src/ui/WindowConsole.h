#pragma once
#include "Window.h"

#include <imgui.h>

#include "core/logging/ImGuiLogger.h"

namespace Hachiko
{

#define DISPLAY_THRESHOLD 100 //lines to display;

    class WindowConsole final : public Window
    {
    public:
        WindowConsole();
        ~WindowConsole() override;

        void Update() override;
        void Clear();

        void CleanUp() override;

        void Add(std::string&, LogLevel&);

    private:
        ImGuiLogger* imgui_logger;
        std::vector<std::pair<std::string, LogLevel>> lines{};

        ImGuiTextFilter filter = nullptr;
        bool autoscroll = true;
    };
}
