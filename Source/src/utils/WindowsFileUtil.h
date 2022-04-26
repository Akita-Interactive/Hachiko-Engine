#pragma once

#include <string>

namespace Hachiko
{
bool GetFileLastWriteTimestamp(const std::wstring& file_name, 
	std::string& timestamp);
} // namespace Hachiko