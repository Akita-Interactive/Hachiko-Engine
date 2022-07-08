#pragma once
namespace Hachiko
{
    class HACHIKO_API bool2
    {
    public:
        bool x;

        bool y;

        bool2() = default;

        bool2(const bool2& other) =  default;

        bool2& operator=(const bool2& other);
        
        bool2(bool x, bool y);

        explicit bool2(bool scalar);

        explicit bool2(const bool* data);

        static const bool2 True;
        static const bool2 False;
    };
}
