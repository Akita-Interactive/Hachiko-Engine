#include "core/hepch.h"
#include "Random.h"

float Hachiko::Random::RandomFloat()
{
    return dist(gen);
}
