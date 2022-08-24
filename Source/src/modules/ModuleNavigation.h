#pragma once

#include "Module.h"
#include "DetourCrowd.h"
#include "resources/ResourceNavMesh.h"
#include "SampleInterfaces.h"

class dtTileCache;

namespace Hachiko
{
    
    class ModuleNavigation : public Module
    {
    public:
        ModuleNavigation() = default;
        ~ModuleNavigation() override = default;

        bool Init() override;
        bool CleanUp() override;
        
        UpdateStatus Update(const float delta) override; // Update crowd
        ResourceNavMesh* GetNavMesh() const
        {
            return scene_navmesh;
        };
        dtTileCache* GetTileCache() const;
        dtCrowd* GetCrowd() const;
        dtNavMeshQuery* GetNavQuery() const;

        
        void RebuildCurrentNavmesh(Scene* scene);
        void SetNavmesh(UID uid);
        // Useful from the navmesh does not come from library (rebuilds on unsaved scene)
        void SetNavmesh(ResourceNavMesh* new_reosurce); 
        ResourceNavMesh* BuildNavmeshResource(Scene* scene);

        void DebugDraw();
        void DrawOptionsGui();
        
        math::float3 GetCorrectedPosition(const math::float3& position, const math::float3& extents) const;
        void CorrectPosition(math::float3& position, const math::float3& extents) const;
        float GetYFromPosition(const math::float3& position) const;

        const dtCrowdAgent* GetAgent(int agent_id) const;
        dtCrowdAgent* GetEditableAgent(int agent_id) const;

        bool Raycast(const float3& start_pos, const float3& end_pos, float3& hit_position);

    private:
        void SetDebugData();
        void RenderAgents(duDebugDraw& dd);
        void UpdateObstacleStats(dtTileCache* tile_cache);
        ResourceNavMesh* scene_navmesh = nullptr;
        int total_obstacle_slots = 0;
        int n_processing = 0;
        int n_processed = 0;
        int n_removing = 0;
        struct AgentTrail
        {
            float trail[ResourceNavMesh::AGENT_MAX_TRAIL * 3];
            int htrail;
        };
        AgentTrail m_trails[ResourceNavMesh::MAX_AGENTS]{};
        dtCrowdAgentDebugInfo agent_debug{};
        dtObstacleAvoidanceDebugData* avoid_debug = nullptr;
    };
}