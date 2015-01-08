#include "proxy.h"
#include "color.h"

typedef int(__cdecl *ColorSpewMessage_t)(int type, const Color &color, const char *format, ...);
extern ColorSpewMessage_t ColorSpewMessage;

int Lprint(lua_State *L)
{
	for (int i = 0; i < LFuncs::lua_gettop(L); i++)
		ColorSpewMessage(0, Color(255, 255, 255, 255), "%s", LFuncs::lua_tolstring(L, i + 1, 0));
	ColorSpewMessage(0, Color(), "\n");
	return 0;
};