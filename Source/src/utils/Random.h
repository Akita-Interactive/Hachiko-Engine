#pragma once

namespace Hachiko
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution u_dist(0.0f, 1.0f);
    static std::uniform_real_distribution s_dist(-1.0f, 1.0f);

    class Random
    {
    public:
        Random() = delete;
        //(0.0; 1.0)
        static float RandomUnsignedFloat();
        //(-1.0; 1.0)
        static float RandomSignedFloat();
    };
}
