#pragma once
#include "Module.h"
#include "Globals.h"

#include "batching/BatchManager.h"
#include "core/rendering/RenderList.h"

#include <vector>

namespace Hachiko
{
    class ComponentCamera;
    class Scene;

    struct GpuData
    {
        unsigned char* name;
        unsigned char* brand;
        float vram_budget_mb;
    };

    struct GlVersion
    {
        unsigned char* glew;
        unsigned char* opengl;
        unsigned char* glsl;
    };

    class ModuleRender : public Module
    {
    public:
        ModuleRender();
        ~ModuleRender() override;

        bool Init() override;
        UpdateStatus PreUpdate(float delta) override;
        UpdateStatus Update(float delta) override;
        UpdateStatus PostUpdate(float delta) override;
        bool CleanUp() override;

        [[nodiscard]] unsigned int GetFrameBuffer() const
        {
            return frame_buffer;
        }

        [[nodiscard]] unsigned int GetTextureId() const
        {
            return fb_texture;
        }

        void OptionsMenu();
        void PerformanceMenu();
        void FpsGraph() const;
        void AddFrame(float delta);

        [[nodiscard]] void* GetGLContext() const
        {
            return context;
        }

        [[nodiscard]] GpuData GetGpuData() const
        {
            return gpu;
        }

        [[nodiscard]] GlVersion GetGlVersion() const
        {
            return gl;
        }

        [[nodiscard]] const RenderList* GetRenderList() const
        {
            return &render_list;
        }

    private:
        void GenerateFrameBuffer();
        void ResizeFrameBuffer(int heigth, int width) const;
        void ManageResolution(ComponentCamera* camera);
        void Draw(Scene* scene, ComponentCamera* camera, ComponentCamera* culling);

        void CreateContext();
        static void SetGLOptions();
        void RetrieveLibVersions();
        void RetrieveGpuInfo();

        void* context{};

        RenderList render_list;

        unsigned frame_buffer = 0;
        unsigned depth_stencil_buffer = 0;
        unsigned fb_texture = 0;
        unsigned fb_height = 0;
        unsigned fb_width = 0;

        // float4 clear_color;
        bool draw_skybox = true;
        bool draw_navmesh = false;
        bool outline_selection = true;

        GpuData gpu{};
        GlVersion gl{};
        int vram_free{};

        static const unsigned n_bins = 50;
        std::vector<float> fps_log;
        std::vector<float> ms_log;
        float current_fps = 0.0f;
        float current_ms = 0.0f;
    };
}
