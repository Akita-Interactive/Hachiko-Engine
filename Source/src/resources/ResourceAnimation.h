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

    public:
        ResourceAnimation(UID id);
        virtual ~ResourceAnimation();

        // RESOURCE INFO
        [[nodiscard]] std::string GetName() const
        {
            return name;
        }
        void SetName(const char* new_name)
        {
            name = new_name;
        }

        // ANIMATION CONTROLL
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


        std::unordered_map<std::string, Channel> channels;

    private:
        //std::vector<Node> nodes;
        unsigned int duration = 0;

        std::string name;
    };
} // namespace Hachiko
