#pragma once

#include "Window.h"

namespace Hachiko
{
    class Hardware;

    class WindowAbout final : public Window
    {
    public:
        WindowAbout();

        ~WindowAbout() override;
        void Update() override;

    private:
        Hardware* hardware;
    };
}
