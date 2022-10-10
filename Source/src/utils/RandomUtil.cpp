#include "core/hepch.h"
#include "utils/RandomUtilDefinitions.h"
#include "utils/RandomUtil.h"

constexpr float MapToRange(float value, float min_old, float max_old, 
    float min_new, float max_new) 
{
    float ratio = (max_new - min_new) / (max_old - min_old);
    float l = max_new - ratio * max_old;

    return ratio * value + l;
}

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

int Hachiko::RandomUtil::RandomIntBetween(int a, int b)
{
    int min = a;
    int max = b;

    if (min > max)
    {
        std::swap(min, max);
    }

    const std::uniform_int_distribution between_dist(min, max);
    return between_dist(generator);
}

float Hachiko::RandomUtil::RandomBetween(const float2& values)
{
    return RandomBetween(values.x, values.y);
}

int Hachiko::RandomUtil::RandomInt() 
{
    return std::rand() & 1 ? 1 : -1;
}

Hachiko::PerlinNoise::PerlinNoise()
{
    _seed = FloorInt(RandomUtil::RandomBetween(FLT_MIN, FLT_MAX));

    Initialize();
}

Hachiko::PerlinNoise::PerlinNoise(unsigned int seed) : _seed(seed)
{
    Initialize();
}

float Hachiko::PerlinNoise::Random(float previous)
{
    float perlin0 = std::floor(previous);
    float perlin1 = perlin0 + 1.0f;

    float delta = previous - perlin0;

    float faded_delta = Fade(delta);

    float grad0 = Gradient(perlin0);
    float grad1 = Gradient(perlin1);

    return (1.0f - faded_delta) * grad0 * (previous - perlin0) + faded_delta * grad1 * (previous - perlin1);
}

float Hachiko::PerlinNoise::RandomRange(float previous, float min, float max)
{
    return MapToRange(Random(previous), -1.0f, 1.0f, min, max);
}

float Hachiko::PerlinNoise::Fade(float t)
{
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

float Hachiko::PerlinNoise::Gradient(float value)
{
    int gradient = _permutations[FloorInt(value) % 256];
    return (gradient < 128 ? -1.0f : 1.0f);
}

void Hachiko::PerlinNoise::Initialize()
{
    _permutations.resize(256);

    for (int i = 0; i < 256; ++i)
    {
        _permutations[i] = i;
    }

    std::default_random_engine engine(_seed);

    std::shuffle(_permutations.begin(), _permutations.end(), engine);
}