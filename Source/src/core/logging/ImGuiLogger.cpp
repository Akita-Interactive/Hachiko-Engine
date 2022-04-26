#include "core/hepch.h"
#include "ImGuiLogger.h"

void Hachiko::ImGuiLogger::Log(const char* str, LogLevel& level)
{
    callback(str, level);
}

void Hachiko::ImGuiLogger::SetCallBack(const std::function<void(std::string&&, LogLevel&)>& callback)
{
    this->callback = callback;
}
