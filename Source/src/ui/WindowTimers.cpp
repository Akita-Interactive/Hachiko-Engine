#include "core/hepch.h"
#include "WindowTimers.h"

Hachiko::WindowTimers::WindowTimers() :
    Window("Timers", false)
{
    timer.Start();
}

Hachiko::WindowTimers::~WindowTimers() = default;

void ClockDisplay(bool running, bool paused, double time)
{
    ImVec4 color(0.0f, 1.0f, 0.0f, 1.0f);
    auto state = "Running";
    if (!running)
    {
        color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
        state = "Stopped";
    }
    else if (paused)
    {
        color = ImVec4(0.5f, 0.0f, 1.0f, 1.0f);
        state = "Paused";
    }
    ImGui::TextColored(color, "(%s): %.2f", state, time);
}

void Hachiko::WindowTimers::Update()
{
    if (ImGui::Begin(name, &active, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNavInputs))
    {
        ImGui::Text("Game Timer");
        ImGui::SameLine();
        ClockDisplay(GameTimer::running, GameTimer::paused, GameTimer::total_time);

        ImGui::Separator();
        ImGui::Text("Engine Timer");
        ImGui::SameLine();
        ClockDisplay(EngineTimer::running, false, EngineTimer::total_time);

        ImGui::Separator();
        ImGui::Text("Test Timer");
        ImGui::SameLine();
        ClockDisplay(timer.IsRunning(), false, timer.Read());

        if (ImGui::Button("Start"))
            timer.Start();
        ImGui::SameLine();
        if (ImGui::Button("Stop"))
            timer.Stop();
        ImGui::SameLine();
        if (ImGui::Button("Resume"))
            timer.Resume();
    }

    ImGui::End();
}
