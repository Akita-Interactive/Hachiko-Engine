#pragma once

namespace Hachiko
{
    class Particle;
    class ParticleModifier;

    class ComponentParticleSystem : public Component
    {
    public:
        ComponentParticleSystem(GameObject* container);
        ~ComponentParticleSystem() override;

        HACHIKO_API void Start() override;
        void Update() override;
        void Draw(ComponentCamera* camera, Program* program) override;
        void DrawGui() override;
        void DebugDraw() override;

        void Save(YAML::Node& node) const override;
        void Load(const YAML::Node& node) override;

        [[nodiscard]] const ParticleSystem::VariableTypeProperty& GetParticlesLife() const;
        [[nodiscard]] const ParticleSystem::VariableTypeProperty& GetParticlesSpeed() const;
        [[nodiscard]] const ParticleSystem::VariableTypeProperty& GetParticlesSize() const;
        [[nodiscard]] const ParticleSystem::VariableTypeProperty& GetParticlesRotation() const;
        [[nodiscard]] const ParticleSystem::VariableTypeProperty& GetParticlesStretch() const;

        [[nodiscard]] const ParticleSystem::Emitter::Properties& GetEmitterProperties() const;
        [[nodiscard]] const ParticleSystem::ParticleProperties& GetParticlesProperties() const;

        [[nodiscard]] float3 GetPositionFromShape() const;

        [[nodiscard]] const ResourceTexture* GetTexture() const;
        [[nodiscard]] int GetTextureTotalTiles() const;
        [[nodiscard]] const bool2& GetFlipTexture() const;
        [[nodiscard]] const float2& GetTextureTiles() const;
        [[nodiscard]] const float2& GetFactor() const;

        [[nodiscard]] bool IsLoop() const;
        [[nodiscard]] ParticleSystem::Emitter::State GetEmitterState() const;
        [[nodiscard]] ParticleSystem::Emitter::Type GetEmitterType() const;

        void HACHIKO_API Play();
        void HACHIKO_API Pause();
        void Restart();
        void HACHIKO_API Stop() override;

    private:
        ParticleSystem::Emitter::State emitter_state = ParticleSystem::Emitter::State::STOPPED;

        //sections
        bool parameters_section = true;
        bool emission_section = true;
        bool shape_section = true;
        bool lights_section = false;
        bool renderer_section = true;
        bool texture_section = true;

        //particle config
        ParticleSystem::ParticleProperties particle_properties;

        //emission
        bool loop = false;
        float duration = 5.0f;
        bool able_to_emit = false;
        float time = 0.0f;
        float emitter_elapsed_time = 0.0f;
        ParticleSystem::VariableTypeProperty rate_over_time{float2(10)};

        ParticleSystem::VariableTypeProperty start_delay{float2::zero, 1.0f, false};
        ParticleSystem::VariableTypeProperty start_life = {float2(5.0f)};
        ParticleSystem::VariableTypeProperty start_speed = {float2(5.0f), 0.5f};
        ParticleSystem::VariableTypeProperty start_size = {float2::one, 0.25f};
        ParticleSystem::VariableTypeProperty start_rotation = {float2::zero};

        //emitter (shape)
        ParticleSystem::Emitter::Type emitter_type = ParticleSystem::Emitter::Type::CONE;
        ParticleSystem::Emitter::Properties emitter_properties;

        ParticleSystem::VariableTypeProperty* current_curve_editing_property = nullptr;
        std::string current_curve_editing_title;

        //render
        bool in_scene = false;

        //particles
        std::vector<Particle> particles{1000};

        //modules
        std::vector<std::shared_ptr<ParticleModifier>> particle_modifiers{};

        //texture
        bool2 flip_texture = bool2::False;
        float2 tiles = float2::one;
        float2 factor = float2::one;
        ResourceTexture* texture = nullptr;
        float total_tiles = 1.0f;

        void AddTexture();
        void RemoveTexture();

        void ActivateParticles();
        void UpdateActiveParticles();
        void UpdateModifiers();
        void UpdateEmitterTimes();
        void ResetActiveParticles();
        void Reset();

        void DisplayControls();
    };
} // namespace Hachiko
