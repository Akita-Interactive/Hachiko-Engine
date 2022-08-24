#pragma once

#include "GeometryBatch.h"
#include "TextureBatch.h"
#include "BatchingProperties.h"

#include <vector>

namespace Hachiko
{
    class ComponentMeshRenderer;
    class Program;
    
    class BatchManager
    {
    public:
        BatchManager();
        ~BatchManager();

        void CollectMeshes(const GameObject* game_object);
        void CollectMesh(const GameObject* game_object);

        void AddDrawComponent(const ComponentMeshRenderer* mesh);

        void BuildBatches();
        void DrawOpaqueBatches(const Program* program);
        void DrawTransparentBatches(const Program* program);

        void ClearOpaqueBatchesDrawList();
        void ClearTransparentBatchesDrawList();

        void CleanUp();

        void DebugMenu();

    private:
        void ShowDebugMenuForBatches(const std::vector<GeometryBatch*>& batches) const;
        void DrawSingleBatch(GeometryBatch* geometry_batch, const Program* program, int segment) const;

    private:
        unsigned material_ssbo;

        // One GeometryBatch per attribute layout, separated by transparencies:
        std::vector<GeometryBatch*> geometry_batches_opaque {};
        std::vector<GeometryBatch*> geometry_batches_transparent {};

        // Indicates if the first or the second segment of the persistent buffers are begin used
        int opaque_buffers_segment = 0;
        int transparent_buffers_segment = 0;
    };
} // namespace Hachiko
