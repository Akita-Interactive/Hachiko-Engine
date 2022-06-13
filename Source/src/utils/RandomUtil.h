#pragma once

namespace Hachiko
{
    //seed with big state space
    static std::mt19937_64 CreateMersenneTwisterEngine64()
    {
        std::random_device source;
        constexpr std::size_t n = std::mt19937_64::state_size * sizeof(std::mt19937_64::result_type);
        std::random_device::result_type randomData[(n - 1) / sizeof(source()) + 1];
        std::generate(std::begin(randomData), std::end(randomData), std::ref(source));
        std::seed_seq seed(std::begin(randomData), std::end(randomData));
        return std::mt19937_64(seed);
    }

    static std::mt19937_64 generator = CreateMersenneTwisterEngine64();
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
