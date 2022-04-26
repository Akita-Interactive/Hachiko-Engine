#pragma once

#include "resources/Resource.h"
#include <unordered_map>

#include <memory>
#include <MathGeoLibFwd.h>


namespace Hachiko
{
    class ResourceAnimation : public Resource
    {
    public:
        struct Channel
        {
            Channel() = default;
            Channel(const Channel& o) = default;
            Channel(Channel&& o) = default;
            Channel& operator=(const Channel& o) = default;
            Channel& operator=(Channel&& o) = default;

            std::unique_ptr<float3[]> positions;
            std::unique_ptr<Quat[]> rotations;
            unsigned int num_positions = 0;
            unsigned int num_rotations = 0;
        };

        struct Node
        {
            Node() = default;
            Node(const std::string name, unsigned int index_parent) : name(name), index_parent(index_parent) {}

            /* TODO: ADD transform and array od meshMat (¿point to resource?)*/

            std::string name;
            unsigned int index_parent;
        };

    public:
        ResourceAnimation(UID id);
        virtual ~ResourceAnimation();

        unsigned int GetDuration() const
        {
            return duration;
        }
        void SetDuration(unsigned int i_duration)
        {
            duration = i_duration;
        }

        // CHANEL MANAGE
        unsigned int GetNumChannels() const
        {
            return unsigned int(channels.size());
        }
        const Channel* GetChannel(const std::string& name) const;

        // NODE MANAGE
        void AddNode(const std::string& name);
        void RemoveNode(unsigned int index);
        const std::string& GetNodeName(unsigned int index) const
        {
            return nodes[index].name;
        }
        unsigned int GetNumNodes() const
        {
            return unsigned int(nodes.size());
        }

        std::unordered_map<std::string, Channel> channels;

    private:
        std::vector<Node> nodes;
        unsigned int duration = 0;
    };
} // namespace Hachiko
