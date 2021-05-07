#pragma once
#include <string>
#include <windows.h>

#include "Lua.h"
#include "Offsets.h"



namespace Bridge
{
	int registry;
	DWORD m_RobloxState;
	lua_State* m_L;
	std::vector<int> int3breakpoints;

	void Wrap(lua_State* State, DWORD RobloxState, int Index);
	void UnWrap(DWORD RobloxState, lua_State* State, int Index);

	int robloxBridge(DWORD RobloxState);
	int vanillaBridge(lua_State* L);
	int resumea(DWORD thread);
}

namespace Bridge
{
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
			lua_pushcclosure(State, vanillaBridge, 1);
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



	int robloxBridge(DWORD RobloxState)
	{

		lua_pushstring(m_L, std::to_string(++registry).c_str());
		lua_State* L = lua_newthread(m_L);
		lua_settable(m_L, LUA_REGISTRYINDEX);

		int key = roblox_lua_tonumber(RobloxState, lua_upvalueindex(1));

		lua_rawgeti(L, LUA_REGISTRYINDEX, key);

		for (int arg = 1; arg <= roblox_lua_gettop(RobloxState); ++arg)
			UnWrap(RobloxState, L, arg);

		switch (lua_pcall(L, roblox_lua_gettop(RobloxState), LUA_MULTRET, 0))
		{


		case LUA_YIELD:

			roblox_lua_pushlightuserdata(m_RobloxState, (void*)L);
			roblox_lua_pushcclosure(m_RobloxState, int3breakpoints[1], 1);
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

	int vanillaBridge(lua_State* State) {

		roblox_lua_pushstring(m_RobloxState, std::to_string(++registry).c_str());
		DWORD RobloxState = roblox_lua_newthread(m_RobloxState);
		roblox_lua_settable(m_RobloxState, LUA_REGISTRYINDEX);

		int key = lua_tonumber(State, lua_upvalueindex(1));

		roblox_lua_rawgeti(RobloxState, LUA_REGISTRYINDEX, key);

		for (int arg = 1; arg <= lua_gettop(State); ++arg)
			Wrap(State, RobloxState, arg);

		if (roblox_lua_pcall(RobloxState, lua_gettop(State), LUA_MULTRET, 0))
		{
			const char* errormessage = roblox_lua_tostring(RobloxState, -1, 0);

			if (!errormessage || strlen(errormessage) == 0)
				errormessage = "Error occoured, no output from Lua\n";

			if (strcmp(errormessage, "attempt to yield across metamethod/C-call boundary") == 0)
			{

				roblox_lua_pop(RobloxState, 1);
				lua_pushthread(State);
				lua_pushcclosure(State, &resume, 1);
				Wrap(State, RobloxState, -1);

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