#pragma once
#include "core/logging/Logger.h"

namespace Hachiko
{
    class ConsoleLogger : public Logger
    {
    public:
        ~ConsoleLogger() override = default;
        void Log(const char* str, LogLevel& level) override;
        void Log(LogLevel log_level, const char* file, int line, const char* format, ...);
    };
}
