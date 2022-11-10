#pragma once
#include "components/Component.h"

#include "resources/ResourceMesh.h"
#include "resources/ResourceMaterial.h"

#include <MathGeoLib.h>

namespace Hachiko
{
    class GameObject;
    class ComponentCamera;
    class Program;

    class ComponentMeshRenderer : public Component
    {
        friend class ModelImporter;
        friend class Scene;
    public:
        ComponentMeshRenderer(GameObject* container);
        ~ComponentMeshRenderer() override;

        void Update() override;
        void Draw(ComponentCamera* camera, Program* program) override;
        void DrawStencil(ComponentCamera* camera, Program* program) const;

        void DebugDraw() override;

        void OnTransformUpdated() override;

        [[nodiscard]] bool IsLoaded() const
        {
            return mesh != nullptr;
        }

        [[nodiscard]] bool IsVisible() const
        {
            return visible;
        }

        void SetVisible(bool v)
        {
            visible = v;
        }

        [[nodiscard]] bool IsNavigable() const
        {
            return navigable;
        }

        [[nodiscard]] AABB GetMeshAABB() const
        {
            return mesh->bounding_box;
        }

        [[nodiscard]] const OBB& GetOBB() const
        {
            return obb;
        }

        const AABB& GetAABB()
        {
            return aabb;
        }

        [[nodiscard]] unsigned GetBufferSize(ResourceMesh::Buffers buffer) const
        {
            return mesh->buffer_sizes[static_cast<int>(buffer)];
        }

        [[nodiscard]] unsigned GetBufferId(ResourceMesh::Buffers buffer) const
        {
            return mesh->buffer_ids[static_cast<int>(buffer)];
        }

        [[nodiscard]] const float* GetVertices() const
        {
            return mesh->vertices;
        }

        [[nodiscard]] const unsigned* GetIndices() const
        {
            return mesh->indices;
        }

        [[nodiscard]] const float* GetNormals() const
        {
            return mesh->normals;
        }

        [[nodiscard]] bool GetOverrideMaterialFlag() const
        {
            return override_emissive_flag_override;
        }

        [[nodiscard]] const ResourceMesh* GetResourceMesh() const
        {
            return mesh;
        }

        [[nodiscard]] const ResourceMaterial* GetResourceMaterial() const
        {
            return material;
        }

        HACHIKO_API void SetTintColor(const float4& color)
        {
            tint_color = color;
        }

        HACHIKO_API  [[nodiscard]] const float4& GetTintColor() const
        {
            return tint_color;
        }

        HACHIKO_API void SetDissolveProgress(const float progress)
        {
            dissolve_progress = progress;
        }

        HACHIKO_API [[nodiscard]] float GetDissolveProgress() const
        {
            return dissolve_progress;
        }

        HACHIKO_API [[nodiscard]] bool IsCastingShadow() const
        {
            return is_casting_shadow;
        }

        HACHIKO_API void SetCastingShadow(bool value);

        HACHIKO_API [[nodiscard]] bool IsOutlined() const
        {
            return outline_type != Outline::Type::NONE;
        }

        HACHIKO_API [[nodiscard]] Outline::Type GetOutlineType() const
        {
            return outline_type;
        }

        HACHIKO_API void SetOutlineType(Outline::Type type)
        {
            outline_type = type;
        }

        void DrawGui() override;

        void Save(YAML::Node& node) const override;
        void Load(const YAML::Node& node) override;
        static void CollectResources(const YAML::Node& node, std::map<Resource::Type, std::set<UID>>& resources);

        // BONES
        [[nodiscard]] const std::vector<float4x4>& GetPalette() const
        {
            return palette;
        }

        // Scripting
        HACHIKO_API [[nodiscard]] bool OverrideMaterialActive() const
        {
            return override_material;
        }

        HACHIKO_API void OverrideEmissive(const float4& color, bool override_flag = false);
        HACHIKO_API void LiftOverrideEmissive();

        HACHIKO_API [[nodiscard]] const float4& GetOverrideEmissiveColor() const
        {
            return override_emissive;
        }

    private:
        void LoadMesh(UID mesh_id);
        void LoadMaterial(UID material_id);
        void SetMeshResource(ResourceMesh* res);
        void SetMaterialResource(ResourceMaterial* res);

        void UpdateSkinPalette(std::vector<float4x4>& palette) const;
        void ChangeMaterial();

        void UpdateBoundingBoxes();
        bool visible = true;
        bool navigable = false;
        bool is_casting_shadow = true;
        Outline::Type outline_type = Outline::Type::NONE;

        AABB aabb;
        OBB obb;

        // SKINING
        const GameObject** node_cache = nullptr;

        std::vector<float4x4> palette{};

        ResourceMesh* mesh = nullptr;
        ResourceMaterial* material = nullptr;
        float4 tint_color = float4::one;
        float dissolve_progress = 1.0f; // Not stored (min 0, max 1)

        // Scripting
        bool override_material = false;
        float override_timer = 0;
        float4 override_emissive;
        bool override_emissive_flag_override = false;
    };
} // namespace Hachiko
