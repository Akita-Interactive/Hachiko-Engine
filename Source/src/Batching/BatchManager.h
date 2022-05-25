#pragma once

#include "GeometryBatch.h"
#include "TextureBatch.h"

#include <vector>

namespace Hachiko
{
    class ComponentMeshRenderer;
    
    class BatchManager
    {
    public:
        BatchManager();
        ~BatchManager();

        void CollectMeshes(const GameObject* game_object);

        void CollectMesh(const GameObject* game_object);

        void AddDrawComponent(const ComponentMeshRenderer* mesh);

        void BuildBatches();
        void DrawBatches();
        void ClearBatchesDrawList();

        void CleanUp();

        void DebugMenu();

        unsigned material_ssbo;
        // One GeometryBatch per attribute layout
        std::vector<GeometryBatch*> geometry_batches {};

        // Indicates if the first or the second segment of the persistent buffers are begin used
        bool use_first_segment = true;
    };
} // namespace Hachiko
