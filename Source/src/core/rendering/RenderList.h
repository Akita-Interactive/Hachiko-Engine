#pragma once

#include "Outline.h"

#include <MathGeoLib.h>
#include <vector>

namespace Hachiko
{
    class GameObject;
    class ComponentMeshRenderer;
    class ComponentCamera;
    class Quadtree;
    class QuadtreeNode;

    struct RenderTarget
    {
        const char* name = nullptr;
        unsigned long long mesh_id = 0;
        ComponentMeshRenderer* mesh_renderer = nullptr;
        double distance = 0.0;
    };

    class RenderList
    {
    public:
        void PreUpdate();
        void Update(const Frustum& camera_frustum, Quadtree* quadtree);

        std::vector<RenderTarget>& GetOpaqueTargets()
        {
            return opaque_targets;
        }

        std::vector<RenderTarget>& GetTransparentTargets()
        {
            return transparent_targets;
        }

        [[nodiscard]] unsigned GetPolycountRendered() const
        {
            return polycount_rendered;
        }

        [[nodiscard]] unsigned GetPolycountTotal() const
        {
            return polycount_total;
        }

        // NOTE: This is only valid for non-NONE types of outlines.
        [[nodiscard]] std::vector<RenderTarget>& GetOutlineTargets(
            Outline::Type outline_type)
        {
            return outline_targets[Outline::TypeToInt(outline_type)];
        }

    private:
        void CollectMeshes(const Frustum& camera_frustum, Quadtree* quadtree);
        void CollectMesh(const float3& camera_pos, ComponentMeshRenderer* mesh);

        std::vector<RenderTarget> opaque_targets;
        std::vector<RenderTarget> transparent_targets;

        std::vector<RenderTarget> outline_targets[Outline::GetEnabledTypeCount()];

        unsigned polycount_rendered = 0, polycount_total = 0;
    };
}
