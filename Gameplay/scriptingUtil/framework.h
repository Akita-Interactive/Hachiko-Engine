#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>

#ifdef GAMEPLAY_EXPORTS
#define GAMEPLAY_API __declspec(dllexport)
#else
#define GAMEPLAY_API __declspec(dllimport)
#endif // GAMEPLAY_EXPORTS
