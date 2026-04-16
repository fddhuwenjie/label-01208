#include <gtest/gtest.h>
#include <string>
#include "script_engine.h"

class ScriptEngineTest : public ::testing::Test {
protected:
    ScriptEngine engine;

    void SetUp() override {
        ASSERT_TRUE(engine.init());
    }
};

TEST_F(ScriptEngineTest, InitializesSuccessfully) {
    ScriptEngine newEngine;
    EXPECT_TRUE(newEngine.init());
    EXPECT_NE(newEngine.getLuaState(), nullptr);
}

TEST_F(ScriptEngineTest, ExecuteSimpleCode) {
    EXPECT_TRUE(engine.executeString("x = 1 + 1"));
}

TEST_F(ScriptEngineTest, ExecuteInvalidCodeFails) {
    EXPECT_FALSE(engine.executeString("invalid syntax here !!!"));
    EXPECT_FALSE(engine.getLastError().empty());
}

TEST_F(ScriptEngineTest, DefineAndCallFunction) {
    EXPECT_TRUE(engine.executeString("function test_func() return 42 end"));
    EXPECT_TRUE(engine.callFunction("test_func"));
}

TEST_F(ScriptEngineTest, CallNonExistentFunctionFails) {
    EXPECT_FALSE(engine.callFunction("nonexistent_function"));
    EXPECT_EQ(engine.getLastError(), "Function not found: nonexistent_function");
}

TEST_F(ScriptEngineTest, RegisterAndCallNativeFunction) {
    auto testFunc = [](lua_State* L) -> int {
        lua_pushinteger(L, 100);
        return 1;
    };
    
    engine.registerFunction("get_hundred", testFunc);
    EXPECT_TRUE(engine.executeString("result = get_hundred()"));
    
    lua_getglobal(engine.getLuaState(), "result");
    EXPECT_EQ(lua_tointeger(engine.getLuaState(), -1), 100);
}

TEST_F(ScriptEngineTest, LoadNonExistentFileFails) {
    EXPECT_FALSE(engine.loadScript("nonexistent_file.lua"));
    EXPECT_FALSE(engine.getLastError().empty());
}

TEST_F(ScriptEngineTest, GlobalVariablePersistence) {
    EXPECT_TRUE(engine.executeString("my_var = 'hello'"));
    EXPECT_TRUE(engine.executeString("my_var = my_var .. ' world'"));
    
    lua_getglobal(engine.getLuaState(), "my_var");
    EXPECT_STREQ(lua_tostring(engine.getLuaState(), -1), "hello world");
}

TEST_F(ScriptEngineTest, TableOperations) {
    EXPECT_TRUE(engine.executeString(R"(
        my_table = {a = 1, b = 2, c = 3}
        sum = my_table.a + my_table.b + my_table.c
    )"));
    
    lua_getglobal(engine.getLuaState(), "sum");
    EXPECT_EQ(lua_tointeger(engine.getLuaState(), -1), 6);
}

TEST_F(ScriptEngineTest, LoadScriptWithSyntaxErrorFails) {
    std::string testScriptPath = __FILE__;
    size_t lastSlash = testScriptPath.find_last_of("\\/");
    testScriptPath = testScriptPath.substr(0, lastSlash + 1) + "syntax_error.lua";
    
    EXPECT_FALSE(engine.loadScript(testScriptPath));
    EXPECT_FALSE(engine.getLastError().empty());
    EXPECT_TRUE(engine.getLastError().find("syntax error") != std::string::npos || 
                engine.getLastError().find("unfinished") != std::string::npos);
}

TEST_F(ScriptEngineTest, RegisterSameNameFunctionOverwrites) {
    auto firstFunc = [](lua_State* L) -> int {
        lua_pushinteger(L, 1);
        return 1;
    };
    
    auto secondFunc = [](lua_State* L) -> int {
        lua_pushinteger(L, 2);
        return 1;
    };
    
    engine.registerFunction("test_overwrite", firstFunc);
    EXPECT_TRUE(engine.executeString("result1 = test_overwrite()"));
    
    engine.registerFunction("test_overwrite", secondFunc);
    EXPECT_TRUE(engine.executeString("result2 = test_overwrite()"));
    
    lua_getglobal(engine.getLuaState(), "result1");
    EXPECT_EQ(lua_tointeger(engine.getLuaState(), -1), 1);
    
    lua_getglobal(engine.getLuaState(), "result2");
    EXPECT_EQ(lua_tointeger(engine.getLuaState(), -1), 2);
}

TEST_F(ScriptEngineTest, CallLuaFunctionWithTypeMismatch) {
    EXPECT_TRUE(engine.executeString(R"(
        function expect_number(n)
            if type(n) ~= "number" then
                error("Expected number, got " .. type(n))
            end
            return n + 1
        end
    )"));
    
    EXPECT_TRUE(engine.executeString(R"(
        function expect_string(s)
            if type(s) ~= "string" then
                error("Expected string, got " .. type(s))
            end
            return s .. " world"
        end
    )"));
    
    EXPECT_TRUE(engine.callFunction("expect_number", 42));
    EXPECT_TRUE(engine.callFunction("expect_string", std::string("hello")));
    
    EXPECT_FALSE(engine.callFunction("expect_number", std::string("not a number")));
    EXPECT_FALSE(engine.getLastError().empty());
    EXPECT_TRUE(engine.getLastError().find("Expected number") != std::string::npos);
}
