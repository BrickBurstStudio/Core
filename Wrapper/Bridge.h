#pragma once
#include <string>
#include <windows.h>

#include "Lua.h"
#include "Offsets.h"

class Bridge
{
public:
	void Wrap(lua_State* State, DWORD RobloxState, int Index)
	{
		int Type = lua_type(State, Index);
		
		switch(Type)
		{
		case LUA_TNIL:
			{
				
			}
		}
	}

	void UnWrap(DWORD RobloxState, lua_State* State, int Index)
	{
		int Type = roblox_lua_type(RobloxState, Index);

		switch (Type)
		{
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
			
		}
	}

private:
	int registry;
	DWORD m_rL;
	lua_State* m_L;
	std::vector<int> int3breakpoints;
	
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
	
	int vanillaBridge(lua_State* State) {

		roblox_lua_pushstring(m_rL, std::to_string(++registry).c_str());
		DWORD rL = roblox_lua_newthread(m_rL);
		roblox_lua_settable(m_rL, LUA_REGISTRYINDEX);

		int key = lua_tonumber(State, lua_upvalueindex(1));

		roblox_lua_rawgeti(rL, LUA_REGISTRYINDEX, key);

		for (int arg = 1; arg <= lua_gettop(State); ++arg)
			Wrap(State, rL, arg);

		if (roblox_lua_pcall(rL, lua_gettop(State), LUA_MULTRET, 0))
		{
			const char* errormessage = roblox_lua_tostring(rL, -1, 0);

			if (!errormessage || strlen(errormessage) == 0)
				errormessage = "Error occoured, no output from Lua\n";

			if (strcmp(errormessage, "attempt to yield across metamethod/C-call boundary") == 0)
			{

				roblox_lua_pop(rL, 1);
				lua_pushthread(State);
				lua_pushcclosure(State, &resume, 1);
				Wrap(State, rL, -1);

				return lua_yield(State, 0);
			}
			return 0;
		}

		int args = 0;

		for (int arg = 1; arg <= roblox_lua_gettop(rL); ++arg, ++args)
		{
			UnWrap(rL, State, arg);
		}

		roblox_lua_settop(rL, 0);

		return args;
	}

	int robloxBridge(DWORD rL)
	{

		lua_pushstring(m_L, std::to_string(++registry).c_str());
		lua_State* L = lua_newthread(m_L);
		lua_settable(m_L, LUA_REGISTRYINDEX);

		int key = roblox_lua_tonumber(rL, lua_upvalueindex(1));

		lua_rawgeti(L, LUA_REGISTRYINDEX, key);

		for (int arg = 1; arg <= roblox_lua_gettop(rL); ++arg)
			UnWrap(rL, L, arg);

		switch (lua_pcall(L, roblox_lua_gettop(rL), LUA_MULTRET, 0))
		{


		case LUA_YIELD:

			roblox_lua_pushlightuserdata(m_rL, (void*)L);
			roblox_lua_pushcclosure(m_rL, Bridge::int3breakpoints[1], 1);
			return -1;
		case LUA_ERRRUN:
			printf("RVX ROBLOX ERROR: %s\n", lua_tostring(L, -1));
			return -1;
		default: break;
		}

		int args = 0;

		for (int arg = 1; arg <= lua_gettop(L); ++arg, ++args)
			Wrap(L, rL, arg);

		lua_settop(L, 0);

		return args;
		lua_close(L);

	}
};
