#ifndef LUAOBJECT_H
#define LUAOBJECT_H
#include <string>
#include "luainterface.h"

typedef unsigned long dword;
struct lua_State;
using CLuaInterface = GarrysMod::Lua::ILuaInterface;


class CLuaObject
{
public:
	void *vtable;
public:
	bool isuserdata;
	dword type;
	dword reference;
	CLuaInterface *m_pInterface;
};

class CLuaGamemode : public CLuaObject
{
public:
	CLuaObject something1;
	CLuaObject hook;
	CLuaObject hookcall;
	std::string *basename;
};

#endif