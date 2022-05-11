#include "core/hepch.h"
#include "WindowAbout.h"

#include "utils/Hardware.h"

Hachiko::WindowAbout::WindowAbout() :
    Window("About", false)
{
    hardware = new Hardware();
}

Hachiko::WindowAbout::~WindowAbout()
{
    delete hardware;
}

void Hachiko::WindowAbout::Update()
{
    ImGui::SetNextWindowSize(ImVec2(400.0f, 200.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin(name, &active, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoNavInputs))
    {
        ImGui::Text("Engine name: %s", TITLE);
        ImGui::Text("Version: %s", ENGINE_VERSION);
        ImGui::Separator();
        ImGui::Text("Akita Interactive");
        ImGui::Text("Master in Advanced Programming for AAA Video Games UPC.");
        ImGui::Text("Engine licensed under the MIT License.");
        ImGui::Separator();

        auto child_size = ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 6);

        ImGui::Text("SDL Version: %d.%d.%d",
            hardware->GetInfo().sdl_version.major, 
            hardware->GetInfo().sdl_version.minor, 
            hardware->GetInfo().sdl_version.patch);
        ImGui::Separator();
        ImGui::Text("CPUs: %d", hardware->GetInfo().n_cpu);
        ImGui::Text("System RAM: %.1f Gb", hardware->GetInfo().ram_gb);
        ImGui::Separator();
        ImGui::Text("GPU: %s", hardware->GetInfo().gpu);
        ImGui::Text("Brand: %s", hardware->GetInfo().gpu_brand);
        ImGui::Text("VRAM Budget: %.1f Mb", static_cast<float>(hardware->GetInfo().vram_mb_budget));
        ImGui::Text("Vram Avaliable:  %.1f Mb", static_cast<float>(hardware->GetInfo().vram_mb_free));
        ImGui::Text("Vram Usage:  %.1f Mb", 
            (float)(hardware->GetInfo().vram_mb_budget) - (float)(hardware->GetInfo().vram_mb_free));
        ImGui::Separator();
        ImGui::Text("DevIL lib version %d", IL_VERSION);
    }
    ImGui::End();
}
