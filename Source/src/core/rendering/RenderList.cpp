#include "core/hepch.h"
#include "RenderList.h"

#include "components/ComponentTransform.h"
#include "components/ComponentCamera.h"
#include "components/ComponentMeshRenderer.h"

void Hachiko::RenderList::PreUpdate()
{
    polycount_rendered = 0;
    polycount_total = 0;
}

void Hachiko::RenderList::Update(const Frustum& camera_frustum, Quadtree* quadtree)
{
    opaque_targets.clear();
    transparent_targets.clear();
    CollectMeshes(camera_frustum, quadtree);
}

void Hachiko::RenderList::CollectMeshes(const Frustum& camera_frustum, Quadtree* quadtree)
{
    std::vector<ComponentMeshRenderer*> meshes;

    quadtree->GetIntersections(meshes, camera_frustum);

    float3 camera_position = camera_frustum.Pos();

    for (ComponentMeshRenderer* mesh_renderer : meshes)
    {
        CollectMesh(camera_position, mesh_renderer);
    }
}

void Hachiko::RenderList::CollectMesh(const float3& camera_pos, ComponentMeshRenderer* mesh_renderer)
{
    if (!mesh_renderer || !mesh_renderer->GetGameObject()->IsActive() || !mesh_renderer->IsVisible())
    {
        return;
    }

    GameObject* game_object = mesh_renderer->GetGameObject();
    
    RenderTarget target;
    target.name = game_object->GetName().c_str();
    target.mesh_id = mesh_renderer->GetID();
    target.mesh_renderer = mesh_renderer;
    target.distance = mesh_renderer->GetOBB().CenterPoint().DistanceSqD(camera_pos);

    // Decide in which list this target should be placed in based on its
    // material's transparency:
    auto targets = mesh_renderer->GetResourceMaterial()->is_transparent 
        ? &transparent_targets 
        : &opaque_targets;

    // Get sorted by distance to camera in ascending order:
    const auto it = std::lower_bound(
        (*targets).begin(), (*targets).end(), target, 
        [](const RenderTarget& it_target, const RenderTarget& new_target) 
        { 
            return it_target.distance < new_target.distance; 
        });

    (*targets).insert(it, target);
}
