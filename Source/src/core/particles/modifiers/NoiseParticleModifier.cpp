#include "core/hepch.h"
#include "core/particles/modifiers/NoiseParticleModifier.h"

#include "OpenSimplex/OpenSimplex2S.h"

Hachiko::NoiseParticleModifier::NoiseParticleModifier(const std::string& name) :
    ParticleModifier(name, false)
{
    cfg.max = 100.0f;
    cfg.min = -100.0f;
}

Hachiko::NoiseParticleModifier::~NoiseParticleModifier()
{
}

void Hachiko::NoiseParticleModifier::Update(std::vector<Particle>& particles)
{
    if (IsActive())
    {
        for (auto& particle : particles)
        {
            if (!particle.IsActive())
            {
                continue;
            }

            auto position = particle.GetCurrentPosition(); //get a copy of the position
            const auto factor = static_cast<float>(osn.noise4_XYZBeforeW(position.x, position.y, position.z, EngineTimer::delta_time));
            if (separate_axes)
            {
                position.x += factor * strength_x.GetValue(particle.GetCurrentLifeNormalized());
                position.y += factor * strength_y.GetValue(particle.GetCurrentLifeNormalized());
                position.z += factor * strength_z.GetValue(particle.GetCurrentLifeNormalized());
            }
            else
            {
                const float strength = strength_x.GetValue(particle.GetCurrentLifeNormalized());
                position.x += factor * strength;
                position.y += factor * strength;
                position.z += factor * strength;
            }

            const float pos = position_amount.GetValue(particle.GetCurrentLifeNormalized());
            position.x += factor * pos;
            position.y += factor * pos;
            position.z += factor * pos;
            particle.SetCurrentPosition(position);

            auto rotation = particle.GetCurrentRotation();
            rotation += factor * rotation_amount.GetValue(particle.GetCurrentLifeNormalized());
            particle.SetCurrentRotation(rotation);

            auto scale = particle.GetCurrentSize();
            scale += factor * scale_amount.GetValue(particle.GetCurrentLifeNormalized());
            particle.SetCurrentSize(scale);
        }
    }
}

void Hachiko::NoiseParticleModifier::DrawGui()
{
    Widgets::Checkbox("Separate axes", &separate_axes);
    if (separate_axes)
    {
        MultiTypeSelector("X", strength_x, &cfg);
        MultiTypeSelector("Y", strength_y, &cfg);
        MultiTypeSelector("Z", strength_z, &cfg);
    }
    else
    {
        MultiTypeSelector("Strength", strength_x, &cfg);
    }

    MultiTypeSelector("Position amount", position_amount, &cfg);
    MultiTypeSelector("Rotation amount", rotation_amount, &cfg);
    MultiTypeSelector("Scale amount", scale_amount, &cfg);
}

void Hachiko::NoiseParticleModifier::Save(YAML::Node& node) const
{
    YAML::Node noise_module = node[MODIFIER_NOISE];
    ParticleModifier::Save(noise_module);
    noise_module[NOISE_SEPARATE_AXES] = separate_axes;
    noise_module[NOISE_STRENGTH][NOISE_X] = strength_x;
    noise_module[NOISE_STRENGTH][NOISE_Y] = strength_y;
    noise_module[NOISE_STRENGTH][NOISE_Z] = strength_z;
    noise_module[NOISE_POSITION] = position_amount;
    noise_module[NOISE_ROTATION] = rotation_amount;
    noise_module[NOISE_SCALE] = scale_amount;
}

void Hachiko::NoiseParticleModifier::Load(const YAML::Node& node)
{
    ParticleModifier::Load(node[MODIFIER_NOISE]);
    separate_axes = node[MODIFIER_NOISE][NOISE_SEPARATE_AXES].IsDefined() ? node[MODIFIER_NOISE][NOISE_SEPARATE_AXES].as<bool>() : separate_axes;
    strength_x = node[MODIFIER_NOISE][NOISE_STRENGTH][NOISE_X].IsDefined() ? node[MODIFIER_NOISE][NOISE_STRENGTH][NOISE_X].as<ParticleSystem::VariableTypeProperty>() : strength_x;
    strength_y = node[MODIFIER_NOISE][NOISE_STRENGTH][NOISE_Y].IsDefined() ? node[MODIFIER_NOISE][NOISE_STRENGTH][NOISE_Y].as<ParticleSystem::VariableTypeProperty>() : strength_y;
    strength_z = node[MODIFIER_NOISE][NOISE_STRENGTH][NOISE_Z].IsDefined() ? node[MODIFIER_NOISE][NOISE_STRENGTH][NOISE_Z].as<ParticleSystem::VariableTypeProperty>() : strength_z;
    position_amount = node[MODIFIER_NOISE][NOISE_POSITION].IsDefined() ? node[MODIFIER_NOISE][NOISE_POSITION].as<ParticleSystem::VariableTypeProperty>() : position_amount;
    rotation_amount = node[MODIFIER_NOISE][NOISE_ROTATION].IsDefined() ? node[MODIFIER_NOISE][ROTATION].as<ParticleSystem::VariableTypeProperty>() : rotation_amount;
    scale_amount = node[MODIFIER_NOISE][NOISE_SCALE].IsDefined() ? node[MODIFIER_NOISE][NOISE_SCALE].as<ParticleSystem::VariableTypeProperty>() : scale_amount;
}
