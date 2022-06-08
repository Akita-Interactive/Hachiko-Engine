#include "core/hepch.h"
#include "ParticleModifier.h"

Hachiko::ParticleModifier::ParticleModifier(std::string name, bool active) :
    name(std::move(name)),
    active(active)
{}

void Hachiko::ParticleModifier::Save(YAML::Node& node) const
{
    node[ACTIVE] = active;
}

void Hachiko::ParticleModifier::Load(const YAML::Node& node)
{
    active = node[ACTIVE].IsDefined() ? node[ACTIVE].as<bool>() : false;
}
