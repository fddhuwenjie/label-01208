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
    
    lua_State* getLuaState() { return L; }
    std::string getLastError() const { return lastError; }

private:
    lua_State* L;
    std::string lastError;
};
