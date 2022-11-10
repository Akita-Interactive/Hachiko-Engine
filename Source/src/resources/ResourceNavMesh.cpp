#include "core/hepch.h"
#include "ResourceNavMesh.h"


struct RasterizationContext
{
    RasterizationContext() :
        solid(0),
        triareas(0),
        lset(0),
        chf(0),
        ntiles(0)
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

static int RasterizeTileLayers(const int tx,
                               const int ty,
                               float* verts,
                               int nVerts,
                               int nTris,
                               BuildContext* ctx,
                               rcChunkyTriMesh* chunkyMesh,
                               const rcConfig& cfg,
                               TileCacheData* tiles,
                               const int maxTiles)
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
    rc.triareas = new unsigned char[chunkyMesh->maxTrisPerChunk];
    if (!rc.triareas)
    {
        HE_LOG("buildNavigation: Out of memory 'm_triareas' (%d).", chunkyMesh->maxTrisPerChunk);
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


Hachiko::ResourceNavMesh::ResourceNavMesh(UID uid) :
    Resource(uid, Type::NAVMESH)
{
    build_context = new BuildContext();
}


Hachiko::ResourceNavMesh::~ResourceNavMesh()
{
    CleanUp();
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
    cfg.cs = build_params.cell_size;
    cfg.ch = build_params.cell_height;
    cfg.walkableSlopeAngle = build_params.agent_max_slope;
    cfg.walkableHeight = static_cast<int>(ceilf(build_params.agent_height / cfg.ch));
    cfg.walkableClimb = static_cast<int>(floorf(build_params.agent_max_climb / cfg.ch));
    cfg.walkableRadius = static_cast<int>(ceilf(build_params.agent_radius / cfg.cs));
    cfg.maxEdgeLen = static_cast<int>(build_params.edge_max_length / build_params.cell_size);
    cfg.maxSimplificationError = build_params.edge_max_error;
    cfg.minRegionArea = static_cast<int>(rcSqr(build_params.region_min_size)); // Note: area = size*size
    cfg.mergeRegionArea = static_cast<int>(rcSqr(build_params.region_merge_size)); // Note: area = size*size
    cfg.maxVertsPerPoly = static_cast<int>(build_params.max_vertices_per_poly);
    cfg.detailSampleDist = build_params.detail_sample_distance < 0.9f ? 0 : build_params.cell_size * build_params.detail_sample_distance;
    cfg.detailSampleMaxError = build_params.cell_height * build_params.detail_sample_max_error;
    cfg.tileSize = tile_size; // Adjust this one maybe
    cfg.borderSize = cfg.walkableRadius + 3; // Reserve enough padding.
    cfg.width = cfg.tileSize + cfg.borderSize * 2;
    cfg.height = cfg.tileSize + cfg.borderSize * 2;

    // Set the navigation were navigation will be built
    rcVcopy(cfg.bmin, scene_bounds.minPoint.ptr());
    rcVcopy(cfg.bmax, scene_bounds.maxPoint.ptr());
    // Build Tile Cache
    int grid_width = 0, grid_height = 0;
    rcCalcGridSize(scene_bounds.minPoint.ptr(), scene_bounds.maxPoint.ptr(), build_params.cell_size, &grid_width, &grid_height);
    const int tile_width = (grid_width + tile_size - 1) / tile_size;
    const int tile_height = (grid_height + tile_size - 1) / tile_size;

    dtTileCacheParams tcparams;
    memset(&tcparams, 0, sizeof(tcparams));
    rcVcopy(tcparams.orig, scene_bounds.minPoint.ptr());
    tcparams.cs = build_params.cell_size;
    tcparams.ch = build_params.cell_height;
    tcparams.width = (int)tile_size;
    tcparams.height = (int)tile_size;
    tcparams.walkableHeight = build_params.agent_height;
    tcparams.walkableRadius = build_params.agent_radius;
    tcparams.walkableClimb = build_params.agent_max_climb;
    tcparams.maxSimplificationError = build_params.edge_max_error;
    // This value specifies how many layers (or "floors") each navmesh tile is expected to have.
    // TODO: Adjust properly
    static const int EXPECTED_LAYERS_PER_TILE = 4;
    tcparams.maxTiles = tile_width * tile_height * EXPECTED_LAYERS_PER_TILE;
    tcparams.maxObstacles = 128;

    tile_cache = dtAllocTileCache();
    if (!tile_cache)
    {
        HE_LOG("buildTiledNavigation: Could not allocate tile cache.");
        return false;
    }

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
    rcVcopy(params.orig, scene_bounds.minPoint.ptr());
    params.tileWidth = tile_size * build_params.cell_size;
    params.tileHeight = tile_size * build_params.cell_size;
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

    // Alloc and Init navigation query
    navigation_query = dtAllocNavMeshQuery();
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

    InitCrowd();
    delete chunky_mesh;

    return true;
}

void drawObstacles(duDebugDraw* dd, const dtTileCache* tc)
{
    // Draw obstacles
    for (int i = 0; i < tc->getObstacleCount(); ++i)
    {
        const dtTileCacheObstacle* ob = tc->getObstacle(i);
        if (ob->state == DT_OBSTACLE_EMPTY)
            continue;
        float bmin[3], bmax[3];
        tc->getObstacleBounds(ob, bmin, bmax);

        unsigned int col = 0;
        if (ob->state == DT_OBSTACLE_PROCESSING)
            col = duRGBA(255, 255, 0, 128);
        else if (ob->state == DT_OBSTACLE_PROCESSED)
            col = duRGBA(255, 192, 0, 192);
        else if (ob->state == DT_OBSTACLE_REMOVING)
            col = duRGBA(220, 0, 0, 128);

        switch (ob->type)
        {
            case DT_OBSTACLE_CYLINDER:
                // There seems to be no way of debug draw oriented box in detour debug draw
                duDebugDrawCylinder(dd, bmin[0], bmin[1], bmin[2], bmax[0], bmax[1], bmax[2], col);
                duDebugDrawCylinderWire(dd, bmin[0], bmin[1], bmin[2], bmax[0], bmax[1], bmax[2], duDarkenCol(col), 2);
                break;
            case DT_OBSTACLE_BOX:
                duDebugDrawBox(dd, bmin[0], bmin[1], bmin[2], bmax[0], bmax[1], bmax[2], &col);
                duDebugDrawBoxWire(dd, bmin[0], bmin[1], bmin[2], bmax[0], bmax[1], bmax[2], duDarkenCol(col), 2);
                break;
            case DT_OBSTACLE_ORIENTED_BOX:
                // There seems to be no way of debug draw oriented box in detour debug draw
                duDebugDrawCylinder(dd, bmin[0], bmin[1], bmin[2], bmax[0], bmax[1], bmax[2], col);
                duDebugDrawCylinderWire(dd, bmin[0], bmin[1], bmin[2], bmax[0], bmax[1], bmax[2], duDarkenCol(col), 2);
                break;
        }
    }
}

void Hachiko::ResourceNavMesh::DebugDraw(DebugDrawGL& dd)
{
    static const unsigned char flags = DU_DRAWNAVMESH_OFFMESHCONS | DU_DRAWNAVMESH_CLOSEDLIST;
    if (navmesh)
    {
        // Draw Navmesh
        duDebugDrawNavMeshWithClosedList(&dd, *navmesh, *navigation_query, flags);
        // Draw Obstacles
        drawObstacles(&dd, tile_cache);
        //duDebugDrawNavMeshPolysWithFlags(&dd, *navmesh, SAMPLE_POLYFLAGS_DISABLED, duRGBA(0, 0, 0, 128));
    }
}


void Hachiko::ResourceNavMesh::DrawOptionsGui()
{
    ImGui::Separator();
    Widgets::DragFloatConfig cfg;
    cfg.speed = 0.5f;
    Widgets::DragFloatConfig cfgi;
    cfgi.speed = 1.0f;
    cfgi.format = "%.f";
    cfgi.min = std::numeric_limits<int>::lowest();
    cfgi.max = std::numeric_limits<int>::max();

    ImGui::TextWrapped("Agent");
    DragFloat("Height", build_params.agent_height, &cfg);
    DragFloat("Radius", build_params.agent_radius, &cfg);
    DragFloat("Max Climb", build_params.agent_max_climb, &cfg);
    DragFloat("Max Slope", build_params.agent_max_slope, &cfg);

    ImGui::Separator();
    ImGui::TextWrapped("Rasterization");
    DragFloat("Cell Size", build_params.cell_size, &cfg);
    DragFloat("Cell Height", build_params.cell_height, &cfg);

    ImGui::Separator();
    ImGui::TextWrapped("Region");
    DragFloat("Region Min Size", build_params.region_min_size, &cfgi);
    DragFloat("Region Merge Size", build_params.region_merge_size, &cfgi);

    ImGui::Separator();
    ImGui::TextWrapped("Polygonization");
    DragFloat("Edge Max Length", build_params.edge_max_length, &cfgi);
    DragFloat("Edge Max Error", build_params.edge_max_error, &cfgi);
    DragFloat("Max Vertices Per Poly", build_params.max_vertices_per_poly, &cfgi);

    ImGui::Separator();
    ImGui::TextWrapped("Detail");
    DragFloat("Sample Distance", build_params.detail_sample_distance, &cfgi);
    DragFloat("Sample Max Error", build_params.detail_sample_max_error, &cfgi);

    if (ImGui::Button("Reset parameters", ImVec2(ImGui::GetContentRegionAvail().x, 0.0f)))
    {
        build_params = NavmeshParams();
    }
}

void Hachiko::ResourceNavMesh::CleanUp()
{
    dtFreeNavMesh(navmesh);
    navmesh = nullptr;
    dtFreeNavMeshQuery(navigation_query);
    navigation_query = nullptr;
    dtFreeCrowd(crowd);
    crowd = nullptr;
    dtFreeTileCache(tile_cache);
    tile_cache = nullptr;

    RELEASE(talloc);
    RELEASE(tcomp);
    RELEASE(tmproc);
}

void Hachiko::ResourceNavMesh::InitCrowd()
{
    // Crowd code based on CrowdTool.cpp from example

    crowd = dtAllocCrowd();
    crowd->init(MAX_AGENTS, build_params.agent_radius, navmesh);

    // Setup local avoidance params to different qualities.
    dtObstacleAvoidanceParams avoidance_params;
    // Use mostly default settings, copy from dtCrowd.
    memcpy(&avoidance_params, crowd->getObstacleAvoidanceParams(0), sizeof(dtObstacleAvoidanceParams));

    // Low (11)
    avoidance_params.velBias = 0.5f;
    avoidance_params.adaptiveDivs = 5;
    avoidance_params.adaptiveRings = 2;
    avoidance_params.adaptiveDepth = 1;
    crowd->setObstacleAvoidanceParams(0, &avoidance_params);

    // Medium (22)
    avoidance_params.velBias = 0.5f;
    avoidance_params.adaptiveDivs = 5;
    avoidance_params.adaptiveRings = 2;
    avoidance_params.adaptiveDepth = 2;
    crowd->setObstacleAvoidanceParams(1, &avoidance_params);

    // Good (45)
    avoidance_params.velBias = 0.5f;
    avoidance_params.adaptiveDivs = 7;
    avoidance_params.adaptiveRings = 2;
    avoidance_params.adaptiveDepth = 3;
    crowd->setObstacleAvoidanceParams(2, &avoidance_params);

    // High (66)
    avoidance_params.velBias = 0.5f;
    avoidance_params.adaptiveDivs = 7;
    avoidance_params.adaptiveRings = 3;
    avoidance_params.adaptiveDepth = 3;

    crowd->setObstacleAvoidanceParams(3, &avoidance_params);
}
