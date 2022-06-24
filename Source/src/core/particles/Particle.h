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
        void Draw(ComponentCamera* camera, const Program* program);

    private:
        bool active = false;
        float total_life = 0.0f;
        float current_life = 0.0f;
        float current_speed = 0.0f;
        float current_size = 1.0f;
        float current_rotation = 0.0f;
        float2 animation_index = {0.0f, 0.0f};
        float3 current_position = float3::zero;
        float3 current_direction = float3::unitY;
        float4 current_color = float4::zero;
        unsigned current_animation_frame = 0;
        float animation_blend = 0.0f;

        ComponentParticleSystem* emitter = nullptr;        

        void GetModelMatrix(ComponentCamera* camera, float4x4& out_matrix) const;
        
    public:
        [[nodiscard]] bool IsActive() const;
        void Activate();
        void Deactivate();
        
        [[nodiscard]] float GetCurrentLifeNormilized() const;

        [[nodiscard]] float GetCurrentLife() const;
        void SetCurrentLife(float life);

        [[nodiscard]] float GetCurrentSpeed() const;
        void SetCurrentSpeed(float speed);

        [[nodiscard]] float GetCurrentRotation() const;
        void SetCurrentRotation(float rotation);

        [[nodiscard]] const float4& GetCurrentColor() const;
        void SetCurrentColor(const float4& color);

        [[nodiscard]] float GetCurrentSize() const;
        void SetCurrentSize(float size);

        [[nodiscard]] const float2& GetAnimationIndex() const;
        void SetAnimationIndex(const float2& animation_idx);

        [[nodiscard]] const float3& GetCurrentPosition() const;
        void SetCurrentPosition(const float3& position);

        [[nodiscard]] const float3& GetCurrentDirection() const;
        void SetCurrentDirection(const float3& direction);

        [[nodiscard]] unsigned GetCurrentAnimationFrame() const;
        void SetCurrentAnimationFrame(unsigned frame);

        [[nodiscard]] float GetAnimationBlend() const;
        void SetAnimationBlend(float animation_blend);

        [[nodiscard]] bool HasTexture() const;
        [[nodiscard]] int GetTextureTotalTiles() const;
        [[nodiscard]] float GetLife() const;
        [[nodiscard]] float GetInitialLife();
        [[nodiscard]] float GetInitialSpeed() const;
        [[nodiscard]] float GetInitialSize() const;
        [[nodiscard]] float GetInitialRotation() const;
        [[nodiscard]] float3 GetInitialDirection() const;
        [[nodiscard]] const float2& GetTextureTiles() const;

        void SetEmitter(ComponentParticleSystem* particle_emitter);
    };
}
