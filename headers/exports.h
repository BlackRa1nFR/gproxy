#ifndef EXTERNS_H
#define EXTERNS_H

template <typename T>
static T GetExport(const char *modulename, const char *exportname);

#ifdef __linux
#elif defined(_MSC_VER)
#include <Windows.h>

template <typename T>
static T GetExport(const char *modulename, const char *exportname)
{
	return (T)GetProcAddress(GetModuleHandleA(modulename), exportname);
}

#else
#endif

#endif // EXTERNS_H