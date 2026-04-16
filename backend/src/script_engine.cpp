#include "script_engine.h"
#include <iostream>

ScriptEngine::ScriptEngine() : L(nullptr) {}

ScriptEngine::~ScriptEngine() {
    if (L) {
        lua_close(L);
    }
}

bool ScriptEngine::init() {
    L = luaL_newstate();
    if (!L) {
        lastError = "Failed to create Lua state";
        return false;
    }
    luaL_openlibs(L);
    return true;
}

bool ScriptEngine::loadScript(const std::string& filepath) {
    if (luaL_dofile(L, filepath.c_str()) != LUA_OK) {
        lastError = lua_tostring(L, -1);
        lua_pop(L, 1);
        return false;
    }
    return true;
}

bool ScriptEngine::executeString(const std::string& code) {
    if (luaL_dostring(L, code.c_str()) != LUA_OK) {
        lastError = lua_tostring(L, -1);
        lua_pop(L, 1);
        return false;
    }
    return true;
}

void ScriptEngine::registerFunction(const std::string& name, lua_CFunction func) {
    lua_pushcfunction(L, func);
    lua_setglobal(L, name.c_str());
}

bool ScriptEngine::callFunction(const std::string& funcName) {
    lua_getglobal(L, funcName.c_str());
    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 1);
        lastError = "Function not found: " + funcName;
        return false;
    }
    if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
        lastError = lua_tostring(L, -1);
        lua_pop(L, 1);
        return false;
    }
    return true;
}

template<>
void ScriptEngine::pushArg<int>(int arg) {
    lua_pushinteger(L, arg);
}

template<>
void ScriptEngine::pushArg<double>(double arg) {
    lua_pushnumber(L, arg);
}

template<>
void ScriptEngine::pushArg<const char*>(const char* arg) {
    lua_pushstring(L, arg);
}

template<>
void ScriptEngine::pushArg<std::string>(std::string arg) {
    lua_pushstring(L, arg.c_str());
}

template<typename T, typename... Args>
void ScriptEngine::pushArgs(T first, Args... rest) {
    pushArg(first);
    pushArgs(rest...);
}

template<typename... Args>
bool ScriptEngine::callFunction(const std::string& funcName, Args... args) {
    lua_getglobal(L, funcName.c_str());
    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 1);
        lastError = "Function not found: " + funcName;
        return false;
    }
    pushArgs(args...);
    if (lua_pcall(L, sizeof...(Args), 0, 0) != LUA_OK) {
        lastError = lua_tostring(L, -1);
        lua_pop(L, 1);
        return false;
    }
    return true;
}

template bool ScriptEngine::callFunction<int>(const std::string&, int);
template bool ScriptEngine::callFunction<double>(const std::string&, double);
template bool ScriptEngine::callFunction<const char*>(const std::string&, const char*);
template bool ScriptEngine::callFunction<std::string>(const std::string&, std::string);
template bool ScriptEngine::callFunction<int, int>(const std::string&, int, int);
template bool ScriptEngine::callFunction<int, double>(const std::string&, int, double);
