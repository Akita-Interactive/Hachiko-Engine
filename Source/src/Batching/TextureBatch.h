#pragma once

namespace Hachiko
{
    class ComponentMeshRenderer;
    class ResourceMaterial;
    class Program;

    class TextureBatch
    {
    public:
        struct TexAddress
        {
            int texIndex = 0; // contains the array where is located
            int layerIndex = 0; // contains the depth in the array where is located
        };

        struct TextureArray
        {
            unsigned id = 0;
            unsigned depth = 0;
            unsigned width = 0;
            unsigned height = 0;
            unsigned format = 0;
            unsigned wrap_mode = 0;
            unsigned min_filter = 0;
            unsigned mag_filter = 0;
        };

        struct Material
        {
            float4 diffuse_color = float4::zero;
            float4 specular_color = float4::zero;
            float4 emissive_color = float4::zero;
            float4 tint_color = float4::one;
            unsigned diffuse_flag = false;
            unsigned specular_flag = false;
            unsigned normal_flag = false;
            unsigned metallic_flag = false;
            unsigned emissive_flag = false;
            TexAddress diffuse_map;
            TexAddress specular_map;
            TexAddress normal_map;
            TexAddress metallic_map;
            TexAddress emissive_map;
            float smoothness = 0.f;
            float metalness_value = 0.f;
            unsigned is_metallic = false;
            unsigned is_transparent = 0;
            unsigned padding[1];
        };

        TextureBatch() {};
        ~TextureBatch();

        void AddMaterial(const ResourceMaterial* material);
        void AddTexture(const ResourceTexture* texture);

        void BuildBatch(unsigned component_count);

        void UpdateBatch(int segment, const std::vector<const ComponentMeshRenderer*>& components, unsigned component_count);
        void BindBatch(int segment, const Program* program, unsigned component_count);

        void ImGuiWindow();

    private:
        bool EqualLayout(const TextureArray& texuteLayout, const ResourceTexture& texture);

        std::map<const ResourceTexture*, TexAddress*> resources; // contains all the Texture resources and their address
        std::vector<TextureArray*> texture_arrays; // contains all the texture arrays

        std::vector<Material> materials;

        unsigned material_buffer = 0;

        Material* material_buffer_data = nullptr;

        bool loaded = false;
    };
} // namespace Hachiko