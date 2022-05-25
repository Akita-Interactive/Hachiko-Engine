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
    public:
        ComponentMeshRenderer(GameObject* container, UID id = UUID::GenerateUID(), ResourceMesh* res = nullptr);
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

        void SetResourceMesh(ResourceMesh* res);

        void AddResourceMaterial(ResourceMaterial* res)
        {
            material = res;
        }

        [[nodiscard]] const ResourceMesh* GetResourceMesh() const
        {
            return mesh;
        }

        [[nodiscard]] const ResourceMaterial* GetResourceMaterial() const
        {
            return material;
        }

        void LoadMesh(UID mesh_id);
        void LoadMaterial(UID material_id);
        void DrawGui() override;

        void Save(YAML::Node& node) const override;
        void Load(const YAML::Node& node) override;

        // BONES
        void UpdateSkinPalette(std::vector<float4x4>& palette) const;

        void ChangeMaterial();

        std::vector<float4x4> palette{}; // TODO: MOVE TO PRIVATE AGAIN

        // Scripting
        [[nodiscard]] bool OverrideMaterialActive() const
        {
            return override_material;
        }
        void OverrideEmissive(float4 color, float time);

        [[nodiscard]] float4 GetOverrideEmissiveColor() const
        {
            return override_emissive;
        }

    private:
        void UpdateBoundingBoxes();
        bool visible = true;       
        bool navigable = false;

        AABB aabb;
        OBB obb;
      
        // SKINING
        const GameObject** node_cache = nullptr;
        //std::vector<float4x4> palette{};

        ResourceMesh* mesh = nullptr;
        ResourceMaterial* material = nullptr;

        // Scripting
        bool override_material = false;
        float override_timer = 0;
        float4 override_emissive;

    };
} // namespace Hachiko