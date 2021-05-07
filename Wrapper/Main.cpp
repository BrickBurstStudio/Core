#include <Windows.h>
#include "Bridge.h"

using Bridge::LuaState;
using Bridge::RobloxState;

static int UserDataGC(lua_State *Thread)
{
    void *UD = lua_touserdata(Thread, 1);
    if (RobloxState)
    {

        roblox_lua_rawgeti(RobloxState, LUA_REGISTRYINDEX, (int)UD);
        if (roblox_lua_type(RobloxState, -1) <= ROBLOX_LUA_TNIL)
        {
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

    PushGlobal(RobloxState, LuaState, "game");
    PushGlobal(RobloxState, LuaState, "Game");
    PushGlobal(RobloxState, LuaState, "workspace");
    PushGlobal(RobloxState, LuaState, "Workspace");
    PushGlobal(RobloxState, LuaState, "Axes");
    PushGlobal(RobloxState, LuaState, "BrickColor");
    PushGlobal(RobloxState, LuaState, "CFrame");
    PushGlobal(RobloxState, LuaState, "Color3");
    PushGlobal(RobloxState, LuaState, "ColorSequence");
    PushGlobal(RobloxState, LuaState, "ColorSequenceKeypoint");
    PushGlobal(RobloxState, LuaState, "NumberRange");
    PushGlobal(RobloxState, LuaState, "NumberSequence");
    PushGlobal(RobloxState, LuaState, "NumberSequenceKeypoint");
    PushGlobal(RobloxState, LuaState, "PhysicalProperties");
    PushGlobal(RobloxState, LuaState, "Ray");
    PushGlobal(RobloxState, LuaState, "Rect");
    PushGlobal(RobloxState, LuaState, "Region3");
    PushGlobal(RobloxState, LuaState, "Region3int16");
    PushGlobal(RobloxState, LuaState, "TweenInfo");
    PushGlobal(RobloxState, LuaState, "UDim");
    PushGlobal(RobloxState, LuaState, "UDim2");
    PushGlobal(RobloxState, LuaState, "Vector2");
    PushGlobal(RobloxState, LuaState, "Vector2int16");
    PushGlobal(RobloxState, LuaState, "Vector3");
    PushGlobal(RobloxState, LuaState, "Vector3int16");
    PushGlobal(RobloxState, LuaState, "Enum");
    PushGlobal(RobloxState, LuaState, "Faces");
    PushGlobal(RobloxState, LuaState, "Instance");
    PushGlobal(RobloxState, LuaState, "math");
    PushGlobal(RobloxState, LuaState, "warn");
    PushGlobal(RobloxState, LuaState, "typeof");
    PushGlobal(RobloxState, LuaState, "type");
    PushGlobal(RobloxState, LuaState, "spawn");
    PushGlobal(RobloxState, LuaState, "Spawn");
    PushGlobal(RobloxState, LuaState, "print");
    PushGlobal(RobloxState, LuaState, "printidentity");
    PushGlobal(RobloxState, LuaState, "ypcall");
    PushGlobal(RobloxState, LuaState, "Wait");
    PushGlobal(RobloxState, LuaState, "wait");
    PushGlobal(RobloxState, LuaState, "delay");
    PushGlobal(RobloxState, LuaState, "Delay");
    PushGlobal(RobloxState, LuaState, "tick");
    PushGlobal(RobloxState, LuaState, "LoadLibrary");
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Init, NULL, NULL, NULL);
        break;
    default:
        break;
    }
    return TRUE;
}
