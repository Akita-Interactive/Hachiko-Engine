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

        void AddResourceMesh(ResourceMesh* res)
        {
            mesh = res;

            if (mesh->num_bones > 0)
            {
                node_cache = new const GameObject*[mesh->num_bones];

                for (unsigned int i = 0; i < mesh->num_bones; ++i)
                {
                    node_cache[i] = nullptr;
                }
            }
        }

        [[nodiscard]] const ResourceMaterial* GetMaterial() const
        {
            return material;
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
        bool navigable = false;        
      
        // SKINING
        const GameObject** node_cache = nullptr;
        std::vector<float4x4> palette;

        ResourceMesh* mesh = nullptr;
        ResourceMaterial* material = nullptr;

    public:
        CLONE_COMPONENT(ComponentMesh)

        ComponentMesh(const ComponentMesh& other) :
            Component(other),
            visible(other.visible),
            // node_cache(other.node_cache),
            mesh_index(other.mesh_index),
            asset_path(other.asset_path),
            model_name(other.model_name)

        {
            mesh = new ResourceMesh(*other.mesh);
            material = new ResourceMaterial(*other.material);
        }

        ComponentMesh& operator=(const ComponentMesh& other)
        {
            if (this == &other)
            {
                return *this;
            }
            Component::operator =(other);
            visible = other.visible;
            node_cache = other.node_cache;
            mesh_index = other.mesh_index;
            asset_path = other.asset_path;
            model_name = other.model_name;
            mesh = other.mesh;
            return *this;
        }
    };
}
