#pragma once

#include <yaml-cpp/yaml.h>
#include <Math/float2.h>
#include <Math/float3.h>
#include <Math/float4.h>
#include <Math/Quat.h>
#include <Math/float4x4.h>
#include <assimp/vector3.h>
#include <assimp/matrix4x4.h>
#include <imgui_color_gradient.h>
#include "utils/UUID.h"
#include "core/particles/ParticleSystem.h"

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

            node.push_back(rhs[0][0]);
            node.push_back(rhs[0][1]);
            node.push_back(rhs[0][2]);
            node.push_back(rhs[0][3]);

            node.push_back(rhs[1][0]);
            node.push_back(rhs[1][1]);
            node.push_back(rhs[1][2]);
            node.push_back(rhs[1][3]);

            node.push_back(rhs[2][0]);
            node.push_back(rhs[2][1]);
            node.push_back(rhs[2][2]);
            node.push_back(rhs[2][3]);

            node.push_back(rhs[3][0]);
            node.push_back(rhs[3][1]);
            node.push_back(rhs[3][2]);
            node.push_back(rhs[3][3]);

            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, float4x4& rhs)
        {
            if (!node.IsSequence() || 
                node.size() != 16)
            {
                return false;
            }

            rhs[0][0] = node[0].as<float>();
            rhs[0][1] = node[1].as<float>();
            rhs[0][2] = node[2].as<float>();
            rhs[0][3] = node[3].as<float>();

            rhs[1][0] = node[4].as<float>();
            rhs[1][1] = node[5].as<float>();
            rhs[1][2] = node[6].as<float>();
            rhs[1][3] = node[7].as<float>();

            rhs[2][0] = node[8].as<float>();
            rhs[2][1] = node[9].as<float>();
            rhs[2][2] = node[10].as<float>();
            rhs[2][3] = node[11].as<float>();

            rhs[3][0] = node[12].as<float>();
            rhs[3][1] = node[13].as<float>();
            rhs[3][2] = node[14].as<float>();
            rhs[3][3] = node[15].as<float>();

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
    
    template<>
    struct convert<FILETIME>
    {
        static Node encode(const FILETIME& rhs)
        {
            Node node;
            node.push_back(static_cast<DWORD>(rhs.dwLowDateTime));
            node.push_back(static_cast<DWORD>(rhs.dwHighDateTime));

            node.SetStyle(EmitterStyle::Flow);
            return node;
        }
        
        static bool decode(const Node& node, FILETIME& rhs)
        {
            if (!node.IsSequence() || node.size() != 2)
            {
                return false;
            }

            rhs.dwLowDateTime = node[0].as<DWORD>();
            rhs.dwHighDateTime = node[1].as<DWORD>();

            return true;
        }
    };

    template<>
    struct convert<ImGradient>
    {
        static Node encode(const ImGradient& rhs)
        {
            Node node;
            for (auto mark : rhs.getMarks())
            {
                node.push_back(*mark);
            }

            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, ImGradient& rhs)
        {
            // Not able to decode. Use ImGradientMark's decode instead and ImGradient::addMark
            return false;
        }
    };

    template<>
    struct convert<ImGradientMark>
    {
        static Node encode(const ImGradientMark& rhs)
        {
            Node node;

            node.push_back(rhs.color[0]);
            node.push_back(rhs.color[1]);
            node.push_back(rhs.color[2]);
            node.push_back(rhs.color[3]);
            node.push_back(rhs.position);

            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, ImGradientMark& rhs)
        {
            if (!node.IsSequence() || node.size() != 5)
            {
                return false;
            }

            rhs.color[0] = node[0].as<float>();
            rhs.color[1] = node[1].as<float>();
            rhs.color[2] = node[2].as<float>();
            rhs.color[3] = node[3].as<float>();
            rhs.position = node[4].as<float>();
            return true;
        }
    };

    template<>
    struct convert<Hachiko::ParticleSystem::VariableTypeProperty>
    {
        static Node encode(const Hachiko::ParticleSystem::VariableTypeProperty& rhs)
        {
            Node node;

            node.push_back(rhs.constant_enabled);
            // TODO: Add curve serialization
            node.push_back(rhs.curve_enabled);
            node.push_back(rhs.selected);
            node.push_back(static_cast<int>(rhs.selected_option));
            node.push_back(rhs.values);

            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, Hachiko::ParticleSystem::VariableTypeProperty& rhs)
        {
            if (!node.IsSequence() || node.size() != 5)
            {
                return false;
            }

            rhs.constant_enabled = node[0].as<bool>();
            // TODO: Add curve serialization
            rhs.curve_enabled = node[1].as<bool>();
            rhs.selected = node[2].as<bool>();
            rhs.selected_option = static_cast<Hachiko::ParticleSystem::Selection>(node[3].as<int>());
            rhs.values = node[4].as<float2>();
            return true;
        }
    };
    
    template<>
    struct convert<Hachiko::ParticleSystem::Emitter::Properties>
    {
        static Node encode(const Hachiko::ParticleSystem::Emitter::Properties& rhs)
        {
            Node node;
            node.push_back(rhs.arc);
            node.push_back(static_cast<int>(rhs.emit_from));
            node.push_back(rhs.position);
            node.push_back(rhs.radius);
            node.push_back(rhs.radius_thickness);
            node.push_back(rhs.rotation);
            node.push_back(rhs.scale);
            node.push_back(rhs.top);

            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, Hachiko::ParticleSystem::Emitter::Properties& rhs)
        {
            if (!node.IsSequence() || node.size() != 8)
            {
                return false;
            }

            rhs.arc = node[0].as<float>();
            rhs.emit_from = static_cast<Hachiko::ParticleSystem::Emitter::EmitFrom>(node[1].as<int>());
            rhs.position = node[2].as<float3>();
            rhs.radius = node[3].as<float>();
            rhs.radius_thickness = node[4].as<float>();
            rhs.rotation = node[5].as<float3>();
            rhs.scale = node[6].as<float3>();
            rhs.top = node[7].as<float>();
            
            return true;
        }
    };
    
} // namespace YAML
