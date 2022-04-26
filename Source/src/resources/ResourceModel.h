#pragma once

#include "Resource.h"

namespace Hachiko
{
    struct ResourceNode
    {
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
        std::vector<ResourceNode*> children {};
    };

    struct MeshInfo
    {
        UID mesh_id;
        int material_index;
    };

    struct MaterialInfo
    {
        UID material_id;
        std::string material_name;
    };

    class ResourceModel final : public Resource
    {
    public:
        ResourceModel() : Resource(Resource::Type::MODEL){};
        ResourceModel(UID uid);
        ~ResourceModel() override;

        std::vector<MeshInfo> meshes {};
        std::vector<MaterialInfo> materials {};

        std::vector<ResourceNode*> child_nodes;
        std::string model_path;
        std::string material_path;
        std::string model_name;
    };
}
