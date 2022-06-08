#pragma once

namespace Hachiko
{
    class Particle;

    class ParticleModifier
    {
    public:

        friend class ComponentParticleSystem;

        ParticleModifier() = default;
        ParticleModifier(std::string name, bool active);
        virtual ~ParticleModifier() = default;

        virtual void Update(std::vector<Particle>&)
        {
        }

        virtual void DrawGui()
        {
        }

        [[nodiscard]] std::string GetName() const
        {
            return name;
        }

        [[nodiscard]] bool IsActive() const
        {
            return active;
        }

        void SetActive(const bool is_active)
        {
            this->active = is_active;
        }
        
        virtual void Save(YAML::Node& node) const;

        virtual void Load(const YAML::Node& node);

    private:
        std::string name;
        bool active = false;
    };
}
