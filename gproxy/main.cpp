// roblox hack v33

#include <Windows.h>

extern DWORD WINAPI createproxy(LPVOID lpParameter);
DWORD WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		CreateThread(0, 0, &createproxy, 0, 0, 0);
	}
	return TRUE;
}
