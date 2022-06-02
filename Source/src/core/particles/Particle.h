#pragma once
namespace Hachiko
{
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

        void Reset()
        {
        }

        void Draw(ComponentCamera* camera, Program* program) const;

    private:
        float initial_life = 0.0f;
        float initial_speed = 0.0f;
        float4 initial_color = float4::zero;
        float2 initial_size = float2::zero;
        float3 initial_position = float3::zero;

        float current_life = 0.0f;
        float current_speed = 0.0f;
        float4 current_color = float4::zero;
        float2 current_size = float2::zero;
        float3 current_position = float3::zero;

        //render
        ParticleRenderMode render_mode = ParticleRenderMode::PARTICLE_ADDITIVE;

    public:
        [[nodiscard]] float GetInitialLife() const;
        void SetInitialLife(float initial_life);
        [[nodiscard]] float GetInitialSpeed() const;
        void SetInitialSpeed(float initial_speed);
        [[nodiscard]] const float4& GetInitialColor() const;
        void SetInitialColor(const float4& initial_color);
        [[nodiscard]] const float2& GetInitialSize() const;
        void SetInitialSize(const float2& initial_size);
        [[nodiscard]] const float3& GetInitialPosition() const;
        void SetInitialPosition(const float3& initial_position);

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
    };
}
