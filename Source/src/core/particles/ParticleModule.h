#pragma once

namespace Hachiko
{
    class Particle;

    class ParticleModule //Forces
    {
    public:

        friend class ComponentParticleSystem;

        ParticleModule() = default;
        ParticleModule(std::string name, bool active);
        virtual ~ParticleModule() = default;

        virtual void Update(Particle&)
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
        
    private:
        std::string name;
        bool active = false;
    };
}
