#include "vthook.h"
#include "luaobjects.h"
#include "proxy.h"
#include "color.h"
#include "externs.h"

#define LUASHARED_CREATELUAINTERFACE 4

typedef int(__cdecl *ColorSpewMessage_t)(int type, const Color &color, const char *format, ...);
extern ColorSpewMessage_t ColorSpewMessage;

VTHook *vtLuaShared;

CLuaInterface *LuaInterface;
VTHook *vtLuaInterface;
class CLuaInterface;
class CLuaShared;

extern Proxy *proxy;

extern CLuaGamemode **g_pGamemode_ptr; 
#define g_pGamemode (*g_pGamemode_ptr)


int LUA_CONV HookCall(lua_State *L)
{
	unsigned int amt = LFuncs::lua_gettop(L);
	LFuncs::lua_rawgeti(L, Lua::SPECIAL_REG, g_pGamemode->hookcall.reference);
	for (unsigned int i = 1; i <= amt; i++)
		LFuncs::lua_pushvalue(L, i);
	LFuncs::lua_call(L, amt, Lua::MULTRET);
	return LFuncs::lua_gettop(L) - amt;
}


void __fastcall ReferencePushHook(CLuaInterface *ths, void *, unsigned int which)
{
	if (g_pGamemode_ptr && g_pGamemode->hookcall.reference == which)
	{
		LFuncs::lua_pushcclosure(ths->L, &HookCall, 0);
		return;
	}
	return vtLuaInterface->GetOldIndex<void (__thiscall *)(CLuaInterface *, unsigned int)>(37)(ths, which);
}

CLuaInterface *__fastcall CreateLuaInterfaceHook(CLuaShared *ths, void *, unsigned char which, bool bsomething)
{
	auto ret = vtLuaShared->GetOldIndex<CLuaInterface *(__thiscall *)(CLuaShared *, unsigned char, bool)>(LUASHARED_CREATELUAINTERFACE)
		(ths, which, bsomething);
	ColorSpewMessage(0, Color(255, 0, 0, 255), "AYY GUESS WHAT? CREATELUAINTERFACE: %u\n", which);
	if (which == 0)
	{
		LuaInterface = ret;
		vtLuaInterface = new VTHook(ret);
		//37 is ReferencePush

		vtLuaInterface->SetFunc(37, &ReferencePushHook);
		vtLuaInterface->Hook();
	}
	return ret;
}

void dohooks(void)
{
	typedef void *(__cdecl *CreateInterfaceFn)(const char *name, int *pOut);

	CreateInterfaceFn LuaSharedFactory = (CreateInterfaceFn)GetExternFromLib("lua_shared", "CreateInterface");
	CLuaShared *LuaShared = (CLuaShared *)LuaSharedFactory("LUASHARED003", 0);

	ColorSpewMessage(0, Color(255, 255, 0, 255), "LuaShared: %p\n", LuaShared);

	vtLuaShared = new VTHook(LuaShared);

	vtLuaShared->SetFunc(LUASHARED_CREATELUAINTERFACE, &CreateLuaInterfaceHook);
	vtLuaShared->Hook();
}