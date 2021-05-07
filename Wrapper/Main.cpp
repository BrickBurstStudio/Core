#include <Windows.h>
#include "Bridge.h"

using Bridge::LuaState;
using Bridge::RobloxState;

static int UserDataGC(lua_State* Thread) {
    void* UD = lua_touserdata(Thread, 1);
    if (RobloxState) {

        roblox_lua_rawgeti(RobloxState, LUA_REGISTRYINDEX, (int)UD);
        if (roblox_lua_type(RobloxState, -1) <= ROBLOX_LUA_TNIL) {
            lua_pushnil(Thread);
            lua_rawseti(Thread, LUA_REGISTRYINDEX, (int)UD);

        }
    }
    return 0;
}

void Init()
{
	// Create Lua State

    LuaState = luaL_newstate();
    luaL_openlibs(LuaState);
	
	
	// Hooking Roblox State

	// Lua Setup

    Bridge::VehHandlerpush();
    luaL_newmetatable(LuaState, "garbagecollector");
    lua_pushcfunction(LuaState, UserDataGC);
    lua_setfield(LuaState, -2, "__gc");
    lua_pushvalue(LuaState, -1);
    lua_setfield(LuaState, -2, "__index");

	// Pushing Globals
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Init, NULL, NULL, NULL);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

