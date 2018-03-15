#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define NOMINMAX

#include <windows.h>
#include <winhttp.h>
#include <tchar.h>

#endif

#include <stdio.h>

#define _NO_ASYNCRTIMP
#define _NO_PPLXIMP
// #define CPPREST_TARGET_XP
