#include "core/hepch.h"
#include "core/particles/modifiers/ForceParticleModifier.h"

Hachiko::ForceParticleModifier::ForceParticleModifier(const std::string& name) :
    ParticleModifier(name, false)
{
    cfg.min = -100.0f;
    cfg.max = 100.0f;
    cfg.speed = 0.1f;
    cfg.format = "%.2f";
}

void Hachiko::ForceParticleModifier::Update(std::vector<Particle>& particles)
{
    for (auto& particle : particles)
    {
        if (!particle.IsActive())
        {
            continue;
        }

        UpdatePositionOverTime(particle);

        if (IsActive())
        {
            UpdateDirectionOverTime(particle);
            UpdateRotationOverTime(particle);
        }
    }
}

void Hachiko::ForceParticleModifier::DrawGui()
{
    MultiTypeSelector("Rotation over lifetime", rotation_delta, &cfg);
    MultiTypeSelector("X force", x_force, &cfg);
    MultiTypeSelector("Y force", y_force, &cfg);
    MultiTypeSelector("Z force", z_force, &cfg);
}

void Hachiko::ForceParticleModifier::Save(YAML::Node& node) const
{
    YAML::Node force_module = node[MODIFIER_FORCE];
    ParticleModifier::Save(force_module);
    force_module[ROTATION] = rotation_delta;
    force_module[X_FORCE] = x_force;
    force_module[Y_FORCE] = y_force;
    force_module[Z_FORCE] = z_force;
}

void Hachiko::ForceParticleModifier::Load(const YAML::Node& node)
{
    ParticleModifier::Load(node[MODIFIER_FORCE]);
    rotation_delta = node[MODIFIER_FORCE][ROTATION].IsDefined() ? node[MODIFIER_FORCE][ROTATION].as<ParticleSystem::VariableTypeProperty>() : rotation_delta;
    x_force = node[MODIFIER_FORCE][X_FORCE].IsDefined() ? node[MODIFIER_FORCE][X_FORCE].as<ParticleSystem::VariableTypeProperty>() : x_force;
    y_force = node[MODIFIER_FORCE][Y_FORCE].IsDefined() ? node[MODIFIER_FORCE][Y_FORCE].as<ParticleSystem::VariableTypeProperty>() : y_force;
    z_force = node[MODIFIER_FORCE][Z_FORCE].IsDefined() ? node[MODIFIER_FORCE][Z_FORCE].as<ParticleSystem::VariableTypeProperty>() : z_force;
}

void Hachiko::ForceParticleModifier::UpdatePositionOverTime(Particle& particle) 
{
    float3 position = particle.GetCurrentPosition();
    position += particle.GetCurrentDirection() * particle.GetCurrentSpeed() * EngineTimer::delta_time;
    particle.SetCurrentPosition(position);
}

void Hachiko::ForceParticleModifier::UpdateDirectionOverTime(Particle& particle)
{
    float3 direction = particle.GetCurrentDirection();
    float particle_life = particle.GetCurrentLifeNormalized();
    direction.x += x_force.GetValue(particle_life) * EngineTimer::delta_time;
    direction.y += y_force.GetValue(particle_life) * EngineTimer::delta_time;
    direction.z += z_force.GetValue(particle_life) * EngineTimer::delta_time;
    direction.Normalize();
    particle.SetCurrentDirection(direction);
}

void Hachiko::ForceParticleModifier::UpdateRotationOverTime(Particle& particle) 
{
    float rotation = particle.GetCurrentRotation();
    rotation += rotation_delta.GetValue(particle.GetCurrentLifeNormalized()) * EngineTimer::delta_time;
    particle.SetCurrentRotation(rotation);
}
