#pragma once

#include "core/GameObject.h"
#include "components/ComponentMeshRenderer.h"

#include <MathGeoLib.h>
#include <list>

#define QUADTREE_MAX_ITEMS 8
#define QUADTREE_MIN_SIZE 50.f
#define QUADTREE_MAX_DEPTH 3

namespace Hachiko
{
    enum class Quadrants
    {
        NW = 0,
        NE,
        SE,
        SW,
        COUNT
    };

    class QuadtreeNode
    {
    public:
        QuadtreeNode(const AABB& box, QuadtreeNode* parent, int depth);
        ~QuadtreeNode();

        void Insert(ComponentMeshRenderer* mesh);
        void Remove(ComponentMeshRenderer* mesh);
        void CreateChildren();
        void RearangeChildren();

        [[nodiscard]] bool IsLeaf() const
        {
            return children[0] == nullptr;
        }

        [[nodiscard]] const AABB& GetBox() const
        {
            return box;
        }

        [[nodiscard]] const std::list<ComponentMeshRenderer*>& GetMeshes() const
        {
            return meshes;
        }

        template<typename T>
        void GetIntersections(std::vector<GameObject*>& objects, const T& primitive) const;

        QuadtreeNode* children[static_cast<int>(Quadrants::COUNT)] {};

        void DebugDraw();

    private:
        int depth = 0;
        AABB box;
        QuadtreeNode* parent =  nullptr;
        std::list<ComponentMeshRenderer*> meshes {};
    };

    class Quadtree
    {
    public:
        Quadtree();
        ~Quadtree();

        void Clear();
        void SetBox(const AABB& box);

        void Insert(ComponentMeshRenderer* mesh) const;
        void Remove(ComponentMeshRenderer* mesh) const;

        [[nodiscard]] QuadtreeNode* GetRoot() const
        {
            return root;
        }

        void DebugDraw() const;

    private:
        QuadtreeNode* root = nullptr;
    };

    template<typename T>
    void QuadtreeNode::GetIntersections(std::vector<GameObject*>& intersected, const T& primitive) const
    {
        if (primitive.Intersects(box))
        {
            float near_hit, far_hit;
            for (ComponentMeshRenderer* mesh : meshes)
            {
                if (primitive.Intersects(mesh->GetOBB(), near_hit, far_hit))
                {
                    intersected.push_back(mesh->GetGameObject());
                }
            }

            // If it has one child all exist
            if (children[0] != nullptr)
            {
                for (int i = 0; i < 4; ++i)
                {
                    children[i]->GetIntersections(intersected, primitive);
                }
            }
        }
    }
} // namespace Hachiko
