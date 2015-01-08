#ifndef PROXY_H
#define PROXY_H

#define LUA_SUCCESS (0)

struct lua_State;
#define LUA_CONV __cdecl

namespace Lua
{
	enum
	{
		SPECIAL_GLOB = -10002,
		SPECIAL_ENV,
		SPECIAL_REG,
	};
}

namespace LFuncs
{
	inline int lua_upvalueindex(lua_State *L, int index)
	{
		return Lua::SPECIAL_GLOB - index;
	}
	typedef int (LUA_CONV *lua_CFunction)(lua_State *);
	extern lua_State *(LUA_CONV *luaL_newstate)(void);

	extern void (LUA_CONV *luaL_openlibs)(lua_State *L);
	extern lua_CFunction (LUA_CONV *lua_atpanic)(lua_State *L, lua_CFunction panicf);
	extern int (LUA_CONV *luaL_loadbuffer)(lua_State *L, const char *buff, size_t sz, const char *name);
	extern void (LUA_CONV *lua_call)(lua_State *L, int nargs, int nresults);
	extern const char * (LUA_CONV *lua_tolstring)(lua_State *L, int index, size_t *size);
	extern int (LUA_CONV *lua_gettop)(lua_State *L);
	extern void (LUA_CONV *lua_pushcclosure)(lua_State *L, lua_CFunction fn, int n);
	extern void (LUA_CONV *lua_setfield)(lua_State *L, int index, const char *k);
	extern void (LUA_CONV *lua_pushvalue)(lua_State *L, int index);
};

class Proxy
{
public:
	Proxy() {};
	~Proxy() {};
	void CreateState(void);

	void RunString(const char *stringtorun, const char *source = 0, bool run = true, size_t len = 0, int rets = 0);

public:
	lua_State *L;
};

extern Proxy *proxy;
#endif // PROXY_H