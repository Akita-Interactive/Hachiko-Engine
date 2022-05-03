#pragma once

#include "Resource.h"

namespace Hachiko
{
    struct ResourceNode
    {
        ResourceNode() = default;

        ~ResourceNode()
        {
            for (auto child : children)
            {
                RELEASE(child);
            }
        }

        std::string node_name;
        float4x4 node_transform;
        std::vector<int> meshes_index;
        std::vector<ResourceNode*> children{};

        ResourceNode(const ResourceNode& other) :
            node_name(other.node_name),
            node_transform(other.node_transform),
            meshes_index(other.meshes_index)
        {
            for (const auto child : other.children)
            {
                children.push_back(new ResourceNode(*child));
            }
        }

        ResourceNode& operator=(const ResourceNode& other)
        {
            if (this == &other)
                return *this;
            node_name = other.node_name;
            node_transform = other.node_transform;
            meshes_index = other.meshes_index;
            children = other.children;
            return *this;
        }
    };


    struct MeshInfo
    {
        UID mesh_id;
        int material_index;

        MeshInfo() = default;

        MeshInfo(const MeshInfo& other) = default;

        MeshInfo& operator=(const MeshInfo& other)
        {
            if (this == &other)
            {
                return *this;
            }
            mesh_id = other.mesh_id;
            material_index = other.material_index;
            return *this;
        }
    };

    struct MaterialInfo
    {
        UID material_id;
        std::string material_name;

        MaterialInfo() = default;
        MaterialInfo(const MaterialInfo& other) = default;

        MaterialInfo& operator=(const MaterialInfo& other)
        {
            if (this == &other)
            {
                return *this;
            }
            material_id = other.material_id;
            material_name = other.material_name;
            return *this;
        }
    };

    struct AnimationInfo
    {
        UID animation_id;
        std::string animation_name;
    };

    class ResourceModel final : public Resource
    {
    public:
        ResourceModel() :
            Resource(Resource::Type::MODEL)
        {
        }

        ResourceModel(UID uid);
        ~ResourceModel() override;

        std::vector<MeshInfo> meshes {};
        std::vector<MaterialInfo> materials {};
        std::vector<AnimationInfo> animations {};

        unsigned int have_animation = 0;

        std::vector<ResourceNode*> child_nodes{};
        std::string model_path;
        std::string material_path;
        std::string model_name;

    public:
        CLONE_RESOURCE(ResourceModel)

        ResourceModel(const ResourceModel& other) :
            Resource(other),
            meshes(other.meshes),
            materials(other.materials),
            model_path(other.model_path),
            material_path(other.material_path),
            model_name(other.model_name)
        {
            for (const auto child_node : other.child_nodes)
            {
                child_nodes.push_back(new ResourceNode(*child_node));
            }
        }

        ResourceModel& operator=(const ResourceModel& other)
        {
            if (this == &other)
            {
                return *this;
            }
            Resource::operator =(other);
            meshes = other.meshes;
            materials = other.materials;
            child_nodes = other.child_nodes;
            model_path = other.model_path;
            material_path = other.material_path;
            model_name = other.model_name;
            return *this;
        }
    };
}
