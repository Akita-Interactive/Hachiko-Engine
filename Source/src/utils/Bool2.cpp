#include "core/hepch.h"
#include "Bool2.h"

Hachiko::bool2& Hachiko::bool2::operator=(const bool2& other)
{
    if (this == &other)
    {
        return *this;
    }
    x = other.x;
    y = other.y;
    return *this;
}

Hachiko::bool2::bool2(const bool x, const bool y) :
    x(x),
    y(y)
{
}

Hachiko::bool2::bool2(const bool scalar):
    x(scalar),
    y(scalar)
{
}

Hachiko::bool2::bool2(const bool* data)
{
    assert(data != nullptr);
    x = data[0];
    y = data[1];
}

const Hachiko::bool2 Hachiko::bool2::True = bool2(true);
const Hachiko::bool2 Hachiko::bool2::False = bool2(false);
