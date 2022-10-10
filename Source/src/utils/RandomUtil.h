#pragma once

#if defined(HACHIKO_API)
// Do Nothing
#elif defined(_MSC_VER)
#define HACHIKO_API __declspec(dllexport)
#endif

namespace Hachiko
{
    class RandomUtil
    {
    public:
        RandomUtil() = delete;
        //(0.0; 1.0)
        HACHIKO_API static float Random();
        //(-1.0; 1.0)
        HACHIKO_API static float RandomSigned();
        //returns -1 or 1
        HACHIKO_API static int RandomInt();
        // (a; b) if a > b returns a
        HACHIKO_API static float RandomBetween(float a, float b);

        HACHIKO_API static int RandomIntBetween(int a, int b);
        // (values.x; values.y) if x > y returns x
        HACHIKO_API static float RandomBetween(const float2& values);
    };

    class HACHIKO_API PerlinNoise
    {
    public:
        PerlinNoise();
        PerlinNoise(unsigned int seed);

        float Random(float previous);
        float RandomRange(float previous, float min, float max);
        
    private:
        float Fade(float t);
        float Gradient(float value);
        void Initialize();
        
    private:
        unsigned int _seed;
        std::vector<int> _permutations;
    };
}
