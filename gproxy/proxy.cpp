#include "proxy.h"
#include "color.h"
#include "externs.h"
#include <string.h>
#include "memorytools.h"
#include "luaobjects.h"
#ifdef TESTING
#include <Windows.h>
#include <iostream>
#endif

CLuaGamemode **g_pGamemode_ptr = 0;
#define g_pGamemode (*g_pGamemode_ptr)

Proxy *proxy = 0;

#define definelfunc(name) decltype(LFuncs::##name) LFuncs::##name = 0
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
definelfunc(lua_type);
definelfunc(lua_pushboolean);
definelfunc(lua_toboolean);
definelfunc(lua_pushnil);
definelfunc(lua_pushnumber);
definelfunc(lua_tonumber);
definelfunc(lua_pushlstring);
definelfunc(lua_tocfunction);
definelfunc(lua_touserdata);
definelfunc(lua_newuserdata);
definelfunc(lua_next);
definelfunc(lua_createtable);
definelfunc(lua_settable);
definelfunc(lua_topointer);
definelfunc(lua_setmetatable);
definelfunc(luaL_ref);
definelfunc(lua_gettable);
definelfunc(lua_remove);
definelfunc(lua_pcall);
definelfunc(lua_insert);
definelfunc(lua_getmetatable);

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
		getlfunc(lua_getmetatable);
		getlfunc(lua_insert);
		getlfunc(luaL_openlibs);
		getlfunc(lua_pcall);
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
		getlfunc(lua_type);
		getlfunc(lua_pushboolean);
		getlfunc(lua_toboolean);
		getlfunc(lua_pushnil);
		getlfunc(lua_pushnumber);
		getlfunc(lua_tonumber);
		getlfunc(lua_tolstring);
		getlfunc(lua_tocfunction);
		getlfunc(lua_touserdata);
		getlfunc(lua_newuserdata);
		getlfunc(lua_next);
		getlfunc(lua_createtable);
		getlfunc(lua_settable);
		getlfunc(lua_topointer);
		getlfunc(lua_setmetatable);
		getlfunc(luaL_ref);
		getlfunc(lua_gettable);
		getlfunc(lua_remove);
		getlfunc(lua_pushlstring);

		proxy = new Proxy();
		proxy->GetGamemode();
		proxy->CreateState();
		proxy->RunString("use('main.lua')");
	}
	return 0;
}

int LUA_CONV AtPanic(lua_State *L)
{
	ColorSpewMessage(0, MainColor, "Uncaught Error!\n");
	return 0;
}

int Lprint(lua_State *L);
int Luse(lua_State *L);
void Proxy::CreateState(void)
{
	if (!L)
	{
		L = LFuncs::luaL_newstate();
		LFuncs::luaL_openlibs(L);

		LFuncs::lua_pushcclosure(L, &Lprint, 0);
		LFuncs::lua_setfield(L, Lua::SPECIAL_GLOB, "print");
		LFuncs::lua_pushcclosure(L, &Luse, 0);
		LFuncs::lua_setfield(L, Lua::SPECIAL_GLOB, "use");
	}
}

void Proxy::GetGamemode(void)
{
	if (!g_pGamemode_ptr)
	{
		// sig leads to something with PostRenderVGUI string
		auto addr = address(sigscan("\x8B\x0D????\x68????\xE8????\xE8????\x8B\x10\x8B\x3D????\x8B\x1F\x8B\xC8\x8B\x02", getmodulebaselib("client"))) + 2;

		g_pGamemode_ptr = *(CLuaGamemode ***)addr;
	}
}

void Proxy::RunString(const char *stringtorun, const char *_source, bool run, size_t len, int rets)
{
	if (!_source)
		_source = "RunString";
	if (!len)
		len = strlen(stringtorun);
	int top = LFuncs::lua_gettop(L);
	char source[_MAX_PATH + 2];
	sprintf_s(source, _MAX_PATH + 2, "=%s", _source);
	auto ret = LFuncs::luaL_loadbuffer(L, stringtorun, len, source);
	if (ret == LUA_SUCCESS && run)
	{
		LFuncs::call(L, 0, rets);
	}
	else
	{
		PrintError(LFuncs::lua_tolstring(L, -1, 0));
		LFuncs::lua_pop(L, 1);
	}
}

void Proxy::PrintError(const char *error)
{
	ColorSpewMessage(0, MainColor, "ERROR!\n%s\n\n", error);
}