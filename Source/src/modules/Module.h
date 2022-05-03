#pragma once

#include "Globals.h"

namespace Hachiko
{
    class Application;

    class Module
    {
    public:
        Module() = default;

        virtual ~Module() = default;

        virtual bool Init()
        {
            return true;
        }

        virtual bool Start()
        {
            return true;
        }

        virtual UpdateStatus PreUpdate(const float delta)
        {
            return UpdateStatus::UPDATE_CONTINUE;
        }

        virtual UpdateStatus Update(const float delta)
        {
            return UpdateStatus::UPDATE_CONTINUE;
        }

        virtual UpdateStatus PostUpdate(const float delta)
        {
            return UpdateStatus::UPDATE_CONTINUE;
        }

        virtual bool CleanUp()
        {
            return true;
        }
    };
}
