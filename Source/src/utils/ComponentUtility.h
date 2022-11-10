#pragma once

#include <set>
#include <map>
#include <yaml-cpp/node/node.h>

#include "resources/Resource.h"

namespace Hachiko::ComponentUtility
{
    // This function checks if the resource node is defined, if so, gets its
    // UID and inserts the UID to the corresponding resource type entry set of
    // the variable "resources" if the uid turns out to be non-zero.
    inline void CollectResource(
        const Resource::Type resource_type, 
        const YAML::Node& resource_node, 
        std::map<Resource::Type, std::set<UID>>& resources)
    {
        const UID resource_id = resource_node.IsDefined()
            ? resource_node.as<UID>()
            : 0;

        if (resource_id != 0)
        {
            resources[resource_type].insert(resource_id);
        }
    }
}