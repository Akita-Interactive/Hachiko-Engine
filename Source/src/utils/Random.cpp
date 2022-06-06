#include "core/hepch.h"
#include "Random.h"

float Hachiko::Random::RandomUnsignedFloat()
{
    return u_dist(gen);
}

float Hachiko::Random::RandomSignedFloat()
{
    return s_dist(gen);
}