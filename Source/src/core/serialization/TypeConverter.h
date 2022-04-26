#pragma once

#include <yaml-cpp/yaml.h>
#include <Math/float2.h>
#include <Math/float3.h>
#include <Math/float4.h>
#include <Math/Quat.h>
#include <Math/float4x4.h>
#include <assimp/vector3.h>
#include "utils/UUID.h"

namespace YAML
{
    template<>
    struct convert<float2>
    {
        static Node encode(const float2& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, float2& rhs)
        {
            if (!node.IsSequence() || node.size() != 2)
            {
                return false;
            }

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            return true;
        }
    };

    template<>
    struct convert<float3>
    {
        static Node encode(const float3& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, float3& rhs)
        {
            if (!node.IsSequence() || node.size() != 3)
            {
                return false;
            }

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };

    template<>
    struct convert<float4>
    {
        static Node encode(const float4& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, float4& rhs)
        {
            if (!node.IsSequence() || node.size() != 4)
            {
                return false;
            }

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };

    template<>
    struct convert<Quat>
    {
        static Node encode(const Quat& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, Quat& rhs)
        {
            if (!node.IsSequence() || node.size() != 4)
            {
                return false;
            }

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };

    template<>
    struct convert<float4x4>
    {
        static Node encode(const float4x4& rhs)
        {
            Node node;

            node.push_back(rhs.scaleX);
            node.push_back(rhs.shearXy);
            node.push_back(rhs.shearXz);
            node.push_back(rhs.x);

            node.push_back(rhs.shearYx);
            node.push_back(rhs.scaleY);
            node.push_back(rhs.shearYz);
            node.push_back(rhs.y);

            node.push_back(rhs.shearZx);
            node.push_back(rhs.shearZy);
            node.push_back(rhs.scaleZ);
            node.push_back(rhs.z);

            node.push_back(rhs.shearWx);
            node.push_back(rhs.shearWy);
            node.push_back(rhs.shearWz);
            node.push_back(rhs.w);

            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, float4x4& rhs)
        {
            if (!node.IsSequence() || 
                (node[0].size() != 4 && 
                 node[1].size() != 4 &&
                 node[2].size() != 4 &&
                 node[3].size() != 4))
            {
                return false;
            }

            rhs.scaleX  = node[0][0].as<float>();
            rhs.shearXy = node[0][1].as<float>();
            rhs.shearXz = node[0][2].as<float>();
            rhs.x       = node[0][3].as<float>();

            rhs.shearYx = node[1][0].as<float>();
            rhs.scaleY  = node[1][1].as<float>();
            rhs.shearYz = node[1][2].as<float>();
            rhs.y       = node[1][3].as<float>();

            rhs.shearZx = node[2][0].as<float>();
            rhs.shearZy = node[2][1].as<float>();
            rhs.scaleZ  = node[2][2].as<float>();
            rhs.z       = node[2][3].as<float>();

            rhs.shearWx = node[3][0].as<float>();
            rhs.shearWy = node[3][1].as<float>();
            rhs.shearWz = node[3][2].as<float>();
            rhs.w       = node[3][3].as<float>();

            return true;
        }
    };

    template<>
    struct convert<aiVector3D>
    {
        static Node encode(const aiVector3D& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, aiVector3D& rhs)
        {
            if (!node.IsSequence() || node.size() != 3)
            {
                return false;
            }

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };

    template<>
    struct convert<aiMatrix4x4>
    {
        static Node encode(const aiMatrix4x4& rhs)
        {
            Node node;
            node[0].push_back(static_cast<float>(rhs.a1));
            node[0].push_back(static_cast<float>(rhs.a2));
            node[0].push_back(static_cast<float>(rhs.a3));
            node[0].push_back(static_cast<float>(rhs.a4));
            
            node[1].push_back(static_cast<float>(rhs.b1));
            node[1].push_back(static_cast<float>(rhs.b2));
            node[1].push_back(static_cast<float>(rhs.b3));
            node[1].push_back(static_cast<float>(rhs.b4));
            
            node[2].push_back(static_cast<float>(rhs.c1));
            node[2].push_back(static_cast<float>(rhs.c2));
            node[2].push_back(static_cast<float>(rhs.c3));
            node[2].push_back(static_cast<float>(rhs.c4));
            
            node[3].push_back(static_cast<float>(rhs.d1));
            node[3].push_back(static_cast<float>(rhs.d2));
            node[3].push_back(static_cast<float>(rhs.d3));
            node[3].push_back(static_cast<float>(rhs.d4));
            
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, aiMatrix4x4& rhs)
        {
            if (!node.IsSequence() || node.size() != 3)
            {
                return false;
            }

            rhs.a1 = node[0][0].as<float>();
            rhs.a2 = node[0][1].as<float>();
            rhs.a3 = node[0][2].as<float>();
            rhs.a4 = node[0][3].as<float>();

            rhs.b1 = node[1][0].as<float>();
            rhs.b2 = node[1][1].as<float>();
            rhs.b3 = node[1][2].as<float>();
            rhs.b4 = node[1][3].as<float>();

            rhs.c1 = node[2][0].as<float>();
            rhs.c2 = node[2][1].as<float>();
            rhs.c3 = node[2][2].as<float>();
            rhs.c4 = node[2][3].as<float>();

            rhs.d1 = node[3][0].as<float>();
            rhs.d2 = node[3][1].as<float>();
            rhs.d3 = node[3][2].as<float>();
            rhs.d4 = node[3][3].as<float>();

            return true;
        }
    };
} // namespace YAML
