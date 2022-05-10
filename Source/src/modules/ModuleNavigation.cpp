#include "core/hepch.h"

#include "ModuleNavigation.h"
#include "core/Scene.h"
#include "ModuleSceneManager.h"
#include "DetourNavMeshQuery.h"
#include "DetourTileCache.h"
#include "DetourCommon.h"
#include "DebugUtils/DebugDraw.h"
#include "DebugUtils/DetourDebugDraw.h"

#include "ModuleCamera.h"
#include "components/ComponentCamera.h"

Hachiko::ModuleNavigation::ModuleNavigation() {}

Hachiko::ModuleNavigation::~ModuleNavigation() {}

bool Hachiko::ModuleNavigation::Init()
{
    return true;
}

bool Hachiko::ModuleNavigation::CleanUp()
{
    // TODO: Manage as a resources aand not released here
    RELEASE(navmesh);

    if (avoid_debug != nullptr)
    {
        dtFreeObstacleAvoidanceDebugData(avoid_debug);
        avoid_debug = nullptr;
    }

    return true;
}

bool Hachiko::ModuleNavigation::BuildNavmesh(Scene* scene)
{
    HE_LOG("Building Navmesh");

    ResourceNavMesh::NavmeshParams initial_params;
    if (navmesh)
    {
        // Keep previous params if we had an already created navmesh 
        initial_params = navmesh->build_params;
    }

    RELEASE(navmesh);

    navmesh = new ResourceNavMesh(0);
    navmesh->build_params = initial_params;

    bool success = navmesh->Build(scene);
    if (!success)
    {
        HE_LOG("Failed to build navmesh uwu");
    }

    // Set agents debug data to clean state
    memset(m_trails, 0, sizeof(m_trails));

    avoid_debug = dtAllocObstacleAvoidanceDebugData();
    avoid_debug->init(2048);

    memset(&agent_debug, 0, sizeof(agent_debug));
    // Idx is used to mark selected agent, unused atm
    agent_debug.idx = -1;
    agent_debug.vod = avoid_debug;

    return success;
}

UpdateStatus Hachiko::ModuleNavigation::Update(const float delta)
{
    if (!navmesh)
    {
        return UpdateStatus::UPDATE_CONTINUE;
    }
    
    dtTileCache* tile_cache = navmesh->tile_cache;
    tile_cache->update(delta, navmesh->navmesh);
    // Note: We can add crowd debug info instead of nullptr
    UpdateObstacleStats(tile_cache);

    // Crowd update with debug data
    dtCrowd* crowd = navmesh->crowd;
    if (crowd != nullptr)
    {
        crowd->update(delta, &agent_debug);

        // Update agent trails
        for (int i = 0; i < crowd->getAgentCount(); ++i)
        {
            const dtCrowdAgent* ag = crowd->getAgent(i);
            AgentTrail* trail = &m_trails[i];
            if (!ag->active)
                continue;
            // Update agent movement trail.
            trail->htrail = (trail->htrail + 1) % ResourceNavMesh::AGENT_MAX_TRAIL;
            dtVcopy(&trail->trail[trail->htrail * 3], ag->npos);
        }

        // Idk the reason behind thees, normalizes obstacle avoidance debug data?
        agent_debug.vod->normalizeSamples();
    }
    
    return UpdateStatus::UPDATE_CONTINUE;
}

dtTileCache* Hachiko::ModuleNavigation::GetTileCache() const
{
    return navmesh ? navmesh->tile_cache : nullptr;
}

dtCrowd* Hachiko::ModuleNavigation::GetCrowd() const
{
    return navmesh ? navmesh->crowd : nullptr;
}

void Hachiko::ModuleNavigation::DebugDraw()
{
    if (navmesh)
    {
        DebugDrawGL dd;

        
        ComponentCamera* camera = App->camera->GetRenderingCamera();

        glUseProgram(0);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        constexpr bool transpose = true;
        glLoadMatrixf(camera->GetProjectionMatrix(transpose).ptr());

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glLoadMatrixf(camera->GetViewMatrix(transpose).ptr());

        glDepthMask(GL_FALSE);


        navmesh->DebugDraw(dd);
        RenderAgents(dd);


        
        glDepthMask(GL_TRUE);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }
}

void Hachiko::ModuleNavigation::DrawOptionsGui()
{
    if (navmesh)
    {
        ImGui::Separator();
        ImGui::Text("Navmesh Editor");       
        if (ImGui::Button("Rebuild Navmesh"))
        {
            App->navigation->BuildNavmesh(App->scene_manager->GetActiveScene());
        }
        
        if (ImGui::CollapsingHeader("Params"))
        {
            navmesh->DrawOptionsGui();
        }        
    }
}

void Hachiko::ModuleNavigation::CorrectPosition(math::float3& position, const math::float3& extents) const 
{
    dtQueryFilter filter;
    dtPolyRef reference;

    navmesh->navigation_query->findNearestPoly(position.ptr(), extents.ptr(), &filter, &reference, 0);

    if (reference != NULL)
    {
        float height = 0.0f;
        navmesh->navigation_query->getPolyHeight(reference, position.ptr(), &height);
        position.y = height;
    }
}

float Hachiko::ModuleNavigation::GetYFromPosition(const math::float3& position) const
{
    float height = FLT_MAX;
    float3 extents = float3(2, 4, 2);

    dtQueryFilter filter;
    dtPolyRef reference;

    navmesh->navigation_query->findNearestPoly(position.ptr(), extents.ptr(), &filter, &reference, 0);

    if (reference != NULL)
    {
        navmesh->navigation_query->getPolyHeight(reference, position.ptr(), &height);
    }

    return height;
}

void Hachiko::ModuleNavigation::UpdateObstacleStats(dtTileCache* tile_cache)
{
    total_obstacle_slots = tile_cache->getObstacleCount();
    n_processing = 0;
    n_processed = 0;
    n_removing = 0;

    for (int i = 0; i < total_obstacle_slots; ++i)
    {
        const dtTileCacheObstacle* ob = tile_cache->getObstacle(i);
        switch (ob->state)
        {
        case DT_OBSTACLE_EMPTY:
            break;
        case DT_OBSTACLE_PROCESSED:
            ++n_processed;
            break;
        case DT_OBSTACLE_PROCESSING:
            ++n_processing;
            break;
        case DT_OBSTACLE_REMOVING:
            ++n_removing;
            break;
        }
    }
}



// Obtained from crowdtool
void Hachiko::ModuleNavigation::RenderAgents(duDebugDraw& dd)
{

    if (!navmesh || !navmesh->navmesh || !navmesh->crowd)
        return;

    dtNavMesh* nav = navmesh->navmesh;
    dtCrowd* crowd = navmesh->crowd;

    bool show_nodes = true;
    bool show_path = true;
    bool show_details_all = true;
    bool target_ref = true;
    bool show_grid = true;
    bool show_corners = true;
    bool show_collision_segments = true;
    bool show_neis = true;
    bool show_opt = true;
    bool show_vo = true;

    if (show_nodes && crowd->getPathQueue())
    {
        const dtNavMeshQuery* navquery = crowd->getPathQueue()->getNavQuery();
        if (navquery)
            duDebugDrawNavMeshNodes(&dd, *navquery);
    }

    dd.depthMask(false);

    // Draw paths    
    if (show_path)
    {
        
        for (int i = 0; i < crowd->getAgentCount(); i++)
        {
            if (show_details_all == false && i != agent_debug.idx)
                continue;
            const dtCrowdAgent* ag = crowd->getAgent(i);
            if (!ag->active)
                continue;
            const dtPolyRef* path = ag->corridor.getPath();
            const int npath = ag->corridor.getPathCount();
            for (int j = 0; j < npath; ++j)
                duDebugDrawNavMeshPoly(&dd, *nav, path[j], duRGBA(255, 255, 255, 24));
        }
    }

    // Draw agents target ref (This is different from example code bcs it only had 1 target and was not obtained from agents)
    if (target_ref)
    {
        for (int i = 0; i < crowd->getAgentCount(); i++)
        {
            if (show_details_all == false && i != agent_debug.idx)
                continue;
            const dtCrowdAgent* ag = crowd->getAgent(i);
            if (!ag->active)
                continue;
            const float* target = ag->targetPos;
            duDebugDrawCross(&dd, target[0], target[1] + 0.1f, target[2], navmesh->build_params.agent_radius, duRGBA(255, 255, 255, 192), 2.0f);

        }
    }

    // Occupancy grid.
    if (show_grid)
    {
        float gridy = -FLT_MAX;
        for (int i = 0; i < crowd->getAgentCount(); ++i)
        {
            const dtCrowdAgent* ag = crowd->getAgent(i);
            if (!ag->active)
                continue;
            const float* pos = ag->corridor.getPos();
            gridy = dtMax(gridy, pos[1]);
        }
        gridy += 1.0f;

        dd.begin(DU_DRAW_QUADS);
        const dtProximityGrid* grid = crowd->getGrid();
        const int* bounds = grid->getBounds();
        const float cs = grid->getCellSize();
        for (int y = bounds[1]; y <= bounds[3]; ++y)
        {
            for (int x = bounds[0]; x <= bounds[2]; ++x)
            {
                const int count = grid->getItemCountAt(x, y);
                if (!count)
                    continue;
                unsigned int col = duRGBA(128, 0, 0, dtMin(count * 40, 255));
                dd.vertex(x * cs, gridy, y * cs, col);
                dd.vertex(x * cs, gridy, y * cs + cs, col);
                dd.vertex(x * cs + cs, gridy, y * cs + cs, col);
                dd.vertex(x * cs + cs, gridy, y * cs, col);
            }
        }
        dd.end();
    }

    // Trail
    for (int i = 0; i < crowd->getAgentCount(); ++i)
    {
        const dtCrowdAgent* ag = crowd->getAgent(i);
        if (!ag->active)
            continue;

        const AgentTrail* trail = &m_trails[i];
        const float* pos = ag->npos;

        dd.begin(DU_DRAW_LINES, 3.0f);
        float prev[3], preva = 1;
        dtVcopy(prev, pos);
        for (int j = 0; j < ResourceNavMesh::AGENT_MAX_TRAIL - 1; ++j)
        {
            const int idx = (trail->htrail + ResourceNavMesh::AGENT_MAX_TRAIL - j) % ResourceNavMesh::AGENT_MAX_TRAIL;
            const float* v = &trail->trail[idx * 3];
            float a = 1 - j / (float)ResourceNavMesh::AGENT_MAX_TRAIL;
            dd.vertex(prev[0], prev[1] + 0.1f, prev[2], duRGBA(0, 0, 0, (int)(128 * preva)));
            dd.vertex(v[0], v[1] + 0.1f, v[2], duRGBA(0, 0, 0, (int)(128 * a)));
            preva = a;
            dtVcopy(prev, v);
        }
        dd.end();
    }

    // Corners & co
    for (int i = 0; i < crowd->getAgentCount(); i++)
    {
        if (show_details_all == false && i != agent_debug.idx)
            continue;
        const dtCrowdAgent* ag = crowd->getAgent(i);
        if (!ag->active)
            continue;

        const float radius = ag->params.radius;
        const float* pos = ag->npos;

        if (show_corners)
        {
            if (ag->ncorners)
            {
                dd.begin(DU_DRAW_LINES, 2.0f);
                for (int j = 0; j < ag->ncorners; ++j)
                {
                    const float* va = j == 0 ? pos : &ag->cornerVerts[(j - 1) * 3];
                    const float* vb = &ag->cornerVerts[j * 3];
                    dd.vertex(va[0], va[1] + radius, va[2], duRGBA(128, 0, 0, 192));
                    dd.vertex(vb[0], vb[1] + radius, vb[2], duRGBA(128, 0, 0, 192));
                }
                if (ag->ncorners && ag->cornerFlags[ag->ncorners - 1] & DT_STRAIGHTPATH_OFFMESH_CONNECTION)
                {
                    const float* v = &ag->cornerVerts[(ag->ncorners - 1) * 3];
                    dd.vertex(v[0], v[1], v[2], duRGBA(192, 0, 0, 192));
                    dd.vertex(v[0], v[1] + radius * 2, v[2], duRGBA(192, 0, 0, 192));
                }

                dd.end();
            }
        }

        if (show_collision_segments)
        {
            const float* center = ag->boundary.getCenter();
            duDebugDrawCross(&dd, center[0], center[1] + radius, center[2], 0.2f, duRGBA(192, 0, 128, 255), 2.0f);
            duDebugDrawCircle(&dd, center[0], center[1] + radius, center[2], ag->params.collisionQueryRange, duRGBA(192, 0, 128, 128), 2.0f);

            dd.begin(DU_DRAW_LINES, 3.0f);
            for (int j = 0; j < ag->boundary.getSegmentCount(); ++j)
            {
                const float* s = ag->boundary.getSegment(j);
                unsigned int col = duRGBA(192, 0, 128, 192);
                if (dtTriArea2D(pos, s, s + 3) < 0.0f)
                    col = duDarkenCol(col);

                duAppendArrow(&dd, s[0], s[1] + 0.2f, s[2], s[3], s[4] + 0.2f, s[5], 0.0f, 0.3f, col);
            }
            dd.end();
        }

        if (show_neis)
        {
            duDebugDrawCircle(&dd, pos[0], pos[1] + radius, pos[2], ag->params.collisionQueryRange, duRGBA(0, 192, 128, 128), 2.0f);

            dd.begin(DU_DRAW_LINES, 2.0f);
            for (int j = 0; j < ag->nneis; ++j)
            {
                // Get 'n'th active agent.
                // TODO: fix this properly.
                const dtCrowdAgent* nei = crowd->getAgent(ag->neis[j].idx);
                if (nei)
                {
                    dd.vertex(pos[0], pos[1] + radius, pos[2], duRGBA(0, 192, 128, 128));
                    dd.vertex(nei->npos[0], nei->npos[1] + radius, nei->npos[2], duRGBA(0, 192, 128, 128));
                }
            }
            dd.end();
        }

        if (show_opt)
        {
            dd.begin(DU_DRAW_LINES, 2.0f);
            dd.vertex(agent_debug.optStart[0], agent_debug.optStart[1] + 0.3f, agent_debug.optStart[2], duRGBA(0, 128, 0, 192));
            dd.vertex(agent_debug.optEnd[0], agent_debug.optEnd[1] + 0.3f, agent_debug.optEnd[2], duRGBA(0, 128, 0, 192));
            dd.end();
        }
    }

    // Agent cylinders.
    for (int i = 0; i < crowd->getAgentCount(); ++i)
    {
        const dtCrowdAgent* ag = crowd->getAgent(i);
        if (!ag->active)
            continue;

        const float radius = ag->params.radius;
        const float* pos = ag->npos;

        unsigned int col = duRGBA(0, 0, 0, 32);
        if (agent_debug.idx == i)
            col = duRGBA(255, 0, 0, 128);

        duDebugDrawCircle(&dd, pos[0], pos[1], pos[2], radius, col, 2.0f);
    }

    for (int i = 0; i < crowd->getAgentCount(); ++i)
    {
        const dtCrowdAgent* ag = crowd->getAgent(i);
        if (!ag->active)
            continue;

        const float height = ag->params.height;
        const float radius = ag->params.radius;
        const float* pos = ag->npos;

        unsigned int col = duRGBA(220, 220, 220, 128);
        if (ag->targetState == DT_CROWDAGENT_TARGET_REQUESTING || ag->targetState == DT_CROWDAGENT_TARGET_WAITING_FOR_QUEUE)
            col = duLerpCol(col, duRGBA(128, 0, 255, 128), 32);
        else if (ag->targetState == DT_CROWDAGENT_TARGET_WAITING_FOR_PATH)
            col = duLerpCol(col, duRGBA(128, 0, 255, 128), 128);
        else if (ag->targetState == DT_CROWDAGENT_TARGET_FAILED)
            col = duRGBA(255, 32, 16, 128);
        else if (ag->targetState == DT_CROWDAGENT_TARGET_VELOCITY)
            col = duLerpCol(col, duRGBA(64, 255, 0, 128), 128);

        duDebugDrawCylinder(&dd, pos[0] - radius, pos[1] + radius * 0.1f, pos[2] - radius, pos[0] + radius, pos[1] + height, pos[2] + radius, col);
    }

    if (show_vo)
    {
        for (int i = 0; i < crowd->getAgentCount(); i++)
        {
            if (show_details_all == false && i != agent_debug.idx)
                continue;
            const dtCrowdAgent* ag = crowd->getAgent(i);
            if (!ag->active)
                continue;

            // Draw detail about agent sela
            const dtObstacleAvoidanceDebugData* vod = agent_debug.vod;

            const float dx = ag->npos[0];
            const float dy = ag->npos[1] + ag->params.height;
            const float dz = ag->npos[2];

            duDebugDrawCircle(&dd, dx, dy, dz, ag->params.maxSpeed, duRGBA(255, 255, 255, 64), 2.0f);

            dd.begin(DU_DRAW_QUADS);
            for (int j = 0; j < vod->getSampleCount(); ++j)
            {
                const float* p = vod->getSampleVelocity(j);
                const float sr = vod->getSampleSize(j);
                const float pen = vod->getSamplePenalty(j);
                const float pen2 = vod->getSamplePreferredSidePenalty(j);
                unsigned int col = duLerpCol(duRGBA(255, 255, 255, 220), duRGBA(128, 96, 0, 220), (int)(pen * 255));
                col = duLerpCol(col, duRGBA(128, 0, 0, 220), (int)(pen2 * 128));
                dd.vertex(dx + p[0] - sr, dy, dz + p[2] - sr, col);
                dd.vertex(dx + p[0] - sr, dy, dz + p[2] + sr, col);
                dd.vertex(dx + p[0] + sr, dy, dz + p[2] + sr, col);
                dd.vertex(dx + p[0] + sr, dy, dz + p[2] - sr, col);
            }
            dd.end();
        }
    }

    // Velocity stuff.
    for (int i = 0; i < crowd->getAgentCount(); ++i)
    {
        const dtCrowdAgent* ag = crowd->getAgent(i);
        if (!ag->active)
            continue;

        const float radius = ag->params.radius;
        const float height = ag->params.height;
        const float* pos = ag->npos;
        const float* vel = ag->vel;
        const float* dvel = ag->dvel;

        unsigned int col = duRGBA(220, 220, 220, 192);
        if (ag->targetState == DT_CROWDAGENT_TARGET_REQUESTING || ag->targetState == DT_CROWDAGENT_TARGET_WAITING_FOR_QUEUE)
            col = duLerpCol(col, duRGBA(128, 0, 255, 192), 32);
        else if (ag->targetState == DT_CROWDAGENT_TARGET_WAITING_FOR_PATH)
            col = duLerpCol(col, duRGBA(128, 0, 255, 192), 128);
        else if (ag->targetState == DT_CROWDAGENT_TARGET_FAILED)
            col = duRGBA(255, 32, 16, 192);
        else if (ag->targetState == DT_CROWDAGENT_TARGET_VELOCITY)
            col = duLerpCol(col, duRGBA(64, 255, 0, 192), 128);

        duDebugDrawCircle(&dd, pos[0], pos[1] + height, pos[2], radius, col, 2.0f);

        duDebugDrawArrow(&dd,
                         pos[0],
                         pos[1] + height,
                         pos[2],
                         pos[0] + dvel[0],
                         pos[1] + height + dvel[1],
                         pos[2] + dvel[2],
                         0.0f,
                         0.4f,
                         duRGBA(0, 192, 255, 192),
                         (agent_debug.idx == i) ? 2.0f : 1.0f);

        duDebugDrawArrow(&dd, pos[0], pos[1] + height, pos[2], pos[0] + vel[0], pos[1] + height + vel[1], pos[2] + vel[2], 0.0f, 0.4f, duRGBA(0, 0, 0, 160), 2.0f);
    }

    dd.depthMask(true);
}