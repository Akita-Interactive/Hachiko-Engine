#include "core/hepch.h"

#include "ModuleDebugMode.h"
#include "core/GameObject.h"

#include "ModuleCamera.h"
#include "components/ComponentCamera.h"

bool Hachiko::ModuleDebugMode::Init()
{
    HE_LOG("INITIALIZING MODULE: DEBUG MODE");

	hw_info.system = (unsigned char*)SDL_GetPlatform();
	hw_info.cpu = SDL_GetCPUCount();
	hw_info.ram = (float)SDL_GetSystemRAM() / 1024.0f;
	hw_info.gpu = (unsigned char*)glGetString(GL_RENDERER);
    hw_info.gpu_vendor = (unsigned char*)glGetString(GL_VENDOR);
    SDL_GetVersion(&hw_info.sdl_version);

    GLint count;
    glGetIntegerv(GL_NUM_EXTENSIONS, &count);
    for (GLint i = 0; i < count; ++i)
    {
        const char* extension = reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i));
        if (!strcmp(extension, "GL_NVX_gpu_memory_info"))
        {
            glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &hw_info.vram_capacity);
            glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &hw_info.vram_free);
        }
    }

    SetupWindow();

	return true;
}

UpdateStatus Hachiko::ModuleDebugMode::Update(const float delta)
{
	if (App->input->IsKeyPressed(SDL_SCANCODE_LALT) &&
		App->input->IsKeyPressed(SDL_SCANCODE_D))
	{
		Toggle();
	}

	if (is_gui_active)
	{
		DrawGUI();
	}

	return UpdateStatus::UPDATE_CONTINUE;
}

bool Hachiko::ModuleDebugMode::CleanUp()
{
	return true;
}

ImGuiWindowFlags Hachiko::ModuleDebugMode::SetupWindow()
{
	window_flags = ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoNavInputs;
	return window_flags;
}

void Hachiko::ModuleDebugMode::UpdateRenderValues()
{
	const Hachiko::RenderList* render_list = App->renderer->GetRenderList();
	poly_on_screen = render_list->GetPolycountRendered();
	poly_total = render_list->GetPolycountTotal();
}

// Delete after VS1
const Hachiko::GameObject* Hachiko::ModuleDebugMode::FindPlayer() const
{
	for (const Hachiko::GameObject* child : App->scene_manager->GetRoot()->children)
	{
		if (child->name == "Player")
		{
			return child;
		}
	}
	return nullptr;
}

// God mode
void Hachiko::ModuleDebugMode::SetLine(LineSegment newLine) 
{
    line = newLine;
}

void Hachiko::ModuleDebugMode::DrawGUI()
{
	static const float vram_total = static_cast<float>(hw_info.vram_capacity) / 1024.0f;
	float vram_free = static_cast<float>(hw_info.vram_free) / 1024.0f;
	float vram_usage = vram_total - vram_free;
	// Do not try this at home, delete asap
	float3 player_pos_editor;
	player = FindPlayer();
    if (player)
    {
        player_pos_editor = player->GetTransform()->GetGlobalPosition();
    }
	
	UpdateRenderValues();

    if (ImGui::Begin("InGame Window", &is_gui_active, window_flags))
    {
       // ImGui::End();
       // return;
    }
    App->renderer->FpsGraph();
    ImGui::Separator();

    ImGui::Text("Polycount (On screen): %i", poly_on_screen);
    ImGui::Text("Polycount (Total loaded): %i", poly_total);
    ImGui::Separator();
    ImGui::Text("CPUs: %d", hw_info.cpu);
    ImGui::Text("System RAM: %.1f Gb", hw_info.ram);
    ImGui::Text("GPU: %s", hw_info.gpu);
    ImGui::Text("Vendor: %s", hw_info.gpu_vendor);
    ImGui::Text("VRAM: %.1f Mb", vram_free);
    ImGui::Text("Vram Usage:  %.1f Mb", vram_usage);
    ImGui::Text("Vram Available:  %.1f Mb", vram_free);
    ImGui::Separator();

    if (ImGui::Button("Toggle Solid"))
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    ImGui::SameLine();
    if (ImGui::Button("Toggle Wire"))
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    ImGui::Separator();

    if (player)
    {
        ImGui::Text("Player position: ");
        if (ImGui::DragFloat3("(x, y, z)", player_pos_editor.ptr(), 0.1f, -inf, inf))
        {
            player->GetTransform()->SetGlobalPosition(player_pos_editor);
        }
        ImGui::Separator();
    }

    if (ImGui::Button("Reload current scene"))
    {
        App->scene_manager->ReloadScene();
    }
    ImGui::End();
}
