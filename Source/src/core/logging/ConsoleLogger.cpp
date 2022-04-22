#include "core/hepch.h"

namespace Hachiko
{
    void ConsoleLogger::Log(LogLevel log_level, const char* file, int line, const char* format, ...)
    {
        static char tmp_string[4096];
        static va_list args;
        va_start(args, format);
        vsprintf_s(tmp_string, 4096, format, args);
        const std::string& msg = StringUtils::Format("%s [%s (%d)] : %s", GetCurrentDateTime(), file, line, tmp_string);
        va_end(args);

        switch (log_level)
        {
        case LogLevel::Error:
            Error(msg);
            break;
        case LogLevel::Info:
            Info(msg);
            break;
        case LogLevel::Debug:
            Debug(msg);
            break;
        }
    }

    void ConsoleLogger::Log(const char* str, LogLevel& level)
    {
        //console logger currently does not care about log level, everything goes as a debug message
        OutputDebugString(StringUtils::Concat(str, "\n").c_str());
    }
}
