#pragma once

#include "yaml-cpp/yaml.h"

namespace Hachiko
{
    class ISerializable
    {
    public:
        virtual ~ISerializable() = default;
        virtual void Save(YAML::Node&) const = 0;
        virtual void Load(const YAML::Node&) = 0;
    };
}
