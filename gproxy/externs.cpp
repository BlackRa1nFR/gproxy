#include <Windows.h>

void *GetExternFromLib(const char *modulename, const char *externname)
{
	return GetProcAddress(GetModuleHandleA(modulename), externname);
}