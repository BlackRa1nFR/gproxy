#include "proxy.h"
#include <string>
#include "types.h"
#include "luainterface.h"
#include "userdata.h"

using namespace GarrysMod::Lua::Type;
using namespace LFuncs;

static int (LUA_CONV ProxyFunction)(lua_State *from) {
	lua_State *to = proxy.proxy_interface->GetState();

	int amt = LFuncs::lua_gettop(from);
	int top = LFuncs::lua_gettop(to);

	LFuncs::lua_pushto(from, to, LFuncs::lua_upvalueindex(1));
	for (int i = 1; i <= amt; i++)
		if (LFuncs::lua_pushto(from, to, i))
			LFuncs::lua_pushnil(to);

	int mask = LFuncs::lua_gethookmask(to), count = LFuncs::lua_gethookcount(to);
	LFuncs::lua_Hook hook = LFuncs::lua_gethook(to);
	LFuncs::lua_sethook(to, NULL, 0, 0);
	LFuncs::lua_call(to, amt, -1);
	LFuncs::lua_sethook(to, hook, mask, count);

	amt = LFuncs::lua_gettop(to) - top;

	for (int i = 1; i <= amt; i++)
		if (LFuncs::lua_pushto(to, from, top + i))
			LFuncs::lua_pushnil(from);

	LFuncs::lua_settop(to, top);
	return amt;
}

#include "luajit_internals.h"

struct ProxyUserData {
	GarrysMod::Lua::UserData ud;
	GCudata *proxy_data;
};

int (LUA_CONV __gc_override)(lua_State *L) {
	auto ud = (ProxyUserData *)lua_touserdata(L, 1);
	ud->proxy_data->unused2 = 0;
	return 0;
}

int LFuncs::lua_pushto(lua_State *from, lua_State *to, int stack, bool first)
{
	if (stack < 0 && stack > -10000) {
		stack = lua_gettop(from) + stack + 1;
	}
	switch (LFuncs::lua_type(from, stack)) {
	case NIL: 
		lua_pushnil(to);
		break;
	case NUMBER: 
		lua_pushnumber(to, lua_tonumber(from, stack));
		break;
	case BOOL:
		lua_pushboolean(to, lua_toboolean(from, stack));
		break;
	case STRING: {
		size_t slen;
		const char *str = lua_tolstring(from, stack, &slen);
		lua_pushlstring(to, str, slen);
		break;
	}
	case USERDATA: {
		auto ud_from = (GarrysMod::Lua::UserData *)lua_touserdata(from, stack);
		if (proxy == to) {
			GCudata *udgc = &((GCudata *)ud_from)[-1];
			auto ud = (ProxyUserData *)lua_newuserdata(to, sizeof(ProxyUserData));
			ud->proxy_data = udgc;
			ud->ud.data = ud_from->data;
			ud->ud.type = ud_from->type;

			udgc->unused2 = 0xc9;

			/* update metatable */

			std::string type = MetaTableTypes.at(ud->ud.type);
			/* todo: better entity method */
			if (ud->ud.type == ENTITY) {
				lua_getmetatable(from, stack);
				lua_pushlstring(from, "MetaName", 8);
				lua_rawget(from, -2);
				if (lua_type(from, -1) == STRING) {
					type = lua_tolstring(from, -1, 0);
				}
				lua_pop(from, 2);
			}
			lua_pushlstring(to, type.c_str(), type.length());
			lua_rawget(to, GarrysMod::Lua::INDEX_REGISTRY);
			if (lua_type(to, -1) == TABLE) {
				lua_pushlstring(to, "__gc", 4);
				lua_pushcclosure(to, __gc_override, 0);
				lua_settable(to, -3);
			}
			lua_setmetatable(to, -2);
		} else if (proxy == from) {
			/* we need to push the userdata's GCobj pointer to the stack directly, here it goes! */
			TValue *top = to->top;
			lua_pushnil(to);
			setudataV(to, top, *(GCudata **)&ud_from[1]);
		}
		break;
	}
	case TABLE:
		if (to == proxy) {
			// if not R._POINTERS then
			//   R._POINTERS = {}
			// end
			// top = R._POINTERS
			lua_pushlstring(to, "_POINTERS", 9);
			lua_gettable(to, GarrysMod::Lua::INDEX_REGISTRY);
			if (lua_type(to, -1) == NIL) {
				lua_pop(to, 1);
				lua_newtable(to);
				lua_pushlstring(to, "_POINTERS", 9);
				lua_pushvalue(to, -2);
				lua_settable(to, GarrysMod::Lua::INDEX_REGISTRY);
			}

			// if R._POINTERS[ptr] then
			//   return R._POINTERS[ptr]

			lua_pushnumber(to, (std::uintptr_t)lua_topointer(from, stack));
			lua_gettable(to, -2);
			if (lua_type(to, -1) == TABLE) {
				lua_remove(to, -2);
				break;
			}
			
			// else 
			//   local ret = {}
			//   R._POINTERS[ptr] = ret
			//   return ret
			// end

			lua_pop(to, 1);
			lua_newtable(to);
			lua_pushnumber(to, (std::uintptr_t)lua_topointer(from, stack));
			lua_pushvalue(to, -2);
			lua_settable(to, -4);
			lua_remove(to, -2);

			// for k, v in pairs(from_tbl) do
			//   to_tbl[k] = copy(v)
			// end
			lua_pushnil(from);

			while (lua_next(from, stack) != 0) {
				if (lua_pushto(from, to, -2, false)) {
					goto end;
				}
				if (lua_pushto(from, to, -1, false)) {
					lua_pop(to, 1);
					goto end;
				}
				lua_settable(to, -3);
			end:
				lua_pop(from, 1);
			}
			break;
		}
		lua_newtable(to);
		break;
		
	case FUNCTION: {
		auto fn = lua_tocfunction(from, stack);
		if (!fn)
			return 1;
		lua_pushcclosure(to, fn, 0);
		if (proxy == to)
			lua_pushcclosure(to, ProxyFunction, 1);
		break;
	}
	default: 
		return 1;
	}
	if (first && to == proxy) {
		lua_pushlstring(to, "_POINTERS", 9);
		lua_pushnil(to);
		lua_settable(to, GarrysMod::Lua::INDEX_REGISTRY);
	}
	return 0;
}