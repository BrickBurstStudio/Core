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

typedef void(__stdcall* rgetfield)(DWORD rL, int idx, const char* k);
rgetfield roblox_lua_getfield = (rgetfield)unprotect(offset(0x0));

typedef char* (__stdcall* rtolstring)(DWORD rL, int idx, size_t* size);
rtolstring roblox_lua_tolstring = (rtolstring)(unprotect(offset(0x0)));

typedef void(__fastcall* rsettop)(DWORD rL, int idx);
rsettop roblox_lua_settop = (rsettop)(unprotect(offset(0x0)));

typedef bool(__cdecl* toboolean)(DWORD rL, bool idx);
toboolean roblox_lua_toboolean = (toboolean)(offset(0x0));

typedef void(__fastcall* pushvalue)(DWORD rL, DWORD idx);
pushvalue roblox_lua_pushvalue = (pushvalue)(unprotect(offset(0x0)));

typedef double(__thiscall* pushnumber)(DWORD rL, double idx);
pushnumber roblox_lua_pushnumber = (pushnumber)(unprotect(offset(0x0)));

typedef void(__fastcall* rpushstring)(DWORD rL, const char*);
rpushstring roblox_lua_pushstring = (rpushstring)(offset(0x0));

typedef int(__cdecl* pcall)(DWORD rL, DWORD, DWORD, DWORD);
pcall roblox_luad_pcall = (pcall)(unprotect(offset(0x0)));

typedef void(__cdecl* pushnil)(DWORD);
pushnil roblox_lua_pushnil = (pushnil)(unprotect(offset(0x0)));

typedef DWORD(__cdecl* next2)(DWORD rL, int idx);
next2 roblox_lua_next = (next2)(unprotect(offset(0x0)));

typedef bool(__cdecl* rboolean)(unsigned int, bool);
rboolean roblox_lua_pushboolean = (rboolean)(unprotect(offset(0x0)));

typedef double(__cdecl* rtonumber)(DWORD, int);
rtonumber roblox_lua_tonumber = (rtonumber)(offset(0x0));

typedef void(__stdcall* rpushcclosure)(DWORD rL, int fn, int non);
rpushcclosure roblox_lua_pushcclosure = (rpushcclosure)(unprotect(offset(0x0)));

typedef void(__cdecl* rcreatetable)(DWORD rL, int num, int fix);
rcreatetable roblox_lua_createtable = (rcreatetable)(unprotect(offset(0x0)));

typedef void* (__cdecl* rnewuserdata)(DWORD rL, int idx);
rnewuserdata roblox_lua_newuserdata = (rnewuserdata)(unprotect(offset(0x0)));

typedef void* (__stdcall* rgetmetatable)(DWORD rL, int idx);
rgetmetatable roblox_lua_getmetatable = (rgetmetatable)(unprotect(offset(0x0)));

typedef void* (__cdecl* rsettable)(DWORD rL, int);
rsettable roblox_lua_settable = (rsettable)(unprotect(offset(0x0)));

typedef DWORD(__cdecl* rtype)(DWORD, int);
rtype roblox_lua_type = (rtype)(offset(0x0));

typedef DWORD(__cdecl* rnewthread)(DWORD);
rnewthread roblox_lua_newthread = (rnewthread)unprotect(offset(0x0));

typedef DWORD(__cdecl* rref)(DWORD, DWORD);
rref roblox_luaL_ref = (rref)(Ret::unprotect<DWORD>((byte*)(offset(0x0))));

typedef void(__cdecl* rrawgeti)(DWORD, DWORD, DWORD);
rrawgeti roblox_lua_rawgeti = (rrawgeti)unprotect(offset(0x0));

typedef int(__cdecl* gettop)(DWORD);
gettop roblox_lua_gettop = (gettop)(offset(0x0));

typedef void(__cdecl* rpushlight)(DWORD, void*);
rpushlight roblox_lua_pushlightuserdata = (rpushlight)(unprotect(offset(0x0)));

typedef void* (__cdecl* touserdata)(DWORD, int);
touserdata roblox_lua_touserdata = (touserdata)Retcheck::Unprotect(offset(0x0), true);

// Helpers

#define roblox_lua_tostring(rL,i)	roblox_lua_tolstring(rL, (i), NULL)
#define roblox_lua_pop(rL,n)		roblox_lua_settop(rL, -(n)-1)
#define roblox_lua_getglobal(rL,s)	roblox_lua_getfield(rL, LUA_GLOBALSINDEX, (s))
#define roblox_lua_newtable(rL) roblox_lua_createtable(rL, 0, 0)

/*
Sehchainfaker
Eternals sehchain fixed for windows7, windows 8,...
*/
void fakeChain(DWORD* chain)
{
	chain[1] = 0x1555555;
	if ((((DWORD*)chain[0])[1]) != NULL) {
		((DWORD*)chain[0])[1] = 0x1555555;
	}
}
void restoreChain(DWORD* chain, DWORD unk, DWORD nextUnk)
{
	chain[1] = unk;
	if ((((DWORD*)chain[0])[1]) != NULL) {
		((DWORD*)chain[0])[1] = nextUnk;
	}
}

/*
roblox_lua_pcall
roblox_lua_pcall we use this to see what error roblox returns if it returns the error we want we put a breakpoint on it
and rewrite it to our own functions sadly roblox_lua_pcall has a check sehchaincheck which i already bypassed!
*/

int roblox_lua_pcall(DWORD rL, int nargs, int nresults, int errfunc)
{
	DWORD* exceptionChain = (DWORD*)__readfsdword(0);
	DWORD unk = exceptionChain[1];
	((((DWORD*)exceptionChain[0])[1]) != NULL);
	{
		DWORD nextUnk = ((DWORD*)exceptionChain[0])[1];
		fakeChain(exceptionChain);
		int ret = roblox_luad_pcall(rL, nargs, nresults, errfunc);
		restoreChain(exceptionChain, unk, nextUnk);
		return ret;
	}

	fakeChain(exceptionChain);
	int ret = roblox_luad_pcall(rL, nargs, nresults, errfunc);
	restoreChain(exceptionChain, unk, 0);
	return ret;
}