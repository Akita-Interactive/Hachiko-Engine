#pragma once
#include <random>

namespace Hachiko
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution dist(0.0f, 1.0f);

    class Random
    {
    public:
        Random() = delete;
        static float RandomFloat();
    };
}
