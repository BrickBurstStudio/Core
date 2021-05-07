#pragma once
#include <string>
#include <windows.h>
#include "Lua.h"
#include "Offsets.h"
#include <vector>
#include <limits>



namespace Bridge
{
	int registry;
	DWORD RobloxState;
	lua_State* LuaState;
	std::vector<int> int3breakpoints;

	void Wrap(lua_State* State, DWORD RobloxState, int Index);
	void UnWrap(DWORD RobloxState, lua_State* State, int Index);

	LONG WINAPI vehHandler(PEXCEPTION_POINTERS ex);
	VOID VehHandlerpush();
	int RobloxBridge(DWORD RobloxState);
	int LuaBridge(lua_State* L);
	int resumea(DWORD thread);
}

namespace Bridge
{

	LONG WINAPI vehHandler(PEXCEPTION_POINTERS ex)
	{
		switch (ex->ExceptionRecord->ExceptionCode)
		{
		case (DWORD)0x80000003L:
		{
			if (ex->ContextRecord->Eip == int3breakpoints[0])
			{
				ex->ContextRecord->Eip = (DWORD)(RobloxBridge);
				return EXCEPTION_CONTINUE_EXECUTION;
			}

			if (ex->ContextRecord->Eip == int3breakpoints[1])
			{
				ex->ContextRecord->Eip = (DWORD)(Bridge::resumea);
				return EXCEPTION_CONTINUE_EXECUTION;
			}
			return -1;
		}
		default: return 0;
		}
		return 0;
	}

	DWORD locateINT3() {
		DWORD _s = offset(0x400000);
		const char i3_8opcode[8] = { 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC };
		for (int i = 0; i < INT_MAX; i++) {
			if (memcmp((void*)(_s + i), i3_8opcode, sizeof(i3_8opcode)) == 0) {
				return (_s + i);
			}
		}
		return NULL;
	}

	VOID VehHandlerpush()
	{
		int3breakpoints.push_back(locateINT3());
		int3breakpoints.push_back(locateINT3());
		AddVectoredExceptionHandler(1, vehHandler);
	}
	
	void Wrap(lua_State* State, DWORD RobloxState, int Index)
	{
		int Type = lua_type(State, Index);
		
		switch (Type)
		{
		case LUA_TLIGHTUSERDATA:
			roblox_lua_pushlightuserdata(RobloxState, nullptr);
			break;
		case LUA_TNIL:
			roblox_lua_pushnil(RobloxState);
			break;
		case LUA_TNUMBER:
			roblox_lua_pushnumber(RobloxState, lua_tonumber(State, Index));
			break;
		case LUA_TBOOLEAN:
			roblox_lua_pushboolean(RobloxState, lua_toboolean(State, Index));
			break;
		case LUA_TSTRING:
			roblox_lua_pushstring(RobloxState, lua_tostring(State, Index));
			break;
		case LUA_TTHREAD:
			roblox_lua_newthread(RobloxState);
			break;
		case LUA_TFUNCTION:
			lua_pushvalue(State, Index);
			roblox_lua_pushnumber(RobloxState, luaL_ref(State, LUA_REGISTRYINDEX));
			roblox_lua_pushcclosure(RobloxState, int3breakpoints[0], 1);
			break;
		case LUA_TTABLE:
			lua_pushvalue(State, Index);
			roblox_lua_newtable(RobloxState);
			lua_pushnil(State);
			while (lua_next(State, -2) != LUA_TNIL)
			{
				Wrap(State, RobloxState, -2);
				Wrap(State, RobloxState, -1);
				roblox_lua_settable(RobloxState, -3);
				lua_pop(State, 1);
			}
			lua_pop(State, 1);
			break;
		case LUA_TUSERDATA:
			lua_pushvalue(State, Index);
			lua_gettable(State, LUA_REGISTRYINDEX);
			if (!lua_isnil(State, -1))
				roblox_lua_getfield(RobloxState, LUA_REGISTRYINDEX, lua_tostring(State, -1));
			else
				roblox_lua_newuserdata(RobloxState, 0);
			lua_pop(State, 1);
			break;
		default: break;
		}
	}

	void UnWrap(DWORD RobloxState, lua_State* State, int Index)
	{
		int Type = roblox_lua_type(RobloxState, Index);

		switch (Type)
		{
		case ROBLOX_LUA_TLIGHTUSERDATA:
			lua_pushlightuserdata(State, nullptr);
			break;
		case ROBLOX_LUA_TNIL:
				lua_pushnil(State);
				break;
		case ROBLOX_LUA_TNUMBER:
			lua_pushnumber(State, roblox_lua_tonumber(RobloxState, Index));
			break;
		case ROBLOX_LUA_TBOOLEAN:
			lua_pushboolean(State, roblox_lua_toboolean(RobloxState, Index));
			break;
		case ROBLOX_LUA_TSTRING:
			lua_pushstring(State, roblox_lua_tostring(RobloxState, Index));
			break;
		case ROBLOX_LUA_TTHREAD:
			lua_newthread(State);
			break;
		case ROBLOX_LUA_TFUNCTION:
			roblox_lua_pushvalue(RobloxState, Index);
			lua_pushnumber(State, roblox_luaL_ref(RobloxState, LUA_REGISTRYINDEX));
			lua_pushcclosure(State, LuaBridge, 1);
			break;
		case ROBLOX_LUA_TTABLE:
			roblox_lua_pushvalue(RobloxState, Index);
			lua_newtable(State);
			roblox_lua_pushnil(RobloxState);
			while (roblox_lua_next(RobloxState, -2) != ROBLOX_LUA_TNIL)
			{
				UnWrap(RobloxState, State, -2);
				UnWrap(RobloxState, State, -1);
				lua_settable(State, -3);
				roblox_lua_pop(RobloxState, 1);
			}
			roblox_lua_pop(RobloxState, 1);
			break;
		case ROBLOX_LUA_TUSERDATA:
			roblox_lua_pushvalue(RobloxState, Index);
			roblox_lua_pushstring(RobloxState, std::to_string(++registry).c_str());

			roblox_lua_pushvalue(RobloxState, -2);
			roblox_lua_settable(RobloxState, LUA_REGISTRYINDEX);
			roblox_lua_pop(RobloxState, 1);
			lua_newuserdata(State, 0);
			lua_pushvalue(State, -1);
			lua_pushstring(State, std::to_string(registry).c_str());
			lua_settable(State, LUA_REGISTRYINDEX);
			roblox_lua_getmetatable(RobloxState, Index);

			UnWrap(RobloxState, State, -1);
			roblox_lua_pop(RobloxState, 1);
			lua_setmetatable(State, -2);
			break;
		deafult:break;
		}
	}

	static int resume(lua_State* thread)
	{
		lua_State* L = lua_tothread(thread, lua_upvalueindex(1));
		const int nargs = lua_gettop(thread);
		lua_xmove(thread, L, nargs);
		return lua_resume(L, nargs);
		lua_newtable(L);
		lua_pushstring(L, "This metatable is locked");
		lua_setfield(L, -2, "__metatable");
		lua_close(L);

	}

	int resumea(DWORD thread)
	{
		lua_State* L = (lua_State*)roblox_lua_touserdata(thread, lua_upvalueindex(1));
		const int nargs = roblox_lua_gettop(thread);
		for (int arg = 1; arg <= nargs; ++arg)
			UnWrap(thread, L, arg);
		return lua_resume(L, nargs);

	}



	int RobloxBridge(DWORD RobloxState)
	{

		lua_pushstring(LuaState, std::to_string(++registry).c_str());
		lua_State* L = lua_newthread(LuaState);
		lua_settable(LuaState, LUA_REGISTRYINDEX);

		int key = roblox_lua_tonumber(RobloxState, lua_upvalueindex(1));

		lua_rawgeti(L, LUA_REGISTRYINDEX, key);

		for (int arg = 1; arg <= roblox_lua_gettop(RobloxState); ++arg)
			UnWrap(RobloxState, L, arg);

		switch (lua_pcall(L, roblox_lua_gettop(RobloxState), LUA_MULTRET, 0))
		{


		case LUA_YIELD:

			roblox_lua_pushlightuserdata(RobloxState, (void*)L);
			roblox_lua_pushcclosure(RobloxState, int3breakpoints[1], 1);
			return -1;
		case LUA_ERRRUN:
			printf("RVX ROBLOX ERROR: %s\n", lua_tostring(L, -1));
			return -1;
		default: break;
		}

		int args = 0;

		for (int arg = 1; arg <= lua_gettop(L); ++arg, ++args)
			Wrap(L, RobloxState, arg);

		lua_settop(L, 0);

		return args;
		lua_close(L);
	}

	int LuaBridge(lua_State* State) {

		roblox_lua_pushstring(RobloxState, std::to_string(++registry).c_str());
		DWORD NewRobloxState = roblox_lua_newthread(RobloxState);
		roblox_lua_settable(NewRobloxState, LUA_REGISTRYINDEX);

		int key = lua_tonumber(State, lua_upvalueindex(1));

		roblox_lua_rawgeti(NewRobloxState, LUA_REGISTRYINDEX, key);

		for (int arg = 1; arg <= lua_gettop(State); ++arg)
			Wrap(State, NewRobloxState, arg);

		if (roblox_lua_pcall(NewRobloxState, lua_gettop(State), LUA_MULTRET, 0))
		{
			const char* errormessage = roblox_lua_tostring(NewRobloxState, -1, 0);

			if (!errormessage || strlen(errormessage) == 0)
				errormessage = "Error occoured, no output from Lua\n";

			if (strcmp(errormessage, "attempt to yield across metamethod/C-call boundary") == 0)
			{

				roblox_lua_pop(NewRobloxState, 1);
				lua_pushthread(State);
				lua_pushcclosure(State, &resume, 1);
				Wrap(State, NewRobloxState, -1);

				return lua_yield(State, 0);
			}
			return 0;
		}

		int args = 0;

		for (int arg = 1; arg <= roblox_lua_gettop(RobloxState); ++arg, ++args)
		{
			UnWrap(RobloxState, State, arg);
		}

		roblox_lua_settop(RobloxState, 0);

		return args;
		lua_close(State);
	}
};

void PushGlobal(DWORD rL, lua_State* L, const char* s)
{
	roblox_lua_getglobal(rL, s);
	Bridge::UnWrap(rL, L, -1);
	lua_setglobal(L, s);
	roblox_lua_pop(rL, 1);
}