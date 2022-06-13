#pragma once
#include "Math/float2.h"
#include "Math/float3.h"
#include "utils/RandomUtil.h"
#include "imgui/curve.h"

namespace Hachiko
{
    namespace ParticleSystem
    {
        constexpr int CURVE_TICKS = 6;

        namespace Emitter
        {
            enum class Type
            {
                CONE = 0,
                BOX,
                SPHERE,
                CIRCLE,
                RECTANGLE
            };

            enum class EmitFrom
            {
                VOLUME = 0,
                SHELL,
                EDGE
            };

            enum class State
            {
                PLAYING = 0,
                STOPPED,
                PAUSED
            };

            struct Properties
            {
                float top = 1.0f;
                float radius = 0.5f;
                float radius_thickness = 0.0f;
                float arc = 360.0f;
                EmitFrom emit_from = EmitFrom::VOLUME;
                float3 position = float3::zero;
                float3 rotation = float3::zero;
                float3 scale = float3::one;
            };
        }

        enum class Selection
        {
            CONSTANT = 0,
            BETWEEN_VALUES,
            CURVE
        };

        struct VariableTypeProperty
        {
            VariableTypeProperty():
                VariableTypeProperty(float2::zero)
            {
            }

            VariableTypeProperty(const float2& values, float multiplier = 1.0f, bool curve_enabled = true, bool constant_enabled = true):
                values(values),
                multiplier(multiplier),
                curve_enabled(curve_enabled),
                constant_enabled(constant_enabled)
            {
                //init curve as linear
                curve[0].x = curve[0].y = 0.0f;
                curve[1].x = curve[1].y = 0.25f;
                curve[2].x = curve[2].y = 0.5f;
                curve[3].x = curve[3].y = 0.75f;
                curve[4].x = curve[4].y = 1.0f;
                curve[5].x = curve[5].y = 0.0f;
            }

            Selection selected_option = Selection::CONSTANT; //constant, curve, random between two numbers.
            float2 values = float2::zero;
            float multiplier = 1.0f;
            ImVec2 curve[CURVE_TICKS];
            bool curve_enabled = true;
            bool constant_enabled = true;
            bool selected = false;

            [[nodiscard]] float GetValue(float current_time = 0) const
            {
                switch (selected_option)
                {
                case Selection::CONSTANT:
                    return values.x * multiplier;
                case Selection::BETWEEN_VALUES:
                    return RandomUtil::RandomBetween(values) * multiplier;
                case Selection::CURVE:
                    return ImGui::CurveValueSmooth(current_time, CURVE_TICKS - 1, curve);
                }
                return values.x * multiplier;
            }
        };

        enum class ParticleRenderMode
        {
            PARTICLE_ADDITIVE,
            PARTICLE_TRANSPARENT
        };

        enum class ParticleOrientation
        {
            NORMAL,
            VERTICAL,
            HORIZONTAL,
            STRETCH
        };

        struct ParticleProperties
        {
            float alpha = 1.0f;
            bool orientate_to_direction = false;
            ParticleRenderMode render_mode = ParticleRenderMode::PARTICLE_ADDITIVE;
            ParticleOrientation orientation = ParticleOrientation::NORMAL;
        };
    }
}
