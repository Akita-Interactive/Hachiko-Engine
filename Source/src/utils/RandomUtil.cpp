#include "core/hepch.h"
#include "RandomUtil.h"

float Hachiko::RandomUtil::Random()
{
    return u_dist(generator);
}

float Hachiko::RandomUtil::RandomSigned()
{
    return s_dist(generator);
}

float Hachiko::RandomUtil::RandomBetween(const float a, const float b)
{
    float min = a;
    float max = b;

    if(min > max)
    {
       std::swap(min, max); 
    }

    const std::uniform_real_distribution between_dist(min, max);
    return between_dist(generator);
}

float Hachiko::RandomUtil::RandomBetween(const float2& values)
{
    return RandomBetween(values.x, values.y);
}
