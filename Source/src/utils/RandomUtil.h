#pragma once

namespace Hachiko
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution u_dist(0.0f, 1.0f);
    static std::uniform_real_distribution s_dist(-1.0f, 1.0f);

    class RandomUtil
    {
    public:
        RandomUtil() = delete;
        //(0.0; 1.0)
        static float Random();
        //(-1.0; 1.0)
        static float RandomSigned();
        // (a; b) if a > b returns a
        static float RandomBetween(float a, float b);
        // (values.x; values.y) if x > y returns x
        static float RandomBetween(const float2& values);
    };
}
