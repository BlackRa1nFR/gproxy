#include "vthook.h"
#include "luaobjects.h"
#include "proxy.h"
#include "color.h"
#include "externs.h"

#define LUASHARED_CREATELUAINTERFACE 4
#define LUAINTERFACE_REFERENCEPUSH 37

lua_State *cl_lua = 0;

typedef int(__cdecl *ColorSpewMessage_t)(int type, const Color &color, const char *format, ...);
extern ColorSpewMessage_t ColorSpewMessage;

VTHook *vtLuaShared;

Color MainColor = Color(220, 20, 60, 255);

CLuaInterface *LuaInterface;
VTHook *vtLuaInterface;
class CLuaInterface;
class CLuaShared;

extern Proxy *proxy;

extern CLuaGamemode **g_pGamemode_ptr; 
#define g_pGamemode (*g_pGamemode_ptr)

int LUA_CONV ErrorFunc(lua_State *L)
{
	ColorSpewMessage(0, MainColor, "GProxy lua error!:\n%s\n", LFuncs::lua_tolstring(L, -1, 0));
	LFuncs::lua_pushlstring(L, "debug", 5);
	LFuncs::lua_gettable(L, Lua::SPECIAL_GLOB);
	LFuncs::lua_pushlstring(L, "traceback", 9);
	LFuncs::lua_gettable(L, -2);
	LFuncs::lua_call(L, 0, 1);

	size_t size;
	ColorSpewMessage(0, MainColor, "%s\n\n", LFuncs::lua_tolstring(L, -1, &size));

	LFuncs::lua_pop(L, 1);
	return 0;
}

int LUA_CONV HookCall(lua_State *L)
{
	cl_lua = L;
	int amt = LFuncs::lua_gettop(L);
	LFuncs::lua_rawgeti(L, Lua::SPECIAL_REG, g_pGamemode->hookcall.reference);

	LFuncs::lua_pushlstring(proxy->L, "hook", 4);
	LFuncs::lua_gettable(proxy->L, Lua::SPECIAL_GLOB);
	LFuncs::lua_pushlstring(proxy->L, "Call", 4);
	LFuncs::lua_gettable(proxy->L, -2);

	for (int i = 1; i <= amt; i++)
		if (i != 2)
			LFuncs::lua_push(L, proxy->L, i);
	LFuncs::call(proxy->L, amt - 1, 0);
	LFuncs::lua_pop(proxy->L, 1);


	for (int i = 1; i <= amt; i++)
		LFuncs::lua_pushvalue(L, i);
	LFuncs::lua_call(L, amt, Lua::MULTRET);
	return LFuncs::lua_gettop(L) - amt;
}


void __fastcall ReferencePushHook(CLuaInterface *ths, void *, unsigned int which)
{
	if (g_pGamemode_ptr && g_pGamemode && g_pGamemode->hookcall.reference == which)
	{
		LFuncs::lua_pushcclosure(ths->L, &HookCall, 0);
		return;
	}
	return vtLuaInterface->GetOldIndex<void (__thiscall *)(CLuaInterface *, unsigned int)>
		(LUAINTERFACE_REFERENCEPUSH)(ths, which);
}

CLuaInterface *__fastcall CreateLuaInterfaceHook(CLuaShared *ths, void *, unsigned char which, bool bsomething)
{
	auto ret = vtLuaShared->GetOldIndex<CLuaInterface *(__thiscall *)(CLuaShared *, unsigned char, bool)>(LUASHARED_CREATELUAINTERFACE)
		(ths, which, bsomething);
	//ColorSpewMessage(0, Color(255, 0, 0, 255), "AYY GUESS WHAT? CREATELUAINTERFACE: %u\n", which);
	if (which == 0)
	{
		LuaInterface = ret;
		vtLuaInterface = new VTHook(ret);
		//37 is ReferencePush

		vtLuaInterface->SetFunc(LUAINTERFACE_REFERENCEPUSH, &ReferencePushHook);
		vtLuaInterface->Hook();
		//ColorSpewMessage(0, Color(), "REFERENCEPUSH ADDRESS: %p\n", vtLuaInterface->GetOldIndex<void *>(LUAINTERFACE_REFERENCEPUSH));
	}
	return ret;
}

void dohooks(void)
{
	typedef void *(__cdecl *CreateInterfaceFn)(const char *name, int *pOut);

	CreateInterfaceFn LuaSharedFactory = (CreateInterfaceFn)GetExternFromLib("lua_shared", "CreateInterface");
	CLuaShared *LuaShared = (CLuaShared *)LuaSharedFactory("LUASHARED003", 0);

	//ColorSpewMessage(0, Color(255, 255, 0, 255), "LuaShared: %p\n", LuaShared);

	vtLuaShared = new VTHook(LuaShared);

	vtLuaShared->SetFunc(LUASHARED_CREATELUAINTERFACE, &CreateLuaInterfaceHook);
	vtLuaShared->Hook();
	//ColorSpewMessage(0, Color(255, 0, 255, 255), "Old: %p\nNew: %p\n\n", vtLuaShared->oldvtable, vtLuaShared->newvtable);
}