#include "core/hepch.h"
#include "RenderList.h"

#include "components/ComponentCamera.h"
#include "components/ComponentMeshRenderer.h"

void Hachiko::RenderList::PreUpdate()
{
    polycount_rendered = 0;
    polycount_total = 0;
}

void Hachiko::RenderList::Update(ComponentCamera* camera, GameObject* game_object)
{
    nodes.clear();
    const Frustum* frustum = camera->GetFrustum();
    const float3 camera_pos = frustum->WorldMatrix().TranslatePart();
    CollectObjects(camera, camera_pos, game_object);
}

void Hachiko::RenderList::Update(ComponentCamera* camera, QuadtreeNode* quadtree)
{
    nodes.clear();
    const Frustum* frustum = camera->GetFrustum();
    const float3 camera_pos = frustum->WorldMatrix().TranslatePart();
    CollectObjects(camera, camera_pos, quadtree);
}

void Hachiko::RenderList::CollectObjects(ComponentCamera* camera, const float3& camera_pos, GameObject* game_object)
{
    const bool inside = camera->GetFrustum()->Intersects(game_object->GetOBB());
    if (inside)
    {
        CollectMesh(camera_pos, game_object);
    }

    for (GameObject* child : game_object->children)
        CollectObjects(camera, camera_pos, child);
}

void Hachiko::RenderList::CollectObjects(ComponentCamera* camera, const float3& camera_pos, QuadtreeNode* quadtree)
{
    const Frustum* frustum = camera->GetFrustum();
    const bool quad_inside = frustum->Intersects(quadtree->GetBox());
    // Check if node intersects camera
    if (quad_inside)
    {
        // Check any gameobjects intersect
        for (GameObject* game_object : quadtree->GetObjects())
        {
            const bool object_inside = frustum->Intersects(game_object->GetOBB());

            if (object_inside)
            {
                CollectMesh(camera_pos, game_object);
            }
        }
        // Call for all children (What to do if it is duplicated when collecting)?
        if (!quadtree->IsLeaf())
        {
            for (QuadtreeNode* child : quadtree->children)
            {
                CollectObjects(camera, camera_pos, child);
            }
        }
    }
}

void Hachiko::RenderList::CollectMesh(const float3& camera_pos, GameObject* game_object)
{
    const std::vector<Component*> components = game_object->GetComponents();
    for (int i = 0; i < components.size(); ++i)
    {
        if (components[i]->GetType() == Component::Type::MESH_RENDERER)
        {
            ComponentMeshRenderer* mesh_renderer = static_cast<ComponentMeshRenderer*>(components[i]);
            if (mesh_renderer->IsVisible())
            {
                RenderTarget target;
                target.name = game_object->GetName().c_str();
                target.mesh = mesh_renderer;
                target.distance = (game_object->GetOBB().CenterPoint() - camera_pos).LengthSq();

                // Get first element which distance is not less than current target one
                const auto it = std::lower_bound(nodes.begin(), 
                                                 nodes.end(), 
                                                 target, 
                                                 [](const RenderTarget& it_target, const RenderTarget& new_target) { 
                                                     return it_target.distance < new_target.distance; 
                                                 });
                nodes.insert(it, target);

                polycount_rendered += game_object->GetComponent<ComponentMeshRenderer>()->GetBufferSize(ResourceMesh::Buffers::INDICES) / 3;
            }
        }
    }
}
