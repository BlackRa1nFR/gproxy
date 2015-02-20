#include "proxy.h"
#include <Windows.h>
#include <string>
#define a(x, y) //MessageBox(0, x, y, 0);


typedef int(__cdecl *ColorSpewMessage_t)(int type, const Color &color, const char *format, ...);
extern ColorSpewMessage_t ColorSpewMessage;

extern lua_State *cl_lua;

int ProxyFn(lua_State *L)
{
	int args = LFuncs::lua_gettop(L);
	int top = LFuncs::lua_gettop(cl_lua);

	LFuncs::lua_push(L, cl_lua, LFuncs::lua_upvalueindex(L, 1));

	for (int i = 1; i <= args; i++)
		LFuncs::lua_push(L, cl_lua, i);


	LFuncs::lua_call(cl_lua, args, Lua::MULTRET);

	int newtop = LFuncs::lua_gettop(cl_lua);
	if (newtop > top)
		for (int i = top + 1; i <= newtop; i++)
			LFuncs::lua_push(cl_lua, L, i);

	LFuncs::lua_settop(cl_lua, top);

	return newtop - top;
}

int LFuncs::lua_push(lua_State *from, lua_State *to, int stack)
{
	char tmp[256];
	static int reference = 0;
	if (reference == 0)
	{
		lua_newtable(to); // reference
		lua_newtable(to); // metatable
		lua_pushlstring(to, "v", 1);
		lua_setfield(to, -2, "__mode");
		lua_setmetatable(to, -2);
		reference = luaL_ref(to, Lua::SPECIAL_REG);
	}
	lua_pushvalue(from, stack);
	int ret = 0;
	// string
	const char *topush = 0;
	size_t size;

	//c function
	lua_CFunction func;

	//userdata
	Lua::UserData *newud;
	Lua::UserData *oldud;

	switch (LFuncs::lua_type(from, -1))
	{
	case Lua::BOOL:
		lua_pushboolean(to, lua_toboolean(from, -1));
		break;
	case Lua::NUMBER:
		lua_pushnumber(to, lua_tonumber(from, -1));
		break;
	case Lua::STRING:
		topush = lua_tolstring(from, -1, &size);
		lua_pushlstring(to, topush, size);
		break;
	case Lua::FUNCTION:
		func = lua_tocfunction(from, -1);
		if (func)
		{
			lua_pushcclosure(to, func, 0);
			if (to == proxy->L)
				lua_pushcclosure(to, &ProxyFn, 1);
		}
		else
		{
			ret = 1;
			lua_pushnil(to);
		}
		break;
	case Lua::USERDATA:
		oldud = (Lua::UserData *)lua_touserdata(from, -1);
		newud = (Lua::UserData *)lua_newuserdata(to, sizeof(Lua::UserData));
		newud->data = oldud->data;
		newud->type = oldud->type;
		if (to == proxy->L && lua_getmetatable(from, -1) != 0)
		{
			if (lua_push(from, to, -1) == 0)
			{
				lua_pushnil(to);
				lua_setfield(to, -2, "__gc");
				lua_setmetatable(to, -2);
			}
			else
			{
				lua_pop(to, 1);
				ColorSpewMessage(0, MainColor, "Couldn't push metatable for type %i userdata\n", newud->type);
			}
			lua_pop(from, 1);
		}
		
		break;
	case Lua::TABLE:

		//
		// if(references[address(topush)]) then return references[address(topush)]
		//
		lua_rawgeti(to, Lua::SPECIAL_REG, reference);
		lua_rawgeti(to, -1, (int)lua_topointer(from, -1));
		if (lua_type(to, -1) != Lua::NIL)
		{
			lua_remove(to, -2);
			break;
		}
		lua_pop(to, 1);


		lua_newtable(to);

		lua_pushnumber(to, (lua_Number)(int)lua_topointer(from, -1));
		lua_pushvalue(to, -2);
		lua_settable(to, -4); // set references[address(table)] to new table
		lua_remove(to, -2); // remove reference table


		lua_pushnil(from);
		while (lua_next(from, -2))
		{
			if (lua_push(from, to, -2) == 0)
			{
				lua_push(from, to, -1);
				lua_settable(to, -3);
			}
			else
				lua_pop(to, 1); // pop nil from stack
			lua_pop(from, 1); // pop value from stack
		}

		sprintf_s(tmp, 256, "%i", lua_gettop(to));
		a(tmp, "END");
		break;
	default:
		lua_pushnil(to);
		ret = 1;
		break;
	}
	LFuncs::lua_pop(from, 1);
	return ret;
}