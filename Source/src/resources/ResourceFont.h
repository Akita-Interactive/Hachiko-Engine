#pragma once

#include "Resource.h"
#include <memory>

#include "GLFont.h"

namespace Hachiko
{
    class ResourceFont : public Resource
    {
    public:
        ResourceFont();
        ResourceFont(UID id);
        ~ResourceFont() override = default;

        std::unique_ptr<GLFont> gl_font = nullptr;

    public:
        CLONE_RESOURCE(ResourceFont)

        ResourceFont(const ResourceFont& other) :
            Resource(other)
        {
            gl_font = std::make_unique<GLFont>(*other.gl_font);
        }

        ResourceFont& operator=(const ResourceFont& other)
        {
            if (this == &other)
                return *this;
            Resource::operator =(other);
            return *this;
        }
    };
} // namespace Hachiko
