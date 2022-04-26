#pragma once

namespace Hachiko
{
    class Hardware
    {
    public:
        struct hw_info
        {
            SDL_version sdl_version;
            int n_cpu;
            float ram_gb;
            unsigned char* gpu;
            unsigned char* gpu_brand;
            int vram_mb_budget;
            int vram_mb_free;
        };

    public:
        Hardware();
        ~Hardware() = default;

        hw_info GetInfo() const
        {
            return info;
        }

    private:
        mutable hw_info info{};
    };
}
