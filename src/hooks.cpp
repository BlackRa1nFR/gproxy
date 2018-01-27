#include "vhook.h"
#include "luashared.h"
#include "luainterface.h"
#include "luaobjects.h"
#include "proxy.h"
#include "color.h"
#include "exports.h"

#define LUASHARED_CREATELUAINTERFACE 4
#define LUAINTERFACE_REFERENCEPUSH 37

std::map<int, std::string> MetaTableTypes;

Color MainColor = Color(220, 20, 60, 255);


extern Proxy proxy;

extern CLuaGamemode **g_pGamemode_ptr; 
#define g_pGamemode (*g_pGamemode_ptr)
static VirtualReplacer<CLuaInterface> cl_luainterface_hooker;
static VirtualReplacer<GarrysMod::Lua::ILuaShared> luashared_hooker;
static int createluainterface_index, referencepush_index, runstringex_index, createmetatabletype_index, callinternal_index, callfunctionprotected_index;
using namespace LFuncs;
using namespace GarrysMod::Lua::Type;

static int LUA_CONV HookCall(lua_State *L) {
	/* stub */
	/*
	int amt = LFuncs::lua_gettop(L);
	lua_rawgeti(L, GarrysMod::Lua::INDEX_REGISTRY, g_pGamemode->hookcall.reference);

	for (int i = 1; i <= amt; i++)
		lua_pushvalue(L, i);
	lua_call(L, amt, -1);

	// do proxy state afterwards

	lua_pushlstring(proxy, "hook", 4);
	lua_gettable(proxy, GarrysMod::Lua::INDEX_GLOBAL);
	lua_pushlstring(proxy, "Run", 3);
	lua_gettable(proxy, -2);

	for (int i = 1; i <= amt; i++)
		if (i != 2)
			if (lua_pushto(L, proxy, i))
				lua_pushnil(proxy);

	lua_call(proxy, amt - 1, 0);

	lua_pop(proxy, 1); // hook

	return lua_gettop(L) - amt; */
	return 0;
}
class __HOOKS__;
__HOOKS__ *hooks;

static void table_merge(lua_State *L, int merge_into, int merge_from) {
	if (merge_into < 0 && merge_into > -10000)
		merge_into = LFuncs::lua_gettop(L) + merge_into + 1;
	if (merge_from < 0 && merge_from > -10000)
		merge_from = LFuncs::lua_gettop(L) + merge_from + 1;

	lua_pushnil(L);
	while (lua_next(L, merge_from)) {
		lua_pushvalue(L, -2);
		lua_gettable(L, merge_into);
		int type = lua_type(L, -1);
		lua_pop(L, 1);
		if (type == TABLE && lua_type(L, -1) == TABLE) {
			lua_pushvalue(L, -2);
			lua_newtable(L);
			table_merge(L, -1, -3);
			lua_settable(L, -3);
		}
		else if (type == NIL) {
			lua_pushvalue(L, -2);
			lua_pushvalue(L, -2);
			lua_settable(L, merge_into);
		}
		lua_pop(L, 1);
	}
}

static void fixup_call(lua_State *L, int iArgs) {
	lua_CFunction fn = lua_tocfunction(L, -iArgs - 1);
	if (fn == HookCall) {
		lua_remove(L, lua_gettop(L) - iArgs);
		lua_rawgeti(L, GarrysMod::Lua::INDEX_REGISTRY, g_pGamemode->hookcall.reference);
		if (iArgs != 0)
			lua_insert(L, lua_gettop(L) - iArgs);
		
		lua_rawgeti(proxy, GarrysMod::Lua::INDEX_REGISTRY, proxy.hook_call_ref);

		for (int i = -iArgs; i < 0; i++)
			if (i != -iArgs + 1 || lua_pushto(L, proxy, i))
					lua_pushnil(proxy);

		lua_call(proxy, iArgs, 0);
	}
}

class __HOOKS__ {
public:
	virtual void ReferencePush(unsigned int ref) {
		if (g_pGamemode_ptr && g_pGamemode && g_pGamemode->hookcall.reference == ref)
		{
			lua_pushcclosure(((CLuaInterface *)this)->GetState(), &HookCall, 0);
			return;
		}

		cl_luainterface_hooker.Call<void>(referencepush_index, ref);
	}
	virtual void CallInternal(int iArgs, int iResults) {
		CLuaInterface *_int = (CLuaInterface *)this;
		fixup_call(_int->GetState(), iArgs);
		cl_luainterface_hooker.Call<void>(callinternal_index, iArgs, iResults);
	}
	virtual bool CallFunctionProtected(int iArgs, int iRets, bool something) {
		CLuaInterface *_int = (CLuaInterface *)this;
		fixup_call(_int->GetState(), iArgs);
		return cl_luainterface_hooker.Call<bool>(callfunctionprotected_index, iArgs, iRets, something);
	}

	virtual bool RunStringEx(const char *filename, const char *path, const char *stringToRun, bool run, bool printErrors, bool dontPushErrors, bool noReturns) {
		cl_luainterface_hooker.Unhook(runstringex_index);

		// Add from client state before it can be tainted
		lua_pushlstring(proxy, "cl_g", 4);
		lua_pushto(((CLuaInterface *)this)->GetState(), proxy, GarrysMod::Lua::INDEX_GLOBAL);
		lua_settable(proxy, GarrysMod::Lua::INDEX_GLOBAL);

		lua_pushlstring(proxy, "cl_r", 4);
		lua_pushto(((CLuaInterface *)this)->GetState(), proxy, GarrysMod::Lua::INDEX_REGISTRY);
		lua_settable(proxy, GarrysMod::Lua::INDEX_GLOBAL);

		// Merge non-existant functions to tables
		lua_pushlstring(proxy, "cl_g", 4);
		lua_gettable(proxy, GarrysMod::Lua::INDEX_GLOBAL);
		lua_pushvalue(proxy, GarrysMod::Lua::INDEX_GLOBAL);
		table_merge(proxy, -1, -2);
		lua_pop(proxy, 2);

		lua_pushlstring(proxy, "cl_r", 4);
		lua_gettable(proxy, GarrysMod::Lua::INDEX_GLOBAL);
		lua_pushvalue(proxy, GarrysMod::Lua::INDEX_REGISTRY);
		table_merge(proxy, -1, -2);
		lua_pop(proxy, 2);

		return cl_luainterface_hooker.Call<bool>(runstringex_index, filename, path, stringToRun, run, printErrors, dontPushErrors, noReturns);
	}
	virtual void CreateMetaTableType(const char* strName, int iType) {
		MetaTableTypes.insert(std::pair<int, std::string>(iType, strName));
		cl_luainterface_hooker.Call<void>(createmetatabletype_index, strName, iType);
	}
	virtual CLuaInterface *CreateLuaInterface(unsigned char state, bool bsomething) {
		auto ret = luashared_hooker.Call<CLuaInterface *>(createluainterface_index, state, bsomething);
		
		if (state == GarrysMod::Lua::State::CLIENT) {
			proxy.proxy_interface = ret;
			GarrysMod::Lua::ILuaBase *cl_luainterface_base = ret;
			referencepush_index = GetVirtualIndex(cl_luainterface_base, &CLuaInterface::ReferencePush);
			runstringex_index = GetVirtualIndex(ret, &CLuaInterface::RunStringEx);
			createmetatabletype_index = GetVirtualIndex(cl_luainterface_base, &CLuaInterface::CreateMetaTableType);
			callinternal_index = GetVirtualIndex(ret, &CLuaInterface::CallInternal);
			callfunctionprotected_index = GetVirtualIndex(ret, &CLuaInterface::CallFunctionProtected);

			new (&cl_luainterface_hooker) VirtualReplacer<CLuaInterface>(ret);
			cl_luainterface_hooker.Hook(referencepush_index, GetVirtualAddress(hooks, &__HOOKS__::ReferencePush));
			cl_luainterface_hooker.Hook(runstringex_index, GetVirtualAddress(hooks, &__HOOKS__::RunStringEx));
			cl_luainterface_hooker.Hook(createmetatabletype_index, GetVirtualAddress(hooks, &__HOOKS__::CreateMetaTableType));
			cl_luainterface_hooker.Hook(callinternal_index, GetVirtualAddress(hooks, &__HOOKS__::CallInternal));
			cl_luainterface_hooker.Hook(callfunctionprotected_index, GetVirtualAddress(hooks, &__HOOKS__::CallFunctionProtected));
		}
		return ret;
	}
};

void dohooks(void) {
	hooks = new __HOOKS__;

	auto LuaSharedFactory = GetExport<void *(__cdecl *)(const char *name, int *out)>("lua_shared", "CreateInterface");
	auto LuaShared = (GarrysMod::Lua::ILuaShared *)LuaSharedFactory("LUASHARED003", 0);

	new (&luashared_hooker) VirtualReplacer<GarrysMod::Lua::ILuaShared>(LuaShared);

	createluainterface_index = GetVirtualIndex(LuaShared, &GarrysMod::Lua::ILuaShared::CreateLuaInterface);

	luashared_hooker.Hook(createluainterface_index, GetVirtualAddress(hooks, &__HOOKS__::CreateLuaInterface));

}