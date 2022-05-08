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
    public:
        ComponentMeshRenderer(GameObject* container, UID id = 0, ResourceMesh* res = nullptr);
        ~ComponentMeshRenderer() override = default;

        void Update() override;
        void Draw(ComponentCamera* camera, Program* program) override;
        void DrawStencil(ComponentCamera* camera, Program* program) const;

        static Type GetType()
        {
            return Type::MESH_RENDERER;
        }

        [[nodiscard]] bool IsLoaded() const
        {
            return mesh != nullptr;
        }

        [[nodiscard]] bool IsVisible() const
        {
            return visible;
        }

        [[nodiscard]] AABB GetAABB() const
        {
            return mesh->bounding_box;
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

        [[nodiscard]] const ResourceMesh* GetResourceMesh() const
        {
            return mesh;
        }

        [[nodiscard]] const ResourceMaterial* GetResourceMaterial() const
        {
            return material;
        }
       
        void AddResourceMesh(ResourceMesh* res)
        {
            mesh = res;
        }

        void AddResourceMaterial(ResourceMaterial* res)
        {
            material = res;
        }

        void LoadMesh(UID mesh_id);
        void LoadMaterial(UID material_id);
        void DrawGui() override;

        void Save(YAML::Node& node) const override;
        void Load(const YAML::Node& node) override;

        // BONES
        void UpdateSkinPalette(std::vector<float4x4>& palette) const;

        void ChangeMaterial();

    private:
        bool visible = true;
              
        // SKINING
        const GameObject** node_cache = nullptr;
        std::vector<float4x4> palette;

        ResourceMesh* mesh = nullptr;
        ResourceMaterial* material = nullptr;
    };
} // namespace Hachiko