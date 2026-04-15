#pragma once

extern "C" {
#include "lua.h"
}

// Native functions exposed to Lua
int lua_print_message(lua_State* L);
int lua_add_numbers(lua_State* L);
int lua_get_timestamp(lua_State* L);
int lua_sleep_ms(lua_State* L);

void registerNativeFunctions(class ScriptEngine& engine);
