#include <Shlobj.h>
#include <fstream>
#include <string>
#include "proxy.h"
#include "lua_functions.h"
#include "exports.h"
#include "memorytools.h"
#include "luaobjects.h"
#include "luashared.h"
#include "luainterface.h"

nlohmann::json config;
CLuaGamemode **g_pGamemode_ptr = 0;
#define g_pGamemode (*g_pGamemode_ptr)
Proxy proxy;

#define definelfunc(name) decltype(LFuncs::##name) LFuncs::##name = 0;
LUA_FUNCTIONS(definelfunc)
#undef definelfunc

extern void dohooks(void);

void createproxy(void)
{
	// Get User's home path
	char userhome[MAX_PATH];
	if (FAILED(SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, userhome))) {
		return;
	}
	std::string json_contents;
	{
		std::ifstream config_file(std::string(userhome) + "/gproxy.json", std::ios::binary);

		if (!config_file.is_open()) {
			std::ofstream config_file_default(userhome + std::string("/gproxy.json"), std::ios::binary);

			config_file_default << R"({
				"home_path": "C:\\gproxy\\"
			})";

			config_file_default.close();
		}
	}
	std::ifstream config_file(std::string(userhome) + "/gproxy.json", std::ios::binary | std::ios::ate);
	json_contents.reserve((size_t)config_file.tellg());
	config_file.seekg(std::ios::beg);
	json_contents.assign((std::istreambuf_iterator<char>(config_file)), std::istreambuf_iterator<char>());
	config_file.close();
	
	config = nlohmann::json::parse(json_contents);

	dohooks();

#define getlfunc(name) LFuncs::##name = GetExport<decltype(LFuncs::##name)>("lua_shared", #name);
	LUA_FUNCTIONS(getlfunc)
#undef getlfunc
	int LUA_CONV Luse(lua_State *L);
	LFuncs::lua_pushlstring(proxy, "use", 3);
	LFuncs::lua_pushcclosure(proxy, Luse, 0);
	LFuncs::lua_settable(proxy, GarrysMod::Lua::INDEX_GLOBAL);
	proxy.GetGamemode();
	proxy.RunString("use 'init.lua'", "Init");
}

lua_State *Proxy::GetState(void)
{
	if (!this->_interface) {
		int out;
		auto CreateInterface = GetExport<void *(__cdecl *)(const char *, int *)>("lua_shared", "CreateInterface");
		GarrysMod::Lua::ILuaShared *LuaShared = (GarrysMod::Lua::ILuaShared *)CreateInterface("LUASHARED003", &out);
		this->_interface = LuaShared->GetLuaInterface(GarrysMod::Lua::State::MENU);

		lua_State *L = this->_interface->GetState();

		LFuncs::lua_pushlstring(L, "hook", 4);
		LFuncs::lua_gettable(L, GarrysMod::Lua::INDEX_GLOBAL);
		LFuncs::lua_pushlstring(L, "Call", 4);
		LFuncs::lua_gettable(L, -2);
		
		this->hook_call_ref = LFuncs::luaL_ref(L, GarrysMod::Lua::INDEX_REGISTRY);
		LFuncs::lua_pop(L, 1);
	}
	return this->_interface->GetState();
}

void Proxy::GetGamemode(void) {
	if (!g_pGamemode_ptr) {
		auto addr = std::uintptr_t(sigscan("\xf3\xa5\x8b\x0d????\xf3\x0f\x11\x35????\xf3\x0f\x11\x05????\xf3\x0f\x10\x00", getmodulebaselib("client"))) + 4;

		g_pGamemode_ptr = *(CLuaGamemode ***)addr;
	}
}

void Proxy::RunString(const char *stringtorun, const char *_source, bool run, size_t len, int rets) {
	if (!_source)
		_source = "RunString";
	if (!len)
		len = strlen(stringtorun);

	std::string source("=");
	source += _source;
	auto ret = LFuncs::luaL_loadbuffer(*this, stringtorun, len, source.c_str());
	if (ret == LUA_SUCCESS && run)
	{
		LFuncs::lua_call(*this, 0, rets);
	}
}