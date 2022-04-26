#pragma once

#include "components/Component.h"
#include "Globals.h"

namespace Hachiko
{
    class GameObject;

    class ComponentCanvas;

    class HACHIKO_API ComponentTransform2D : public Component
    {
    public:
        ComponentTransform2D(GameObject* container);
        ~ComponentTransform2D() override = default;

        static Type GetType();

        void DrawGui() override;
        void DebugDraw() override;

        void SetPosition(float3 position);

        void SetSize(float2 size);
        void SetScale(float2 scale);

        void SetRotation(Quat rotation);
        void SetRotation(float3 rotation);

        void SetPivot(float2 pivot_position);
        void SetAnchor(float2 anchor_position);

        [[nodiscard]] float3 GetPosition() const
        {
            return position;
        }

        [[nodiscard]] float2 GetSize() const
        {
            return size;
        }

        [[nodiscard]] float3 GetScale() const
        {
            return scale;
        }
        
        [[nodiscard]] float3 GetPivotOffsetFromParent() const;
        [[nodiscard]] float3 GetPivotScreenPosition() const;

        [[nodiscard]] float4x4 GetGlobalTransform();
        [[nodiscard]] float4x4 GetGlobalScaledTransform();

        bool HasDependentComponents(GameObject* game_object) const override;
        bool Intersects(const float2& mouse_pos) const;

        void Save(YAML::Node& node) const override;
        void Load(const YAML::Node& node) override;

    private:
        void Invalidate(); // Set this and children to dirty
        void UpdateTransforms(); // Only does oeprations triggers class is dirty
        void UpdateUIComponents(); // Called when the transforms change
        void UpdateBoundingBox();


    private:
        const float default_size = 20.f;
        float3 position = float3::zero; // Offset Position relative to pivot

        float2 size = float2(default_size); // Size of the UI item
        float3 scale = float3::one; // Scaling factor for size, usually one

        Quat rotation = Quat::identity;
        float3 rotation_euler = float3::zero;

        float2 pivot_pct_position = float2(0.5f); // Percent position of the pivot in 2D, can surpass the 0-1 range

        float2 anchor_pct_position = float2(0.5f); // Percent position of the anchor point relative to the parent transform 2D

        // Check if needed (GO already has his own)
        float4x4 local_transform = float4x4::identity;
        float4x4 global_transform = float4x4::identity;
        
        // Used to check click intersections
        AABB2D aabb = {{0, 0}, {0, 0}};

        bool has_canvas = false;

        // Dirty flag pattern
        bool dirty = true;
    };
}

inline Hachiko::Component::Type Hachiko::ComponentTransform2D::GetType()
{
    return Component::Type::TRANSFORM_2D;
}

