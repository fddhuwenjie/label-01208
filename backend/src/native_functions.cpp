#include "native_functions.h"
#include "script_engine.h"
#include <iostream>
#include <chrono>
#include <thread>

int lua_print_message(lua_State* L) {
    const char* msg = luaL_checkstring(L, 1);
    std::cout << "[Native] " << msg << std::endl;
    return 0;
}

int lua_add_numbers(lua_State* L) {
    double a = luaL_checknumber(L, 1);
    double b = luaL_checknumber(L, 2);
    lua_pushnumber(L, a + b);
    return 1;
}

int lua_get_timestamp(lua_State* L) {
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    lua_pushinteger(L, ms);
    return 1;
}

int lua_sleep_ms(lua_State* L) {
    int ms = luaL_checkinteger(L, 1);
    if (ms < 0) {
        return luaL_error(L, "sleep time cannot be negative: %d", ms);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    return 0;
}

void registerNativeFunctions(ScriptEngine& engine) {
    engine.registerFunction("native_print", lua_print_message);
    engine.registerFunction("native_add", lua_add_numbers);
    engine.registerFunction("native_timestamp", lua_get_timestamp);
    engine.registerFunction("native_sleep", lua_sleep_ms);
}
