#include "core/hepch.h"
#include "ResourceNaveMesh.h"

#include "Recast.h"
#include "InputGeom.h"
#include "RecastAlloc.h"
#include "RecastAssert.h"


#include "DetourCommon.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshBuilder.h"
#include "DetourNavMeshQuery.h"
#include "DetourTileCache.h"
#include "DetourTileCacheBuilder.h"

#include "SampleInterfaces.h"

#include "fastlz.h"

//#include "RecastDebugDraw.h"
//#include "DetourDebugDraw.h"

struct FastLZCompressor : public dtTileCacheCompressor
{
    virtual int maxCompressedSize(const int bufferSize) override
    {
        return (int)(bufferSize * 1.05f);
    }

    virtual dtStatus compress(const unsigned char* buffer, const int bufferSize, unsigned char* compressed, const int /*maxCompressedSize*/, int* compressedSize) override
    {
        *compressedSize = fastlz_compress((const void* const)buffer, bufferSize, compressed);
        return DT_SUCCESS;
    }

    virtual dtStatus decompress(const unsigned char* compressed, const int compressedSize, unsigned char* buffer, const int maxBufferSize, int* bufferSize) override
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

    LinearAllocator(const size_t cap) : buffer(0), capacity(0), top(0), high(0)
    {
        resize(cap);
    }

    ~LinearAllocator()
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

    virtual void reset()
    {
        high = dtMax(high, top);
        top = 0;
    }

    virtual void* alloc(const size_t size)
    {
        if (!buffer)
            return 0;
        if (top + size > capacity)
            return 0;
        unsigned char* mem = &buffer[top];
        top += size;
        return mem;
    }

    virtual void free(void* /*ptr*/)
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
                polyAreas[i] = Hachiko::SAMPLE_POLYAREA_GROUND;
                polyFlags[i] = Hachiko::SAMPLE_POLYFLAGS_WALK;
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

struct RasterizationContext
{
    RasterizationContext() : solid(0), triareas(0), lset(0), chf(0), ntiles(0)
    {
        memset(tiles, 0, sizeof(TileCacheData) * Hachiko::ResourceNavMesh::MAX_LAYERS);
    }

    ~RasterizationContext()
    {
        rcFreeHeightField(solid);
        delete[] triareas;
        rcFreeHeightfieldLayerSet(lset);
        rcFreeCompactHeightfield(chf);
        for (int i = 0; i < Hachiko::ResourceNavMesh::MAX_LAYERS; ++i)
        {
            dtFree(tiles[i].data);
            tiles[i].data = 0;
        }
    }

    rcHeightfield* solid;
    unsigned char* triareas;
    rcHeightfieldLayerSet* lset;
    rcCompactHeightfield* chf;
    TileCacheData tiles[Hachiko::ResourceNavMesh::MAX_LAYERS];
    int ntiles;
};

static int RasterizeTileLayers(const int tx, const int ty, float* verts, int nVerts, int nTris, BuildContext* ctx, rcChunkyTriMesh* chunkyMesh, const rcConfig& cfg, TileCacheData* tiles, const int maxTiles)
{
    FastLZCompressor comp;
    RasterizationContext rc;

    // Tile bounds.
    const float tcs = cfg.tileSize * cfg.cs;

    rcConfig tcfg;
    memcpy(&tcfg, &cfg, sizeof(tcfg));

    tcfg.bmin[0] = cfg.bmin[0] + tx * tcs;
    tcfg.bmin[1] = cfg.bmin[1];
    tcfg.bmin[2] = cfg.bmin[2] + ty * tcs;
    tcfg.bmax[0] = cfg.bmin[0] + (tx + 1) * tcs;
    tcfg.bmax[1] = cfg.bmax[1];
    tcfg.bmax[2] = cfg.bmin[2] + (ty + 1) * tcs;
    tcfg.bmin[0] -= tcfg.borderSize * tcfg.cs;
    tcfg.bmin[2] -= tcfg.borderSize * tcfg.cs;
    tcfg.bmax[0] += tcfg.borderSize * tcfg.cs;
    tcfg.bmax[2] += tcfg.borderSize * tcfg.cs;

    // Allocate voxel heightfield where we rasterize our input data to.
    rc.solid = rcAllocHeightfield();
    if (!rc.solid)
    {
        HE_LOG("buildNavigation: Out of memory 'solid'.");
        return 0;
    }
    if (!rcCreateHeightfield(ctx, *rc.solid, tcfg.width, tcfg.height, tcfg.bmin, tcfg.bmax, tcfg.cs, tcfg.ch))
    {
        HE_LOG("buildNavigation: Could not create solid heightfield.");
        return 0;
    }

    // Allocate array that can hold triangle flags.
    // If you have multiple meshes you need to process, allocate
    // and array which can hold the max number of triangles you need to process.
    rc.triareas = new unsigned char[nTris];
    if (!rc.triareas)
    {
        HE_LOG("buildNavigation: Out of memory 'm_triareas' (%d).", nTris);
        return 0;
    }

    float tbmin[2], tbmax[2];
    tbmin[0] = tcfg.bmin[0];
    tbmin[1] = tcfg.bmin[2];
    tbmax[0] = tcfg.bmax[0];
    tbmax[1] = tcfg.bmax[2];
    int cid[512]; // TODO: Make grow when returning too many items.
    const int ncid = rcGetChunksOverlappingRect(chunkyMesh, tbmin, tbmax, cid, 512);
    if (!ncid)
    {
        return 0; // empty
    }

    for (int i = 0; i < ncid; ++i)
    {
        const rcChunkyTriMeshNode& node = chunkyMesh->nodes[cid[i]];
        const int* tris = &chunkyMesh->tris[node.i * 3];
        const int ntris = node.n;

        memset(rc.triareas, 0, ntris * sizeof(unsigned char));
        rcMarkWalkableTriangles(ctx, tcfg.walkableSlopeAngle, verts, nVerts, tris, ntris, rc.triareas);

        if (!rcRasterizeTriangles(ctx, verts, nVerts, tris, rc.triareas, ntris, *rc.solid, tcfg.walkableClimb))
            return 0;
    }

    // Once all geometry is rasterized, we do initial pass of filtering to
    // remove unwanted overhangs caused by the conservative rasterization
    // as well as filter spans where the character cannot possibly stand.
    rcFilterLowHangingWalkableObstacles(ctx, tcfg.walkableClimb, *rc.solid);
    rcFilterLedgeSpans(ctx, tcfg.walkableHeight, tcfg.walkableClimb, *rc.solid);
    rcFilterWalkableLowHeightSpans(ctx, tcfg.walkableHeight, *rc.solid);

    rc.chf = rcAllocCompactHeightfield();
    if (!rc.chf)
    {
        HE_LOG("buildNavigation: Out of memory 'chf'.");
        return 0;
    }
    if (!rcBuildCompactHeightfield(ctx, tcfg.walkableHeight, tcfg.walkableClimb, *rc.solid, *rc.chf))
    {
        HE_LOG("buildNavigation: Could not build compact data.");
        return 0;
    }

    // Erode the walkable area by agent radius.
    if (!rcErodeWalkableArea(ctx, tcfg.walkableRadius, *rc.chf))
    {
        HE_LOG("buildNavigation: Could not erode.");
        return 0;
    }

    //// (Optional) Mark areas.
    //const ConvexVolume* vols = geom->getConvexVolumes();
    //for (int i = 0; i < geom->getConvexVolumeCount(); ++i) {
    //	rcMarkConvexPolyArea(ctx, vols[i].verts, vols[i].nverts, vols[i].hmin, vols[i].hmax, (unsigned char) vols[i].area, *rc.chf);
    //}

    rc.lset = rcAllocHeightfieldLayerSet();
    if (!rc.lset)
    {
        HE_LOG("buildNavigation: Out of memory 'lset'.");
        return 0;
    }
    if (!rcBuildHeightfieldLayers(ctx, *rc.chf, tcfg.borderSize, tcfg.walkableHeight, *rc.lset))
    {
        HE_LOG("buildNavigation: Could not build heighfield layers.");
        return 0;
    }

    rc.ntiles = 0;
    for (int i = 0; i < rcMin(rc.lset->nlayers, Hachiko::ResourceNavMesh::MAX_LAYERS); ++i)
    {
        TileCacheData* tile = &rc.tiles[rc.ntiles++];
        const rcHeightfieldLayer* layer = &rc.lset->layers[i];

        // Store header
        dtTileCacheLayerHeader header;
        header.magic = DT_TILECACHE_MAGIC;
        header.version = DT_TILECACHE_VERSION;

        // Tile layer location in the navmesh.
        header.tx = tx;
        header.ty = ty;
        header.tlayer = i;
        dtVcopy(header.bmin, layer->bmin);
        dtVcopy(header.bmax, layer->bmax);

        // Tile info.
        header.width = (unsigned char)layer->width;
        header.height = (unsigned char)layer->height;
        header.minx = (unsigned char)layer->minx;
        header.maxx = (unsigned char)layer->maxx;
        header.miny = (unsigned char)layer->miny;
        header.maxy = (unsigned char)layer->maxy;
        header.hmin = (unsigned short)layer->hmin;
        header.hmax = (unsigned short)layer->hmax;

        dtStatus status = dtBuildTileCacheLayer(&comp, &header, layer->heights, layer->areas, layer->cons, &tile->data, &tile->dataSize);
        if (dtStatusFailed(status))
        {
            return 0;
        }
    }

    // Transfer ownsership of tile data from build context to the caller.
    int n = 0;
    for (int i = 0; i < rcMin(rc.ntiles, maxTiles); ++i)
    {
        tiles[n++] = rc.tiles[i];
        rc.tiles[i].data = 0;
        rc.tiles[i].dataSize = 0;
    }

    return n;
}

static int calcLayerBufferSize(const int gridWidth, const int gridHeight)
{
    const int headerSize = dtAlign4(sizeof(dtTileCacheLayerHeader));
    const int gridSize = gridWidth * gridHeight;
    return headerSize + gridSize * 4;
}


Hachiko::ResourceNavMesh::ResourceNavMesh(UID uid) : Resource(uid, Type::NAVMESH)
{
    build_context = new BuildContext();
}


Hachiko::ResourceNavMesh::~ResourceNavMesh()
{
    RELEASE(build_context);
}

bool Hachiko::ResourceNavMesh::Build(Scene* scene)
{
    if (!scene)
    {
        HE_LOG("Error: No mesh was specified.");
        return false;
    }

    CleanUp();

    talloc = new LinearAllocator(32000);
    tcomp = new FastLZCompressor;
    tmproc = new MeshProcess;

    std::vector<float> scene_vertices;
    std::vector<int> scene_triangles;
    std::vector<float> scene_normals;
    AABB scene_bounds;
    scene->GetNavmeshData(scene_vertices, scene_triangles, scene_normals, scene_bounds);
    int n_triangles = scene_triangles.size() / 3;
    int n_vertices = scene_vertices.size() / 3;


    // Step 1. Initialize generation config.
    rcConfig cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.cs = cell_size;
    cfg.ch = cell_height;
    cfg.walkableSlopeAngle = agent_max_slope;
    cfg.walkableHeight = static_cast<int>(ceilf(agent_height / cfg.ch));
    cfg.walkableClimb = static_cast<int>(floorf(agent_max_climb / cfg.ch));
    cfg.walkableRadius = static_cast<int>(ceilf(agent_radius / cfg.cs));
    cfg.maxEdgeLen = static_cast<int>(edge_max_length / cell_size);
    cfg.maxSimplificationError = edge_max_error;
    cfg.minRegionArea = static_cast<int>(rcSqr(region_min_size)); // Note: area = size*size
    cfg.mergeRegionArea = static_cast<int>(rcSqr(region_merge_size)); // Note: area = size*size
    cfg.maxVertsPerPoly = static_cast<int>(max_vertices_per_poly);
    cfg.detailSampleDist = detail_sample_distance < 0.9f ? 0 : cell_size * detail_sample_distance;
    cfg.detailSampleMaxError = cell_height * detail_sample_max_error;
    cfg.tileSize = tile_size; // Adjust this one maybe
    cfg.borderSize = cfg.walkableRadius + 3; // Reserve enough padding.
    cfg.width = cfg.tileSize + cfg.borderSize * 2;
    cfg.height = cfg.tileSize + cfg.borderSize * 2;

    // Set the navigation were navigation will be built
    rcVcopy(cfg.bmin, scene_bounds.minPoint.ptr());
    rcVcopy(cfg.bmax, scene_bounds.maxPoint.ptr());

    // Build Tile Cache
    const float* bmin = scene_bounds.minPoint.ptr();
    const float* bmax = scene_bounds.maxPoint.ptr();
    int grid_width = 0, grid_height = 0;
    rcCalcGridSize(bmin, bmax, cell_size, &grid_width, &grid_height);
    const int tile_size = (int)tile_size;
    const int tile_width = (grid_width + tile_size - 1) / tile_size;
    const int tile_height = (grid_height + tile_size - 1) / tile_size;


    dtTileCacheParams tcparams;
    memset(&tcparams, 0, sizeof(tcparams));
    rcVcopy(tcparams.orig, bmin);
    tcparams.cs = cell_size;
    tcparams.ch = cell_height;
    tcparams.width = (int)tile_size;
    tcparams.height = (int)tile_size;
    tcparams.walkableHeight = agent_height;
    tcparams.walkableRadius = agent_radius;
    tcparams.walkableClimb = agent_max_climb;
    tcparams.maxSimplificationError = edge_max_error;
    // This value specifies how many layers (or "floors") each navmesh tile is expected to have.
    // TODO: Adjust properly
    static const int EXPECTED_LAYERS_PER_TILE = 4;
    tcparams.maxTiles = tile_width * tile_height * EXPECTED_LAYERS_PER_TILE;
    tcparams.maxObstacles = 128;

    tile_cache = dtAllocTileCache();

    dtStatus status;

    status = tile_cache->init(&tcparams, talloc, tcomp, tmproc);
    if (dtStatusFailed(status))
    {
        HE_LOG("buildTiledNavigation: Could not init tile cache.");
        return false;
    }

    // Build Navmesh

    // Arcane magic from example, should investigate more
    int tile_bits = rcMin((int)dtIlog2(dtNextPow2(tile_width * tile_height * EXPECTED_LAYERS_PER_TILE)), 14);
    int polygon_bits = 22 - tile_bits;
    int max_tiles = 1 << tile_bits;
    int max_polys_per_tile = 1 << polygon_bits;

    dtNavMeshParams params;
    memset(&params, 0, sizeof(params));
    rcVcopy(params.orig, bmin);
    params.tileWidth = tile_size * cell_size;
    params.tileHeight = tile_size * cell_size;
    params.maxTiles = max_tiles;
    params.maxPolys = max_polys_per_tile;

    navmesh = dtAllocNavMesh();
    if (!navmesh)
    {
        HE_LOG("buildTiledNavigation: Could not allocate navmesh.");
        return false;
    }

    // Init navmesh
    status = navmesh->init(&params);
    if (dtStatusFailed(status))
    {
        HE_LOG("buildTiledNavigation: Could not init navmesh.");
        return false;
    }

    // Init navigation query
    status = navigation_query->init(navmesh, 2048);
    if (dtStatusFailed(status))
    {
        HE_LOG("buildTiledNavigation: Could not init Detour navmesh query");
        return false;
    }

    rcChunkyTriMesh* chunky_mesh = new rcChunkyTriMesh;
    if (!chunky_mesh)
    {
        HE_LOG("buildTiledNavigation: Out of memory 'm_chunkyMesh'.");
        return false;
    }

    constexpr int tris_per_chunk = 256;
    if (!rcCreateChunkyTriMesh(scene_vertices.data(), scene_triangles.data(), n_triangles, tris_per_chunk, chunky_mesh))
    {
        HE_LOG("buildTiledNavigation: Failed to build chunky mesh.");
        return false;
    }

    // Preprocess tiles
    int cache_layer_count = 0;
    int cache_compressed_size = 0;
    int cache_raw_size = 0;

    for (int y = 0; y < tile_height; ++y)
    {
        for (int x = 0; x < tile_width; ++x)
        {
            TileCacheData tiles[MAX_LAYERS];
            memset(tiles, 0, sizeof(tiles));
            // TODO: Double check params
            int ntiles = RasterizeTileLayers(x, y, scene_vertices.data(), n_vertices, n_triangles, build_context, chunky_mesh, cfg, tiles, MAX_LAYERS);

            for (int i = 0; i < ntiles; ++i)
            {
                TileCacheData* tile = &tiles[i];
                status = tile_cache->addTile(tile->data, tile->dataSize, DT_COMPRESSEDTILE_FREE_DATA, 0);
                if (dtStatusFailed(status))
                {
                    dtFree(tile->data);
                    tile->data = 0;
                    continue;
                }

                cache_layer_count++;
                cache_compressed_size += tile->dataSize;
                cache_raw_size += calcLayerBufferSize(tcparams.width, tcparams.height);
            }
        }
    }

    // Build initial meshes
    for (int y = 0; y < tile_height; ++y)
        for (int x = 0; x < tile_width; ++x)
            tile_cache->buildNavMeshTilesAt(x, y, navmesh);

    size_t cache_build_memory_usagge = talloc->high;


    // Seems to only tack navmesh memory usage, we can comment it out
    const dtNavMesh* nav = navmesh;
    int nav_mesh_memory_usage = 0;
    for (int i = 0; i < nav->getMaxTiles(); ++i)
    {
        const dtMeshTile* tile = nav->getTile(i);
        if (tile->header)
            nav_mesh_memory_usage += tile->dataSize;
    }
    HE_LOG("navmeshMemUsage = %.1f kB", nav_mesh_memory_usage / 1024.0f);

    // Info on: https://github.com/recastnavigation/recastnavigation/blob/master/RecastDemo/Source/Sample_TempObstacles.cpp

    return true;
}

void Hachiko::ResourceNavMesh::DebugDraw() {}

void Hachiko::ResourceNavMesh::CleanUp()
{
	dtFreeNavMesh(navmesh);
	navmesh = nullptr;
    dtFreeNavMeshQuery(navigation_query);
    navigation_query = nullptr;
    dtFreeTileCache(tile_cache);
    tile_cache = nullptr;
}

