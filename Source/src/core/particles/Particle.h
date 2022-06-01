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

        void Start() {};
        void Update() {};
        void Reset() {};
        void Draw(ComponentCamera* camera, Program* program);

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

        //render
        ParticleRenderMode render_mode = ParticleRenderMode::PARTICLE_ADDITIVE;
    };
}


