#include "core/hepch.h"
#include "UUID.h"

namespace Hachiko
{
    UID UUID::GenerateUID()
    {
        return uid_distribution(generator);
    }

    UID UUID::StringToUID(const std::string& in)
    {
        return std::stoull(in);
    }

    std::string UUID::GenerateUUIDv4()
    {
        std::stringstream ss;
        int i;

        ss << std::hex; //convert dec to hex
        ss << std::uppercase; //convert to uppercase letters

        for (i = 0; i < 8; i++)
        {
            ss << uuid_distribution(generator);
        }
        ss << "-";
        for (i = 0; i < 4; i++)
        {
            ss << uuid_distribution(generator);
        }
        ss << "-4"; //version 4
        for (i = 0; i < 3; i++)
        {
            ss << uuid_distribution(generator);
        }
        ss << "-";
        ss << uuid_variant_distribution(generator); //variant
        for (i = 0; i < 3; i++)
        {
            ss << uuid_distribution(generator);
        }
        ss << "-";
        for (i = 0; i < 12; i++)
        {
            ss << uuid_distribution(generator);
        }
        return ss.str();
    }
}
