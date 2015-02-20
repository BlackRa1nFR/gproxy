#ifndef PROXY_H
#define PROXY_H

#include "color.h"

#define LUA_SUCCESS (0)

struct lua_State;
#define LUA_CONV __cdecl

extern Color MainColor;

namespace Lua
{
	struct UserData
	{
		void *data;
		unsigned int type;
	};
	enum
	{
		INVALID = -1,
		NIL,
		BOOL,
		LIGHTUSERDATA,
		NUMBER,
		STRING,
		TABLE,
		FUNCTION,
		USERDATA,
		THREAD
	};
	enum
	{
		MULTRET = -1,
		SPECIAL_GLOB = -10002,
		SPECIAL_ENV,
		SPECIAL_REG,
	};

}

extern int LUA_CONV ErrorFunc(lua_State *L);

namespace LFuncs
{
	typedef int (LUA_CONV *lua_CFunction)(lua_State *);
	typedef double lua_Number;
	extern int lua_push(lua_State *from, lua_State *to, int stack);
	extern int (LUA_CONV *lua_pcall)(lua_State *L, int nargs, int nresults, int errfunc);
	inline int lua_upvalueindex(lua_State *L, int index)
	{
		return Lua::SPECIAL_GLOB - index;
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

	inline void lua_pop(lua_State *L, int amount)
	{
		lua_settop(L, lua_gettop(L) - amount);
	}

	inline const char *lua_tostring(lua_State *L, int stack)
	{
		lua_pushvalue(L, stack);
		lua_pushlstring(L, "tostring", 8);
		lua_gettable(L, Lua::SPECIAL_GLOB);
		lua_pushvalue(L, -2);
		lua_call(L, 1, 1);
		lua_remove(L, -2);
		return lua_tolstring(L, -1, 0);
	}

	inline bool call(lua_State *L, int args, int rets)
	{
		lua_pushcclosure(L, &ErrorFunc, 0);
		lua_insert(L, -args - 2);
		bool ret = 0 == lua_pcall(L, args, rets, -args - 2);
		if (!ret)
			lua_pop(L, 1);
		else
			lua_remove(L, -rets - 1);
		return ret;
	}
};

class Proxy
{
public:
	Proxy() {};
	~Proxy() {};
	void CreateState(void);

	void RunString(const char *stringtorun, const char *source = 0, bool run = true, size_t len = 0, int rets = 0);

	void GetGamemode(void);

	void PrintError(const char *error);

public:
	lua_State *L;
};

extern Proxy *proxy;
#endif // PROXY_H