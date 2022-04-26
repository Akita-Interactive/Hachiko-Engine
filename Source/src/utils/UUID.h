#pragma once

#include <string>
#include <random>

namespace Hachiko
{
    using UID = unsigned long long; //64 bits positive number (size_t)

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
    static std::uniform_int_distribution<UID> uid_distribution;
    static std::uniform_int_distribution<> uuid_distribution(0, 15);

    //https://datatracker.ietf.org/doc/html/rfc4122#section-4.1.1
    //Variant used is 2. The binary equivalent of 'a' (10xx), this goes from 1000 to 1011 (8 to 11) (0x8 to 0xB)
    static std::uniform_int_distribution<> uuid_variant_distribution(8, 11);

    class UUID
    {
    public:
        UUID() = delete;

        static UID GenerateUID();

        static UID StringToUID(const std::string& in);

        static std::string GenerateUUIDv4();
    };
}
