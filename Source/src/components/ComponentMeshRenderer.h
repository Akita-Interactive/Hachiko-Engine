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
        ~ComponentMeshRenderer() override;

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

        [[nodiscard]] const std::string& GetResourcePath() const
        {
            return asset_path;
        }

        void SetResourcePath(const std::string& path)
        {
            asset_path = path;
        }

        [[nodiscard]] const std::string& GetModelName() const
        {
            return model_name;
        }

        void SetModelName(const std::string& name)
        {
            model_name = name;
        }

        [[nodiscard]] int GetMeshIndex() const
        {
            return mesh_index;
        }

        void SetMeshIndex(int index)
        {
            mesh_index = index;
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
        
        int mesh_index;
        std::string asset_path;
        std::string model_name;
      
        // SKINING

        const GameObject** node_cache = nullptr;
        std::vector<float4x4> palette;

        ResourceMesh* mesh = nullptr;
        ResourceMaterial* material = nullptr;
    };
}
