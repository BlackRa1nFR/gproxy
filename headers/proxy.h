#ifndef PROXY_H
#define PROXY_H

#include "json.hpp"
#include "color.h"
#include "luabase.h"
#include <map>
#include <string>

extern std::map<int, std::string> MetaTableTypes;

extern nlohmann::json config;

struct lua_State;

#define LUA_SUCCESS (0)

struct lua_State;
#define LUA_CONV __cdecl

namespace LFuncs
{
	struct lua_Debug;
	typedef void(LUA_CONV *lua_Hook) (lua_State *L, lua_Debug *ar);
	typedef int (LUA_CONV *lua_CFunction)(lua_State *);
	typedef double lua_Number;
	extern int lua_pushto(lua_State *from, lua_State *to, int stack, bool first = true);
	extern int (LUA_CONV *lua_pcall)(lua_State *L, int nargs, int nresults, int errfunc);
	inline int lua_upvalueindex(int index)
	{
		return GarrysMod::Lua::INDEX_GLOBAL - index;
	}
	extern int (LUA_CONV *lua_type)(lua_State *L, int index);
	extern lua_State *(LUA_CONV *luaL_newstate)(void);

	extern void (LUA_CONV *lua_pushboolean)(lua_State *L, int b);
	extern int (LUA_CONV *lua_toboolean)(lua_State *L, int index);
	extern void (LUA_CONV *lua_pushnil)(lua_State *L);
	extern void (LUA_CONV *lua_pushnumber)(lua_State *L, lua_Number n);
	extern lua_Number(LUA_CONV *lua_tonumber)(lua_State *L, int index);
	extern void (LUA_CONV *lua_pushlstring)(lua_State *L, const char *s, size_t len);
	extern lua_CFunction(LUA_CONV *lua_tocfunction)(lua_State *L, int index);
	extern void *(LUA_CONV *lua_touserdata)(lua_State *L, int index);
	extern void *(LUA_CONV *lua_newuserdata)(lua_State *L, size_t size);
	extern int (LUA_CONV *lua_next)(lua_State *L, int index);
	extern void (LUA_CONV *lua_createtable)(lua_State *L, int numarray, int numrec);
	inline void lua_newtable(lua_State *L)
	{
		lua_createtable(L, 0, 0);
	}
	extern void (LUA_CONV *lua_settable)(lua_State *L, int index);
	extern const void *(LUA_CONV *lua_topointer)(lua_State *L, int index);
	extern void (LUA_CONV *lua_replace)(lua_State *L, int idx);
	extern int (LUA_CONV *luaL_ref)(lua_State *L, int t);
	extern void (LUA_CONV *lua_gettable)(lua_State *L, int index);
	extern int (LUA_CONV *lua_setmetatable)(lua_State *L, int index);
	extern int (LUA_CONV *lua_getmetatable)(lua_State *L, int index);

	extern void (LUA_CONV *luaL_openlibs)(lua_State *L);
	extern lua_CFunction (LUA_CONV *lua_atpanic)(lua_State *L, lua_CFunction panicf);
	extern int (LUA_CONV *luaL_loadbuffer)(lua_State *L, const char *buff, size_t sz, const char *name);
	extern void (LUA_CONV *lua_call)(lua_State *L, int nargs, int nresults);
	extern const char * (LUA_CONV *lua_tolstring)(lua_State *L, int index, size_t *size);
	extern int (LUA_CONV *lua_gettop)(lua_State *L);
	extern void (LUA_CONV *lua_pushcclosure)(lua_State *L, lua_CFunction fn, int n);
	extern void (LUA_CONV *lua_setfield)(lua_State *L, int index, const char *k);
	extern void (LUA_CONV *lua_pushvalue)(lua_State *L, int index);
	extern void (LUA_CONV *lua_settop)(lua_State *L, int index);
	extern void (LUA_CONV *lua_rawgeti)(lua_State *L, int index, int n);
	extern void (LUA_CONV *lua_remove)(lua_State *L, int index);
	extern void (LUA_CONV *lua_insert)(lua_State *L, int index);
	extern int (LUA_CONV *lua_sethook)(lua_State *L, lua_Hook f, int mask, int count);
	extern int (LUA_CONV *lua_gethookmask)(lua_State *L);
	extern int (LUA_CONV *lua_gethookcount)(lua_State *L);
	extern lua_Hook (LUA_CONV *lua_gethook)(lua_State *L);
	extern void (LUA_CONV *lua_rawget)(lua_State *L, int index);

	inline void lua_pop(lua_State *L, int amount)
	{
		lua_settop(L, lua_gettop(L) - amount);
	}

	inline const char *lua_tostring(lua_State *L, int stack)
	{
		lua_pushvalue(L, stack);
		lua_pushlstring(L, "tostring", 8);
		lua_gettable(L, GarrysMod::Lua::INDEX_GLOBAL);
		lua_pushvalue(L, -2);
		lua_call(L, 1, 1);
		lua_remove(L, -2);
		return lua_tolstring(L, -1, 0);
	}
};
namespace GarrysMod {
	namespace Lua {
		class ILuaInterface;
	};
};

class Proxy
{
public:
	Proxy() {};
	~Proxy() {};
	lua_State *GetState(void);

	void RunString(const char *stringtorun, const char *source = 0, bool run = true, size_t len = 0, int rets = 0);

	void GetGamemode(void); 
	
	operator lua_State *() {
		return GetState();
	}

public:
	GarrysMod::Lua::ILuaInterface *_interface;
	GarrysMod::Lua::ILuaInterface *proxy_interface;
	int hook_call_ref;
};

extern Proxy proxy;
#endif // PROXY_H