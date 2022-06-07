#include "core/hepch.h"
#include "ParticleModule.h"

Hachiko::ParticleModule::ParticleModule(std::string name, bool active) :
    name(std::move(name)),
    active(active)
{}

void Hachiko::ParticleModule::Save(YAML::Node& node) const
{
    node[ACTIVE] = active;
}

void Hachiko::ParticleModule::Load(const YAML::Node& node)
{
    active = node[ACTIVE].IsDefined() ? node[ACTIVE].as<bool>() : false;
}
