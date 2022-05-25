#pragma once

#include "resources/ResourceMesh.h"
#include "DrawCommand.h"

#include <vector>

namespace Hachiko
{
    class ComponentMeshRenderer;
    class TextureBatch;

    struct Material;

    struct PalettePerInstance
    {
        unsigned numBones;
        unsigned paletteOffset;
        unsigned padding0, padding1;
    };

    class GeometryBatch
    {
    public:

        GeometryBatch(ResourceMesh::Layout batch_layout);
        ~GeometryBatch();

        void AddMesh(const ComponentMeshRenderer* mesh);

        void AddDrawComponent(const ComponentMeshRenderer* mesh);

        void BuildBatch();
        void BatchMeshes();
        void BatchData();

        void UpdateWithTextureBatch(bool use_first_segment);

        void ClearDrawList();

        void BindBuffers(bool use_first_segment);
        void GenerateBuffers();
        void UpdateBuffers();
        void GenerateCommands();        

        void ImGuiWindow();

        const std::vector<DrawCommand>& GetCommands() const
        {
            return commands;
        }
        
        std::vector<const ComponentMeshRenderer*> components; // contains all ComponentMeshes in the batch
        // Commands will be used as templates for the final command list
        std::unordered_map<const ResourceMesh*, DrawCommand*> resources; // contains unique ResourceMeshes and their position in the buffer
                
        // We can use resource mesh to contain a concatenation of all original meshes
        unsigned component_count = 0;
        unsigned component_palette_count = 0;

        ResourceMesh* batch = nullptr;
        unsigned instance_indices_vbo;
        std::vector<float4x4> transforms;
        std::vector<PalettePerInstance> palettes_per_instance;
        std::vector<float4x4> palettes;
        std::vector<DrawCommand> commands;

        TextureBatch* texture_batch = nullptr;
        unsigned indirect_buffer_id;
        unsigned transform_buffer;
        unsigned palettes_buffer;
        unsigned palettes_per_instances_buffer;

        float4x4* transform_buffer_data = nullptr;
        float4x4* palettes_buffer_data = nullptr;
        PalettePerInstance* palettes_per_instances_buffer_data = nullptr;
    };

} // namespace Hachiko