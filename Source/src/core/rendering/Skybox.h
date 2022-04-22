#pragma once

#include "modules/ModuleTexture.h"

namespace Hachiko
{
    class ComponentCamera;

    class Skybox
    {
    public:
        Skybox();
        ~Skybox();
        void Draw(ComponentCamera* camera) const;

    private:
        TextureCube texture;
        unsigned vao{};
        unsigned vbo{};
    };
}
