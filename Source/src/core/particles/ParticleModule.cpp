#include "core/hepch.h"
#include "ParticleModule.h"

Hachiko::ParticleModule::ParticleModule(std::string name, bool active) :
    name(std::move(name)),
    active(active)
{
}
