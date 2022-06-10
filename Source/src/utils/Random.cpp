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

float Hachiko::Random::RandomBetweenFloats(float a, float b)
{
    if (a > b)
    {
        return a;
    }

    std::uniform_real_distribution between_dist(a, b);
    return between_dist(gen);
}

float Hachiko::Random::RandomBetweenFloats(float2 values)
{
    return RandomBetweenFloats(values.x, values.y);
}
