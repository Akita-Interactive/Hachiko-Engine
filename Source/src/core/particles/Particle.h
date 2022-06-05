#pragma once
namespace Hachiko
{
    class ComponentParticleSystem;

    enum class ParticleRenderMode
    {
        PARTICLE_ADDITIVE,
        PARTICLE_TRANSPARENT,
        NONE
    };

    class Particle
    {
    public:
        Particle() = default;
        virtual ~Particle() = default;

        void Update();
        void Reset();
        void Draw(ComponentCamera* camera, Program* program) const;

    private:
        bool active = false;
        float current_life = 0.0f;
        float current_speed = 0.0f;
        float4 current_color = float4::zero;
        float2 current_size = float2::one;
        float3 current_position = float3::zero;
        float3 current_direction = float3::unitY;

        //render
        ParticleRenderMode render_mode = ParticleRenderMode::PARTICLE_ADDITIVE;

        ComponentParticleSystem* emitter = nullptr;
        
        void GetModelMatrix(ComponentCamera* camera, float4x4& out_matrix) const;
        
    public:
        [[nodiscard]] bool IsActive() const;
        void Activate();
        void Deactivate();
        
        [[nodiscard]] float GetCurrentLife() const;
        void SetCurrentLife(float current_life);

        [[nodiscard]] float GetCurrentSpeed() const;
        void SetCurrentSpeed(float current_speed);

        [[nodiscard]] const float4& GetCurrentColor() const;
        void SetCurrentColor(const float4& current_color);

        [[nodiscard]] const float2& GetCurrentSize() const;
        void SetCurrentSize(const float2& current_size);

        [[nodiscard]] const float3& GetCurrentPosition() const;
        void SetCurrentPosition(const float3& current_position);

        [[nodiscard]] ParticleRenderMode GetRenderMode() const;
        void SetRenderMode(ParticleRenderMode render_mode);

        [[nodiscard]] const float3& GetCurrentDirection() const;
        void SetCurrentDirection(const float3& current_direction);

        [[nodiscard]] float GetInitialLife() const;
        [[nodiscard]] float GetInitialSpeed() const;
        [[nodiscard]] const float2& GetInitialSize() const;
        [[nodiscard]] const float3& GetInitialPosition() const;
        [[nodiscard]] const float3& GetInitialDirection() const;
        void SetEmitter(ComponentParticleSystem* emitter);
    };
}
