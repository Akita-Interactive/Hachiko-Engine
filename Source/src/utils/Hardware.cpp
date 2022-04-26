#include "core/hepch.h"
#include "Hardware.h"

Hachiko::Hardware::Hardware() 
{
    SDL_GetVersion(&info.sdl_version);

    info.n_cpu = SDL_GetCPUCount();
    info.ram_gb = SDL_GetSystemRAM() / 1024.0f;

    info.gpu = (unsigned char*)glGetString(GL_RENDERER);
    info.gpu_brand = (unsigned char*)glGetString(GL_VENDOR);
    glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &info.vram_mb_budget);
    glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &info.vram_mb_free);

    info.vram_mb_budget = info.vram_mb_budget / 1024;
    glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &info.vram_mb_free);
    info.vram_mb_free = info.vram_mb_free / 1024;
}
