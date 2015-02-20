#include "proxy.h"
#include "color.h"
#include <fstream>
#include <istream>
#include <direct.h>

#define DEFAULT_LUA_PATH "C:\\gproxy\\"

typedef int(__cdecl *ColorSpewMessage_t)(int type, const Color &color, const char *format, ...);
extern ColorSpewMessage_t ColorSpewMessage;
extern Proxy *proxy;

int Lprint(lua_State *L)
{
	for (int i = 1; i <= LFuncs::lua_gettop(L); i++)
	{
		ColorSpewMessage(0, Color(255, 255, 255, 255), "%s\t", LFuncs::lua_tostring(L, i));
		LFuncs::lua_pop(L, 1);
	}
	ColorSpewMessage(0, Color(), "\n");
	return 0;
};

long fsize(FILE *file)
{
	long before = ftell(file);
	fseek(file, 0, SEEK_END);
	long ret = ftell(file);
	fseek(file, before, SEEK_SET);
	return ret;
}

bool fexists(const char *name)
{
	FILE *file;
	bool ret = false;
	fopen_s(&file, name, "rb");
	if(file) ret = true;
	if(file) fclose(file);
	return ret;
}

int Luse(lua_State *L)
{
	_mkdir(DEFAULT_LUA_PATH);
	char tmp[_MAX_PATH + 1];
	sprintf_s(tmp, _MAX_PATH, "%s%s", DEFAULT_LUA_PATH, LFuncs::lua_tolstring(L, 1, 0));
	if (fexists(tmp))
	{
		FILE *file;
		fopen_s(&file, tmp, "rb");
		long size = fsize(file);
		fseek(file, 0, SEEK_SET);
		char *content = (char *)malloc(sizeof(char) * size + 1);
		fread(content, sizeof(char), size, file);
		content[size * sizeof(char)] = 0;
		fclose(file);

		ColorSpewMessage(0, Color(), "size: %i\n\n%s\n\n", size, content);

		proxy->RunString(content, tmp, true, size, 1);

		free(content);
		
		return 1;
	}
	ColorSpewMessage(0, Color(), "NOT EXIST!!!\n");
	return 0;
}