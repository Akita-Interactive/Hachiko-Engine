#pragma once
#include "core/HachikoApiDefine.h"
#include "core/logging/Logger.h"

namespace Hachiko
{
    class ConsoleLogger : public Logger
    {
    public:
        ~ConsoleLogger() override = default;
        HACHIKO_API void Log(const char* str, LogLevel& level) override;
        HACHIKO_API void Log(LogLevel log_level, const char* file, int line, const char* format, ...);
    };
}
