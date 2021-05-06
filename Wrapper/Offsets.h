#pragma once
#include <vector>
#include <windows.h>
#include "Lua.h"
#include "RetCheck.h"

#define offset(x) (x - 0x400000 + (DWORD)GetModuleHandleA(0))

#define ROBLOX_LUA_TNIL 0
#define ROBLOX_LUA_TLIGHTUSERDATA 1
#define ROBLOX_LUA_TNUMBER 2
#define ROBLOX_LUA_TBOOLEAN 3
#define ROBLOX_LUA_TSTRING 4
#define ROBLOX_LUA_TTHREAD 5
#define ROBLOX_LUA_TFUNCTION 6
#define ROBLOX_LUA_TTABLE 7
#define ROBLOX_LUA_TUSERDATA 8
#define ROBLOX_LUA_TPROTO 9
#define ROBLOX_LUA_TUPVALUE 10

// Offsets

typedef DWORD(__cdecl* type)(DWORD, int);
type roblox_lua_type = (type)(unprotect(offset(0x0)));

typedef double(__cdecl* tonumber)(DWORD, int);
tonumber roblox_lua_tonumber = (tonumber)(unprotect(offset(0x0)));

typedef bool(__cdecl* toboolean)(DWORD rL, bool idx);
toboolean roblox_lua_toboolean = (toboolean)(unprotect(offset(0x0)));

typedef char* (__stdcall* tolstring)(DWORD rL, int idx, size_t* size);
tolstring roblox_lua_tolstring = (tolstring)(unprotect(offset(0x0)));

typedef void(__fastcall* pushvalue)(DWORD rL, DWORD idx);
pushvalue roblox_lua_pushvalue = (pushvalue)(unprotect(offset(0x0)));

typedef DWORD(__cdecl* ref)(DWORD, DWORD);
ref roblox_luaL_ref = (ref)(Ret::unprotect<DWORD>((byte*)(offset(0x0))));

// Helpers

#define roblox_lua_tostring(rL,i)	roblox_lua_tolstring(rL, (i), NULL)