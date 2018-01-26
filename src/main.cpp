#include <Windows.h>
static bool loaded = false;

extern void createproxy();
DWORD WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved) {
	if (!loaded) {
		loaded = true;
		createproxy();
	}
	return TRUE;
}

struct lua_State;

extern "C" int __declspec(dllexport) __cdecl gmod13_open(lua_State *L) noexcept {
	if (!loaded) {
		loaded = true;
		createproxy();
	}
	return 0;
}