#include "proxy.h"
#include "color.h"
#include "externs.h"
#include <string.h>
#include "memorytools.h"
#include "luaobjects.h"
#include "vthook.h"
#ifdef TESTING
#include <Windows.h>
#include <iostream>
#endif

CLuaGamemode **g_pGamemode_ptr = 0;
#define g_pGamemode (*g_pGamemode_ptr)

Proxy *proxy = 0;

#define definelfunc(name) extern decltype(LFuncs::##name) LFuncs::##name = 0
definelfunc(luaL_openlibs);
definelfunc(luaL_newstate);
definelfunc(luaL_loadbuffer);
definelfunc(lua_tolstring);
definelfunc(lua_pushvalue);
definelfunc(lua_gettop);
definelfunc(lua_atpanic);
definelfunc(lua_call);
definelfunc(lua_setfield);
definelfunc(lua_pushcclosure);
definelfunc(lua_settop);
definelfunc(lua_rawgeti);

#define getlfunc(name) LFuncs::##name = (decltype(LFuncs::##name))GetExternFromLib("lua_shared", #name)

typedef int(__cdecl *ColorSpewMessage_t)(int type, const Color &color, const char *format, ...);
ColorSpewMessage_t ColorSpewMessage = 0;
extern void dohooks(void);


void InitExternals(void)
{
	ColorSpewMessage = (ColorSpewMessage_t)GetExternFromLib("tier0", "ColorSpewMessage");
}

unsigned long __stdcall createproxy(void *lpParameter)
{
	if (!proxy)
	{
		InitExternals();

		dohooks();
		getlfunc(luaL_openlibs);
		getlfunc(luaL_newstate);
		getlfunc(luaL_loadbuffer);
		getlfunc(lua_tolstring);
		getlfunc(lua_pushvalue);
		getlfunc(lua_gettop);
		getlfunc(lua_atpanic);
		getlfunc(lua_call);
		getlfunc(lua_setfield);
		getlfunc(lua_pushcclosure);
		getlfunc(lua_settop);
		getlfunc(lua_rawgeti);

		proxy = new Proxy();
		proxy->GetGamemode();
		proxy->CreateState();
		proxy->RunString("print('hi!');");
	}
	return 0;
}

int LUA_CONV AtPanic(lua_State *L)
{
	ColorSpewMessage(0, Color(255, 0, 0), "Error!\n");
	return 0;
}

int Lprint(lua_State *L);
void Proxy::CreateState(void)
{
	if (!L)
	{
		L = LFuncs::luaL_newstate();
		LFuncs::luaL_openlibs(L);

		LFuncs::lua_pushcclosure(L, &Lprint, 0);
		LFuncs::lua_setfield(L, Lua::SPECIAL_GLOB, "print");
	}
}

void Proxy::GetGamemode(void)
{
	if (!g_pGamemode_ptr)
	{
		// sig leads to something with InitPostEntity string
		auto addr = address(sigscan("\x8B\x0D????\x85\xC9\x74?\x68????\xE8????\xC3", getmodulebaselib("client"))) + 2;

		g_pGamemode_ptr = *(CLuaGamemode ***)addr;
	}

}

void Proxy::RunString(const char *stringtorun, const char *source, bool run, size_t len, int rets)
{
	if (!source)
		source = "RunString";
	if (!len)
		len = strlen(stringtorun);
	auto ret = LFuncs::luaL_loadbuffer(L, stringtorun, len, source);
	ColorSpewMessage(0, Color(), "Return: %i\n", ret);
	if (ret == LUA_SUCCESS && run)
	{
		LFuncs::lua_call(L, 0, rets);
	}
}