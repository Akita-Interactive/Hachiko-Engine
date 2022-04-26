#pragma once
#include "components/Component.h"

#include "resources/ResourceMesh.h"

#include <MathGeoLib.h>

namespace Hachiko
{
    class GameObject;
    class ComponentCamera;
    class Program;

    class ComponentMesh : public Component
    {
    public:
        ComponentMesh(GameObject* container, UID id = 0, ResourceMesh* res = nullptr);
        ~ComponentMesh() override = default;

        void Draw(ComponentCamera* camera, Program* program) override;
        void DrawStencil(ComponentCamera* camera, Program* program) const;

        static Type GetType()
        {
            return Type::MESH;
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
        }

        void LoadMesh(const char* mesh_path);
        void LoadMesh(UID mesh_id);
        void DrawGui() override;

        void Save(YAML::Node& node) const override;
        void Load(const YAML::Node& node) override;

    private:
        bool visible = true;
        int mesh_index;
        std::string asset_path;
        std::string model_name;
        ResourceMesh* mesh = nullptr;
    };
}
