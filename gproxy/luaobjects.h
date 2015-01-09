#ifndef LUAOBJECT_H
#define LUAOBJECT_H
#include <string>

typedef unsigned long dword;
struct lua_State;
class CLuaInterface;

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
	CLuaObject something2;
	CLuaObject hookcall;
	std::string *basename;
};

class CLuaInterface
{
public:
	void *vtable;

public:
	lua_State *L;
};

#endif