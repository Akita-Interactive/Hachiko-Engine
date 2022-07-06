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

void Hachiko::RenderList::Update(ComponentCamera* camera, Quadtree* quadtree)
{
    opaque_targets.clear();
    transparent_targets.clear();
    const float3 camera_pos = camera->GetGameObject()->GetTransform()->GetGlobalPosition();
    CollectMeshes(camera, camera_pos, quadtree);
}

void Hachiko::RenderList::CollectMeshes(ComponentCamera* camera, const float3& camera_pos, Quadtree* quadtree)
{
    const Frustum* frustum = camera->GetFrustum();

    std::vector<ComponentMeshRenderer*> meshes;

    quadtree->GetIntersections(meshes, *frustum);

    for (ComponentMeshRenderer* mesh_renderer : meshes)
    {
        CollectMesh(camera_pos, mesh_renderer);
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
    if (mesh_renderer->GetResourceMaterial()->is_transparent)
    {
        // Get sorted by distance to camera in ascending order:
        const auto it = std::lower_bound(transparent_targets.begin(), transparent_targets.end(), target, 
            [](const RenderTarget& it_target, const RenderTarget& new_target) 
            { 
                return it_target.distance > new_target.distance; 
            });

        transparent_targets.insert(it, target);
    }
    else
    {
        // Get sorted by distance to camera in ascending order:
        const auto it = std::lower_bound(opaque_targets.begin(), opaque_targets.end(), target, 
            [](const RenderTarget& it_target, const RenderTarget& new_target) 
            { 
                return it_target.distance < new_target.distance; 
            });

        opaque_targets.insert(it, target);
    }
}
