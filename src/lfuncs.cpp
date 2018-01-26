#include "proxy.h"
#include <fstream>

extern Proxy proxy;

int LUA_CONV Luse(lua_State *L) {
	std::string lua_file_path = config["home_path"];
	lua_file_path += LFuncs::lua_tolstring(L, 1, 0);

	std::string lua_file_contents;

	std::ifstream lua_file(lua_file_path, std::ios::binary | std::ios::ate);
	if (lua_file.is_open()) {
		lua_file_contents.reserve((size_t)lua_file.tellg());
		lua_file.seekg(std::ios::beg);
		lua_file_contents.assign((std::istreambuf_iterator<char>(lua_file)), std::istreambuf_iterator<char>());

		int top = LFuncs::lua_gettop(proxy);
		proxy.RunString(lua_file_contents.c_str(), lua_file_path.c_str(), true, 0, -1);
		return LFuncs::lua_gettop(proxy) - top;
	}

	return 0;
}