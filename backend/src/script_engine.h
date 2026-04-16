#pragma once

#include <string>
#include <functional>
#include <unordered_map>

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

class ScriptEngine {
public:
    ScriptEngine();
    ~ScriptEngine();

    bool init();
    bool loadScript(const std::string& filepath);
    bool executeString(const std::string& code);
    
    // Register native C++ functions to Lua
    void registerFunction(const std::string& name, lua_CFunction func);
    
    // Call Lua function from C++
    bool callFunction(const std::string& funcName);
    
    // Call Lua function from C++ with arguments
    template<typename... Args>
    bool callFunction(const std::string& funcName, Args... args);
    
    lua_State* getLuaState() { return L; }
    std::string getLastError() const { return lastError; }

private:
    lua_State* L;
    std::string lastError;
    
    // Helper functions for pushing arguments
    void pushArgument(int value) { lua_pushinteger(L, value); }
    void pushArgument(double value) { lua_pushnumber(L, value); }
    void pushArgument(const std::string& value) { lua_pushstring(L, value.c_str()); }
    void pushArgument(bool value) { lua_pushboolean(L, value); }
    void pushArguments() {}
    
    template<typename T, typename... Args>
    void pushArguments(T first, Args... rest) {
        pushArgument(first);
        pushArguments(rest...);
    }
};

// Template function implementations
template<typename... Args>
bool ScriptEngine::callFunction(const std::string& funcName, Args... args) {
    lua_getglobal(L, funcName.c_str());
    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 1);
        lastError = "Function not found: " + funcName;
        return false;
    }
    
    int numArgs = sizeof...(args);
    pushArguments(args...);
    
    if (lua_pcall(L, numArgs, 0, 0) != LUA_OK) {
        lastError = lua_tostring(L, -1);
        lua_pop(L, 1);
        return false;
    }
    return true;
}
