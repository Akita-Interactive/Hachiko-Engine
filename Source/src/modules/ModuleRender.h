#pragma once
#include "Module.h"
#include "Globals.h"

#include "core/rendering/RenderList.h"
#include "core/rendering/GBuffer.h"
#include "core/rendering/ShadowManager.h"
#include "core/rendering/BloomManager.h"

#include <vector>

namespace Hachiko
{
    class GameObject;
    class ComponentCamera;
    class ComponentTransform2D;
    class ComponentImage;
    class ComponentVideo;
    class Scene;
    class StandaloneGLTexture;

    struct GpuData
    {
        unsigned char* name;
        unsigned char* brand;
        int vram_budget_mb = 0;
        int vram_free = 0;
    };

    struct GlVersion
    {
        unsigned char* glew;
        unsigned char* opengl;
        unsigned char* glsl;
    };

    typedef int DrawConfig;
    enum DrawConfigFlags
    {
        DRAW_CONFIG_TRANSPARENT = 1 << 1,
        DRAW_CONFIG_OPAQUE = 1 << 2
    };

    class ModuleRender : public Module
    {
    public:
        ModuleRender();
        ~ModuleRender() override;

        bool Init() override;
        UpdateStatus PreUpdate(const float delta) override;
        UpdateStatus Update(const float delta) override;
        UpdateStatus PostUpdate(const float delta) override;
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
        void DeferredOptions();
        void LoadingScreenOptions();
        void PerformanceMenu();
        void FpsGraph() const;
        void AddFrame(float delta);
        void SetOpenGLAttributes() const;

        [[nodiscard]] float GetCurrentFps() const
        {
            return current_fps;
        }

        [[nodiscard]] unsigned int GetCurrentMs() const
        {
            return current_ms;
        }

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

        void SetDrawSkybox(const bool v)
        {
            draw_skybox = v;
        }

        void SetDrawNavmesh(const bool v)
        {
            draw_navmesh = v;
        }
        
        [[nodiscard]] float2 GetFrameBufferSize() const;

        [[nodiscard]] bool IsDeferred() const 
        {
            return draw_deferred;
        }

        [[nodiscard]] const unsigned& GetFullScreenQuadVAO() const
        {
            return full_screen_quad_vao;
        }

        void ApplyGaussianFilter(unsigned source_fbo, unsigned source_texture, 
            unsigned temp_fbo, unsigned temp_texture, float blur_scale_amount, 
            float blur_sigma, int blur_size, unsigned width, unsigned height, 
            const Program* program) const;

        void RenderFullScreenQuad() const;
            
        static void EnableBlending(
            GLenum blend_func_sfactor = GL_SRC_ALPHA, 
            GLenum blend_func_dfactor = GL_ONE_MINUS_SRC_ALPHA, 
            GLenum blend_equation = GL_FUNC_ADD) 
        {
            glEnable(GL_BLEND);
            glBlendFunc(blend_func_sfactor, blend_func_dfactor);
            glBlendEquation(blend_equation);
        }

        static void DisableBlending()
        {
            glDisable(GL_BLEND);
        }

        BloomManager& GetBloomManager()
        {
            return bloom_manager;
        };
    
        void LoadLoadingScreen();
        void DeleteLoadingScreen(); 
        void DrawLoadingScreen(const float delta);

    private:
        void GenerateFrameBuffer();
        void ResizeFrameBuffer(int width, int height) const;
        void ManageResolution(const ComponentCamera* camera);
        void Draw(Scene* scene, ComponentCamera* camera, ComponentCamera* culling);
        void DrawDeferred(Scene* scene, ComponentCamera* camera, BatchManager* batch_manager);
        void DrawForward(Scene* scene, BatchManager* batch_manager);
        void DrawPreForwardPass(Scene* scene, ComponentCamera* camera) const;
        bool DrawToShadowMap(Scene* scene, BatchManager* batch_manager, DrawConfig draw_config);

        void SetRenderMode(bool is_deferred);

        void CreateContext();
        static void SetGLOptions();
        void RetrieveLibVersions() const;
        void RetrieveGpuInfo();

        void GenerateFullScreenQuad();
        void FreeFullScreenQuad() const;

        void CreateNoiseTexture();
        void BindNoiseTexture(Program* program);

        void* context{};

        RenderList render_list;

        unsigned frame_buffer = 0;
        unsigned depth_stencil_buffer = 0;
        unsigned fb_texture = 0;
        unsigned fb_height = 0;
        unsigned fb_width = 0;

        // Full Screen Quad related:
        unsigned full_screen_quad_vao = 0;
        unsigned full_screen_quad_vbo = 0;
        unsigned full_screen_quad_ebo = 0;

        // Shadow Map related:
        ShadowManager shadow_manager;
        bool shadow_pass_enabled = true;

        // Deferred rendering:
        GBuffer g_buffer;
        bool draw_deferred = true;
        int deferred_mode = 0;
        bool render_forward_pass = true;

        BloomManager bloom_manager;

        bool draw_skybox = false;
        bool draw_navmesh = false;
        bool outline_selection = true;

        GpuData gpu{};
        GlVersion gl{};

        static const unsigned n_bins = 50;
        std::vector<float> fps_log;
        std::vector<float> ms_log;
        float current_fps = 0.0f;
        float current_ms = 0.0f;

        // Loading screen
        bool using_image = false;
        GameObject* loading_game_object = nullptr;
        ComponentTransform2D* loading_transform2d = nullptr;
        ComponentImage* loading_image = nullptr;
        ComponentVideo* loading_video = nullptr;

        unsigned noise_id = 0;
    };
}