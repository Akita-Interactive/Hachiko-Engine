#include "core/hepch.h"
#include "Bool3.h"

Hachiko::bool3& Hachiko::bool3::operator=(const bool3& other)
{
    if (this == &other)
    {
        return *this;
    }
    x = other.x;
    y = other.y;
    z = other.z;
    return *this;
}

Hachiko::bool3::bool3(const bool x, const bool y, const bool z) :
    x(x),
    y(y),
    z(z)
{
}

Hachiko::bool3::bool3(const bool scalar):
    x(scalar),
    y(scalar),
    z(scalar)
{
}

Hachiko::bool3::bool3(const bool* data)
{
    assert(data != nullptr);
    x = data[0];
    y = data[1];
    z = data[2];
}

const Hachiko::bool3 Hachiko::bool3::True = bool3(true);
const Hachiko::bool3 Hachiko::bool3::False = bool3(false);
