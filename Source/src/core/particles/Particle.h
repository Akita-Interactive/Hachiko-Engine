#pragma once
namespace Hachiko
{
    class ComponentParticleSystem;

    class Particle
    {
    public:
        Particle() = default;
        virtual ~Particle() = default;

        void Update();
        void Reset();
        void Draw(ComponentCamera* camera, Program* program);

    private:
        bool active = false;
        float current_life = 0.0f;
        float current_speed = 0.0f;
        float2 animation_index = {0.0f, 0.0f};
        float2 current_size = float2::one;
        float3 current_position = float3::zero;
        float3 current_direction = float3::unitY;
        float4 current_color = float4::zero;
        unsigned current_animation_frame = 1;

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

        [[nodiscard]] const float2& GetAnimationIndex() const;
        void SetAnimationIndex(const float2& animation_index);

        [[nodiscard]] const float3& GetCurrentPosition() const;
        void SetCurrentPosition(const float3& current_position);

        [[nodiscard]] const float3& GetCurrentDirection() const;
        void SetCurrentDirection(const float3& current_direction);

        [[nodiscard]] unsigned GetCurrentAnimationFrame() const;
        void SetCurrentAnimationFrame(unsigned current_animation_frame);

        [[nodiscard]] bool HasTexture() const;
        [[nodiscard]] int GetTextureTotalTiles() const;
        [[nodiscard]] float GetInitialLife() const;
        [[nodiscard]] float GetInitialSpeed() const;
        [[nodiscard]] const float2& GetInitialSize() const;
        [[nodiscard]] const float2& GetTextureTiles() const;
        [[nodiscard]] const float3& GetInitialPosition() const;
        [[nodiscard]] const float3& GetInitialDirection() const;

        void SetEmitter(ComponentParticleSystem* emitter);
    };
}
