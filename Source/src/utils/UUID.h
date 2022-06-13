#pragma once

#include <string>
#include <random>

#if defined(HACHIKO_API)
// Do Nothing
#elif defined(_MSC_VER)
#define HACHIKO_API __declspec(dllexport)
#endif

namespace Hachiko
{
    using UID = unsigned long long; //64 bits positive number (size_t)

    static std::uniform_int_distribution<UID> uid_distribution;
    static std::uniform_int_distribution<> uuid_distribution(0, 15);

    //https://datatracker.ietf.org/doc/html/rfc4122#section-4.1.1
    //Variant used is 2. The binary equivalent of 'a' (10xx), this goes from 1000 to 1011 (8 to 11) (0x8 to 0xB)
    static std::uniform_int_distribution<> uuid_variant_distribution(8, 11);

    class HACHIKO_API UUID
    {
    public:
        UUID() = delete;

        static UID GenerateUID();

        static UID StringToUID(const std::string& in);

        static std::string GenerateUUIDv4();
    };
}
