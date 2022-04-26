#pragma once

//TODO log representation? (json, xml, yml, ...)

namespace Hachiko
{
    enum class LogLevel
    {
        Debug,
        Info,
        Error
    };

    class Logger
    {
    public:
        Logger() = default;
        virtual ~Logger() = default;

        void Debug(const char* str)
        {
            _log(str, LogLevel::Debug);
        }

        void Debug(const std::string& str)
        {
            _log(str.c_str(), LogLevel::Debug);
        }

        virtual void Info(const char* str)
        {
            _log(str, LogLevel::Info);
        }

        virtual void Info(const std::string& str)
        {
            _log(str.c_str(), LogLevel::Info);
        }

        virtual void Error(const char* str)
        {
            _log(str, LogLevel::Error);
        }

        virtual void Error(const std::string& str)
        {
            _log(str.c_str(), LogLevel::Error);
        }

        // chain of responsibility for calling all loggers
        void Register(Logger* next)
        {
            if (this->next_logger)
            {
                this->next_logger->Register(next);
            }
            else
            {
                this->next_logger = next;
            }
        }

        void Unregister(Logger* logger)
        {
            if (next_logger == logger)
            {
                if (next_logger->next_logger)
                {
                    next_logger = next_logger->next_logger;
                }
                else
                {
                    next_logger = nullptr;
                }
            }
            else if (next_logger)
            {
                next_logger->Unregister(logger);
            }
        }

    protected:
        virtual void Log(const char* str, LogLevel& level) = 0;

        [[nodiscard]] static const char* GetCurrentDateTime()
        {
            static char buffer[48];
            const time_t now = std::time(nullptr);
            std::tm ltm {};
            localtime_s(&ltm, &now);
            //2022/01/18 11:52:00 +0200
            strftime(buffer, sizeof(buffer), "%F %T %z", &ltm);
            return buffer;
        }

    private:
        void _log(const char* str, LogLevel level)
        {
            Log(str, level);
            if (next_logger)
            {
                next_logger->_log(str, level);
            }
        }

    private:
        Logger* next_logger {nullptr};
    };
}
