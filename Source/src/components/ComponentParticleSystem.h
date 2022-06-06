#pragma once

namespace Hachiko
{
    class Particle;
    class ParticleModule;

    class HACHIKO_API ComponentParticleSystem : public Component
    {
    public:
        ComponentParticleSystem(GameObject* container);
        ~ComponentParticleSystem() override;

        void Start() override;
        void Update() override;
        void Draw(ComponentCamera* camera, Program* program) override;
        void DrawGui() override;
        void DebugDraw() override;

        void Save(YAML::Node& node) const override;
        void Load(const YAML::Node& node) override;

        [[nodiscard]] ParticleSystem::VariableTypeProperty GetParticlesLife() const;
        [[nodiscard]] ParticleSystem::VariableTypeProperty GetParticlesSize() const;
        [[nodiscard]] ParticleSystem::VariableTypeProperty GetParticlesSpeed() const;
        [[nodiscard]] ParticleSystem::VariableTypeProperty GetParticlesColor() const;
        [[nodiscard]] float3 GetParticlesDirection() const;

    private:
        //sections
        bool parameters_section = true;
        bool emission_section = true;
        bool shape_section = true;
        bool lights_section = false;
        bool renderer_section = true;

        //particle config
        float duration = 5.0f;
        bool loop = false;

        ParticleSystem::VariableTypeProperty delay{float2::zero, false};
        ParticleSystem::VariableTypeProperty life = {float2(5.0f)};
        ParticleSystem::VariableTypeProperty speed = {float2(5.0f)};
        ParticleSystem::VariableTypeProperty size = {float2::one};
        ParticleSystem::VariableTypeProperty rotation = {float2::zero};

        //emission
        float time = 0.0f;
        ParticleSystem::VariableTypeProperty rate_over_time{float2(10)};

        //emitter (shape)
        ParticleSystem::Emitter::Type emitter_type = ParticleSystem::Emitter::Type::CONE;
        ParticleSystem::Emitter::Properties emitter_properties;

        ParticleSystem::VariableTypeProperty* current_curve_editing_property = nullptr;
        std::string current_curve_editing_title;

        //render
        bool in_scene = false;

        //particles
        std::vector<Particle> particles {1000};

        //modules
        std::vector<std::shared_ptr<ParticleModule>> particle_modules{};

        void UpdateActiveParticles();
        void UpdateModules();
        void ActivateParticles();
        float3 GetParticlesDirectionFromShape() const;
    };
} // namespace Hachiko
