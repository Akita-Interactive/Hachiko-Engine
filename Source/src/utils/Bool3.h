#pragma once
namespace Hachiko
{
    class bool3
    {
    public:
        bool x;

        bool y;

        bool z;

        bool3() = default;

        bool3(const bool3& other) =  default;

        bool3& operator=(const bool3& other);
        
        bool3(bool x, bool y, bool z);

        explicit bool3(bool scalar);

        explicit bool3(const bool* data);

        static const bool3 True;
        static const bool3 False;
    };
}
