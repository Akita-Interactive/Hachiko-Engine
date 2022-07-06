#pragma once

#include "Component.h"

class ImGradient;
struct ImGradientMark;

namespace Hachiko
{
    class ComponentBillboard : public Component
    {
    public:
        ComponentBillboard(GameObject* container);
        ~ComponentBillboard() override;
        
        void Start() override;
        void Update() override;

        HACHIKO_API void Play();
        HACHIKO_API void Pause();
        HACHIKO_API void Restart();
        HACHIKO_API void Stop() override;

        void Save(YAML::Node& node) const override;
        void Load(const YAML::Node& node) override;
        
        void DrawGui() override;
        void Draw(ComponentCamera* camera, Program* program) override;

    private:
        bool in_scene = false;
        ParticleSystem::Emitter::State state = ParticleSystem::Emitter::State::STOPPED;
        ParticleSystem::Emitter::Properties emitter_properties;
        float elapsed_time = 0.0f;
        float color_time = 0.0f;
        float animation_time = 0.0f;

        // Sections
        bool parameters_section = true;
        bool renderer_section = true;
        bool texture_section = true;
        bool animation_section = false;
        bool color_section = false;
        bool size_section = false;
        bool rotation_section = false;

        // Parameters
        bool loop = false;
        bool play_on_awake = false;
        float duration = 5.0f;
        ParticleSystem::VariableTypeProperty start_delay {float2::zero, 1.0f, false, true};
        ParticleSystem::VariableTypeProperty start_size {float2::one, 1.0f};
        ParticleSystem::VariableTypeProperty start_rotation {float2::zero, 1.0f};

        // Render
        ParticleSystem::ParticleProperties properties;

        // Texture
        bool2 flip_texture = bool2::False;
        float2 tiles = float2::one;
        float2 factor = float2::one;
        ResourceTexture* texture = nullptr;

        // Animation
        float animation_speed = 0.0f;
        float current_frame = 0.0f;
        float2 animation_index = {0.0f, 0.0f};
        float blend_factor = 0.0f;
        float total_tiles = 1.0f;

        // Color
        int color_cycles = 1.0f;
        float color_frame = 0.0f;
        ImGradient* gradient = nullptr;
        ImGradientMark* dragging_gradient = nullptr;
        ImGradientMark* selected_gradient = nullptr;

        // Size over time
        float size = start_size.GetValue();
        ParticleSystem::VariableTypeProperty size_over_time {float2::one, 0.1f, false, true};
        
        // Rotation over time
        float rotation = start_rotation.GetValue();
        ParticleSystem::VariableTypeProperty rotation_over_time {float2::one, 0.1f, false, true};

        void Reset();
        void PublishIntoScene();
        void DetachFromScene();

        inline bool HasTexture();
        void AddTexture();
        void RemoveTexture();
        
        void UpdateAnimationData();
        void UpdateColorOverLifetime();
        void UpdateRotationOverLifetime();
        void UpdateSizeOverLifetime();

        void GetOrientationMatrix(ComponentCamera* camera, float4x4& model_matrix);
        void DisplayControls();

    };
}
