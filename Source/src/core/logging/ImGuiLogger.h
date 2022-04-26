#pragma once

#include <functional>

#include "core/logging/Logger.h"

namespace Hachiko
{
    class ImGuiLogger : public Logger
    {
    public:
        void Log(const char* str, LogLevel& level) override;
        void SetCallBack(const std::function<void(std::string&&, LogLevel&)>& callback);

    private:
        std::function<void(std::string&&, LogLevel&)> callback;
    };
}
