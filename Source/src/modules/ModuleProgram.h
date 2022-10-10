#pragma once
#include "Module.h"

#include <vector>
#include "Batching/TextureBatch.h"

#define MAX_POINT_LIGHTS 1200
#define MAX_SPOT_LIGHTS 4

namespace Hachiko
{
    class ComponentCamera;
    class ComponentDirLight;
    class ComponentPointLight;
    class ComponentSpotLight;
    class ComponentMeshRenderer;

    struct PalettePerInstance;

    class ModuleProgram : public Module
    {
    public:
        enum class BINDING
        {
            MATERIAL = 1,
            MODEL = 3,
            PALETTE = 4,
            PALETTE_PER_INSTANCE = 5,
        };

        enum class UBOPoints
        {
            CAMERA = 0,
            MATERIAL,
            LIGHTS,
            COUNT,
        };

        enum class TextureSlots
        {
            DIFFUSE = 0,
            SPECULAR,
            NORMAL,
            METALNESS,
            EMISSIVE,
            COUNT,
        };

        struct CameraData
        {
            float4x4 view = float4x4::identity;
            float4x4 proj = float4x4::identity;
            float3 pos = float3::zero;
            float padding = 0; // Renderdoc complained
        };

        ModuleProgram();
        ~ModuleProgram() override;

        bool Init() override;
        bool CleanUp() override;

        [[nodiscard]] Program* GetProgram(Program::PROGRAMS program) const
        {
            return programs[static_cast<int>(program)];
        }

        void UpdateCamera(const ComponentCamera* camera) const;
        void UpdateCamera(const Frustum& frustum) const;
        void UpdateCamera(const CameraData& camera) const;
        void UpdateMaterial(const ComponentMeshRenderer* component_mesh_renderer, const Program* program) const;
        void UpdateLights(float ambient_intensity, const float4& ambient_color, const ComponentDirLight* dir_light, const std::vector<ComponentPointLight*>& point_lights, const std::vector<ComponentSpotLight*>& spot_lights) const;

        void* CreatePersistentBuffers(unsigned& buffer_id, int binding_point, unsigned size);

    private:
        static char* LoadShaderSource(const char* shader_file_name);
        unsigned int CompileShader(unsigned type, const char* source) const;
        void CompileShaders(const char* vtx_shader_path, const char* frg_shader_path, unsigned& vtx_shader, unsigned& frg_shader) const;
        Program* CreateProgram(const char* vtx_shader_path, const char* frg_shader_path);
        void CreateGLSLIncludes() const;

        Program* programs[static_cast<int>(Program::PROGRAMS::COUNT)];

        // Assume the shader already manages its binding points
        void CreateUBO(UBOPoints binding_point, unsigned size);
        void UpdateUBO(UBOPoints binding_point, unsigned size, void* data, unsigned offset = 0) const;
        void CreateSSBO(UBOPoints binding_point, unsigned size);
        void UpdateSSBO(UBOPoints binding_point, unsigned size, void* data, unsigned offset = 0) const;

        unsigned buffers[static_cast<int>(UBOPoints::COUNT)] {};

        struct MaterialData
        {
            float4 diffuse_color;
            float4 specular_color;
            float4 emissive_color;
            unsigned diffuse_flag{};
            unsigned specular_flag{};
            unsigned normal_flag{};
            unsigned metalness_flag{};
            unsigned emissive_flag{};
            float smoothness{};
            float metalness_value{};
            unsigned is_metallic{};
            unsigned smoothness_alpha{};
            unsigned is_transparent{};
        };

        // Use float4 to prevent padding
        struct AmbientLight
        {
            float4 color = float4::one;
            float intensity = 0.05f;
            float padding[3] {};
        };

        struct DirLight
        {
            float4 direction = float4::zero;
            float4 color = float4::zero;
            float intensity {};
            float padding[3] {};
        };

        struct PointLight
        {
            float4 position = float4::zero;
            float4 color = float4::zero;
            float intensity {};
            float radius {};
            float padding[2] {};
        };

        struct SpotLight
        {
            float4 position = float4::zero;
            float4 direction = float4::zero;
            float4 color = float4::zero;
            float inner {};
            float outer {};
            float intensity {};
            float radius {};
        };

        struct Lights
        {
            AmbientLight ambient;
            DirLight directional;
            PointLight points[MAX_POINT_LIGHTS];
            SpotLight spots[MAX_SPOT_LIGHTS];
            unsigned int n_points {};
            unsigned int n_spots {};
        };
    };
} // namespace Hachiko