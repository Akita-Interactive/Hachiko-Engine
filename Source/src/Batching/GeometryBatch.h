#pragma once

#include "resources/ResourceMesh.h"
#include "DrawCommand.h"
#include "BatchingProperties.h"

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

        void ClearDrawList();

        void GenerateVAO();
        void UpdateVAO();
        void GenerateBuffers();

        void UpdateCommands();
        void UpdateBatch(int segment);
        void FenceSync(int segment);
        void WaitSync(int segment);
        void BindBatch(int segment, const Program* program);

        void ImGuiWindow();

        const std::vector<DrawCommand>& GetCommands() const
        {
            return commands;
        }
        
        const int GetCommandAmount() const
        {
            return commands.size();
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

        GLsync buffer_syncs[BatchingProperties::MAX_SEGMENTS] = {};
        float4x4* transform_buffer_data = nullptr;
        float4x4* palettes_buffer_data = nullptr;
        PalettePerInstance* palettes_per_instances_buffer_data = nullptr;

        bool dirty_draw_components = true;
    };

} // namespace Hachiko