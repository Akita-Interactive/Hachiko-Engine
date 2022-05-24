#include "core/hepch.h"
#include "Quadtree.h"
#include <debugdraw.h>

Hachiko::QuadtreeNode::QuadtreeNode(const AABB& box, QuadtreeNode* parent, int depth) : depth(depth), box(box), parent(parent)
{
    children[static_cast<int>(Quadrants::NW)] = children[static_cast<int>(Quadrants::NE)] = children[static_cast<int>(Quadrants::SE)] = children[static_cast<int>(Quadrants::SW)] = nullptr;
}

Hachiko::QuadtreeNode::~QuadtreeNode()
{
    RELEASE(children[(int)Quadrants::NW]);
    RELEASE(children[(int)Quadrants::NE]);
    RELEASE(children[(int)Quadrants::SE]);
    RELEASE(children[(int)Quadrants::SW]);
}

void Hachiko::QuadtreeNode::Insert(ComponentMeshRenderer* mesh)
{
    meshes.push_back(mesh);

    if (depth >= QUADTREE_MAX_DEPTH)
    {
        return;
    }

    // No split due to not enough objects
    if (IsLeaf() && meshes.size() < QUADTREE_MAX_ITEMS)
    {
        return;
    }

    // No split due to minimum size
    if (box.HalfSize().LengthSq() <= (QUADTREE_MIN_SIZE * QUADTREE_MIN_SIZE))
    {
        return;
    }
    // Rearrange children
    if (IsLeaf())
    {
        CreateChildren();
    }

    RearangeChildren();
}

void Hachiko::QuadtreeNode::Remove(ComponentMeshRenderer* mesh)
{
    // TODO: Reduce size when no longer necessary?
    const auto find_iter = std::find(meshes.begin(), meshes.end(), mesh);
    if (find_iter != meshes.end())
    {
        meshes.erase(find_iter);
    }

    if (!IsLeaf())
    {
        for (const auto& child : children)
        {
            child->Remove(mesh);
        }
    }
}

void Hachiko::QuadtreeNode::CreateChildren()
{
    HE_LOG("Create Children");
    // Subdivide current box
    const auto size = float3(box.Size());
    const float3 center = box.CenterPoint();
    // Do not operate y, 2d
    const auto child_size = float3(0.5f * size.x, size.y, 0.5f * size.z);
    // Recalculate for each child
    float3 child_center(center);
    AABB child_box;

    int new_detph = depth + 1;

    // NW
    child_center.x = center.x - size.x * 0.25f;
    child_center.z = center.z + size.z * 0.25f;
    child_box.SetFromCenterAndSize(child_center, child_size);
    children[static_cast<int>(Quadrants::NW)] = new QuadtreeNode(child_box, this, new_detph);

    // NE
    child_center.x = center.x + size.x * 0.25f;
    child_center.z = center.z + size.z * 0.25f;
    child_box.SetFromCenterAndSize(child_center, child_size);
    children[static_cast<int>(Quadrants::NE)] = new QuadtreeNode(child_box, this, new_detph);

    // SE
    child_center.x = center.x + size.x * 0.25f;
    child_center.z = center.z - size.z * 0.25f;
    child_box.SetFromCenterAndSize(child_center, child_size);
    children[static_cast<int>(Quadrants::SE)] = new QuadtreeNode(child_box, this, new_detph);

    // SW
    child_center.x = center.x - size.x * 0.25f;
    child_center.z = center.z - size.z * 0.25f;
    child_box.SetFromCenterAndSize(child_center, child_size);
    children[static_cast<int>(Quadrants::SW)] = new QuadtreeNode(child_box, this, new_detph);
}

void Hachiko::QuadtreeNode::RearangeChildren()
{
    //HE_LOG("Rearange Children %s", this);
    for (auto it = meshes.begin(); it != meshes.end();)
    {
        ComponentMeshRenderer* mesh = *it;
        int intersection_count = 0;
        bool intersects[static_cast<int>(Quadrants::COUNT)];
        for (int i = 0; i < static_cast<int>(Quadrants::COUNT); ++i)
        {
            intersects[i] = children[i]->box.Intersects(mesh->GetAABB());
            if (intersects[i])
            {
                intersection_count += 1;
            }
        }

        // If it intersects all there is no point in moving downwards
        //if (intersects[static_cast<int>(Quadrants::NW)] && intersects[static_cast<int>(Quadrants::NE)] && intersects[static_cast<int>(Quadrants::SE)] && intersects[static_cast<int>(Quadrants::SW)])
        if (intersection_count > 1)
        {
            ++it;
            continue;
        }
        it = meshes.erase(it);
        for (int i = 0; i < static_cast<int>(Quadrants::COUNT); ++i)
        {
            if (intersects[i])
            {
                children[i]->Insert(mesh);
            }
        }
    }
}

void Hachiko::QuadtreeNode::DebugDraw()
{
    static const int order[8] = {0, 1, 5, 4, 2, 3, 7, 6};
    if (IsLeaf())
    {
        ddVec3 p[8];

        for (int i = 0; i < 8; ++i)
        {
            p[i] = box.CornerPoint(order[i]);
        }

        dd::box(p, dd::colors::LawnGreen);
        return;
    }

    for (QuadtreeNode* child : children)
    {
        child->DebugDraw();
    }
}

Hachiko::Quadtree::Quadtree() = default;

Hachiko::Quadtree::~Quadtree()
{
    Clear();
}

void Hachiko::Quadtree::Clear()
{
    RELEASE(root);
}

void Hachiko::Quadtree::SetBox(const AABB& box)
{
    Clear();
    root = new QuadtreeNode(box, nullptr, 0);
}

void Hachiko::Quadtree::Insert(ComponentMeshRenderer* mesh) const
{
    if (root)
    {
        root->Insert(mesh);
    }
}

void Hachiko::Quadtree::Remove(ComponentMeshRenderer* mesh) const
{
    if (root)
    {
        root->Remove(mesh);
    }
}

void Hachiko::Quadtree::DebugDraw() const
{
    if (root)
    {
        root->DebugDraw();
    }
}