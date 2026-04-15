#include <gtest/gtest.h>
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
