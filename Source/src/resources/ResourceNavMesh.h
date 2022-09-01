#pragma once
#include "Resource.h"

#include "resources/ResourceMesh.h"
#include "components/ComponentMeshRenderer.h"
#include "InputGeom.h"
#include "DetourNavMeshBuilder.h"
#include "DetourTileCache.h"
#include "fastlz.h"

#include "Recast.h"
#include "RecastAlloc.h"
#include "RecastAssert.h"


#include "DetourCommon.h"
#include "DetourNavMesh.h"

#include "DetourNavMeshQuery.h"
#include "DetourTileCache.h"
#include "DetourTileCacheBuilder.h"
#include "DetourCrowd.h"
#include "SampleInterfaces.h"


#include "DebugUtils/DebugDraw.h"
#include "DebugUtils/RecastDebugDraw.h"
#include "DebugUtils/DetourDebugDraw.h"

#include "modules/ModuleCamera.h"
#include "components/ComponentCamera.h"


/*
class dtNavMesh;
class dtNavMeshQuery;
class dtCrowd;
class dtTileCache;
class BuildContext;
class LinearAllocator;
class FastLZCompressor;
class MeshProcess;
class DebugDrawGL;*/

struct FastLZCompressor : public dtTileCacheCompressor
{
    int maxCompressedSize(const int bufferSize) override
    {
        return static_cast<int>(static_cast<float>(bufferSize) * 1.05f);
    }

    dtStatus compress(const unsigned char* buffer, const int bufferSize, unsigned char* compressed, const int /*maxCompressedSize*/, int* compressedSize) override
    {
        *compressedSize = fastlz_compress((const void* const)buffer, bufferSize, compressed);
        return DT_SUCCESS;
    }

    dtStatus decompress(const unsigned char* compressed, const int compressedSize, unsigned char* buffer, const int maxBufferSize, int* bufferSize) override
    {
        *bufferSize = fastlz_decompress(compressed, compressedSize, buffer, maxBufferSize);
        return *bufferSize < 0 ? DT_FAILURE : DT_SUCCESS;
    }
};

struct LinearAllocator : public dtTileCacheAlloc
{
    unsigned char* buffer;
    size_t capacity;
    size_t top;
    size_t high;

    explicit LinearAllocator(const size_t cap) : buffer(nullptr)
    {
        capacity = 0;
        top = 0;
        high = 0;
        resize(cap);
    }

    ~LinearAllocator() override
    {
        dtFree(buffer);
    }

    void resize(const size_t cap)
    {
        if (buffer)
            dtFree(buffer);
        buffer = (unsigned char*)dtAlloc(cap, DT_ALLOC_PERM);
        capacity = cap;
    }

    void reset() override
    {
        high = dtMax(high, top);
        top = 0;
    }

    void* alloc(const size_t size) override
    {
        if (!buffer)
            return nullptr;
        if (top + size > capacity)
            return nullptr;
        unsigned char* mem = &buffer[top];
        top += size;
        return mem;
    }

    void free(void* /*ptr*/)
    {
        // Empty
    }
};

struct MeshProcess : public dtTileCacheMeshProcess
{
    InputGeom* m_geom;

    inline MeshProcess() : m_geom(0) {}

    inline void init(InputGeom* geom)
    {
        m_geom = geom;
    }

    virtual void process(struct dtNavMeshCreateParams* params, unsigned char* polyAreas, unsigned short* polyFlags)
    {
        // Update poly flags from areas.
        for (int i = 0; i < params->polyCount; ++i)
        {
            if (polyAreas[i] == DT_TILECACHE_WALKABLE_AREA)
            {
                polyAreas[i] = 0;
                polyFlags[i] = 1;
            }
        }

        // Pass in off-mesh connections.
        if (m_geom)
        {
            params->offMeshConVerts = m_geom->getOffMeshConnectionVerts();
            params->offMeshConRad = m_geom->getOffMeshConnectionRads();
            params->offMeshConDir = m_geom->getOffMeshConnectionDirs();
            params->offMeshConAreas = m_geom->getOffMeshConnectionAreas();
            params->offMeshConFlags = m_geom->getOffMeshConnectionFlags();
            params->offMeshConUserID = m_geom->getOffMeshConnectionId();
            params->offMeshConCount = m_geom->getOffMeshConnectionCount();
        }
    }
};

struct TileCacheData
{
    unsigned char* data;
    int dataSize;
};


namespace Hachiko
{

    enum SamplePartitionType
    {
        SAMPLE_PARTITION_WATERSHED,
        SAMPLE_PARTITION_MONOTONE,
        SAMPLE_PARTITION_LAYERS,
    };

    // These are just sample areas, we have to define ours
    enum SamplePolyAreas
    {
        SAMPLE_POLYAREA_GROUND,
        SAMPLE_POLYAREA_WATER,
        SAMPLE_POLYAREA_ROAD,
        SAMPLE_POLYAREA_DOOR,
        SAMPLE_POLYAREA_GRASS,
        SAMPLE_POLYAREA_JUMP,
    };

    enum SamplePolyFlags
    {
        SAMPLE_POLYFLAGS_WALK = 0x01, // Ability to walk (ground, grass, road)
        SAMPLE_POLYFLAGS_SWIM = 0x02, // Ability to swim (water).
        SAMPLE_POLYFLAGS_DOOR = 0x04, // Ability to move through doors.
        SAMPLE_POLYFLAGS_JUMP = 0x08, // Ability to jump.
        SAMPLE_POLYFLAGS_DISABLED = 0x10, // Disabled polygon
        SAMPLE_POLYFLAGS_ALL = 0xffff // All abilities.
    };

    class ResourceNavMesh : public Resource
    {
    public:
        friend class ModuleNavigation;
        friend class NavmeshImporter;
        ResourceNavMesh(UID uid);
        ~ResourceNavMesh() override;

        dtCrowd* GetCrowd() const { return crowd; }
        dtNavMeshQuery* GetQuery() const { return navigation_query; }

        bool Build(Scene* scene);    // Creates NavMesh for the scene level
        void DebugDraw(DebugDrawGL& dd); // Draw debug info

    public:
       // Build Params
        struct NavmeshParams
        {
            // Agent
            float agent_height = 2.0f;
            float agent_radius = 0.5f;
            float agent_max_climb = 0.9f;
            float agent_max_slope = 60.0f;

            // Rasterization
            float cell_size = 0.3f;
            float cell_height = 0.20f;

            // Region
            float region_min_size = 8;
            float region_merge_size = 20;

            // Polygonization
            float edge_max_length = 12;
            float edge_max_error = 1.3f;
            float max_vertices_per_poly = 6;

            // Detail
            float detail_sample_distance = 6;
            float detail_sample_max_error = 1;
        };

        NavmeshParams build_params;
        void DrawOptionsGui();
        
    private:
        void CleanUp();
        void InitCrowd();

        //void SetupNavMeshParams();                      // Setup Params prior to nav mesh building
    private:
        dtNavMesh* navmesh = nullptr;
        dtNavMeshQuery* navigation_query = nullptr;
        dtTileCache* tile_cache = nullptr;
        dtCrowd* crowd = nullptr;
        BuildContext* build_context = nullptr;

        LinearAllocator* talloc = nullptr;
        FastLZCompressor* tcomp = nullptr;
        MeshProcess* tmproc = nullptr;

        // Partition
        SamplePartitionType partition_type = SAMPLE_PARTITION_WATERSHED;

        // Tiling? 
        int tile_size = 48;

    public:
        static const int EXPECTED_LAYERS_PER_TILE = 4;
        static const int MAX_LAYERS = 32;
        static const int MAX_AGENTS = 128;
        static const int AGENT_MAX_TRAIL = 64;
    };
}