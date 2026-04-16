#include <gtest/gtest.h>
#include "script_engine.h"
#include "native_functions.h"

class NativeFunctionsTest : public ::testing::Test {
protected:
    ScriptEngine engine;

    void SetUp() override {
        ASSERT_TRUE(engine.init());
        registerNativeFunctions(engine);
    }
};

TEST_F(NativeFunctionsTest, NativeAddReturnsCorrectSum) {
    EXPECT_TRUE(engine.executeString("result = native_add(10, 20)"));
    
    lua_getglobal(engine.getLuaState(), "result");
    EXPECT_EQ(lua_tonumber(engine.getLuaState(), -1), 30);
}

TEST_F(NativeFunctionsTest, NativeAddWithNegativeNumbers) {
    EXPECT_TRUE(engine.executeString("result = native_add(-5, 3)"));
    
    lua_getglobal(engine.getLuaState(), "result");
    EXPECT_EQ(lua_tonumber(engine.getLuaState(), -1), -2);
}

TEST_F(NativeFunctionsTest, NativeAddWithFloats) {
    EXPECT_TRUE(engine.executeString("result = native_add(1.5, 2.5)"));
    
    lua_getglobal(engine.getLuaState(), "result");
    EXPECT_DOUBLE_EQ(lua_tonumber(engine.getLuaState(), -1), 4.0);
}

TEST_F(NativeFunctionsTest, NativeTimestampReturnsPositive) {
    EXPECT_TRUE(engine.executeString("ts = native_timestamp()"));
    
    lua_getglobal(engine.getLuaState(), "ts");
    EXPECT_GT(lua_tointeger(engine.getLuaState(), -1), 0);
}

TEST_F(NativeFunctionsTest, NativeTimestampIncreases) {
    EXPECT_TRUE(engine.executeString(R"(
        ts1 = native_timestamp()
        native_sleep(10)
        ts2 = native_timestamp()
        diff = ts2 - ts1
    )"));
    
    lua_getglobal(engine.getLuaState(), "diff");
    EXPECT_GE(lua_tointeger(engine.getLuaState(), -1), 10);
}

TEST_F(NativeFunctionsTest, NativePrintDoesNotCrash) {
    EXPECT_TRUE(engine.executeString("native_print('test message')"));
}

TEST_F(NativeFunctionsTest, NativeSleepWorks) {
    EXPECT_TRUE(engine.executeString(R"(
        start = native_timestamp()
        native_sleep(50)
        elapsed = native_timestamp() - start
    )"));
    
    lua_getglobal(engine.getLuaState(), "elapsed");
    EXPECT_GE(lua_tointeger(engine.getLuaState(), -1), 50);
}

TEST_F(NativeFunctionsTest, AllFunctionsRegistered) {
    lua_State* L = engine.getLuaState();
    
    lua_getglobal(L, "native_print");
    EXPECT_TRUE(lua_isfunction(L, -1));
    lua_pop(L, 1);
    
    lua_getglobal(L, "native_add");
    EXPECT_TRUE(lua_isfunction(L, -1));
    lua_pop(L, 1);
    
    lua_getglobal(L, "native_timestamp");
    EXPECT_TRUE(lua_isfunction(L, -1));
    lua_pop(L, 1);
    
    lua_getglobal(L, "native_sleep");
    EXPECT_TRUE(lua_isfunction(L, -1));
}

TEST_F(NativeFunctionsTest, NativeSleepWithNegativeValueThrowsError) {
    EXPECT_FALSE(engine.executeString("native_sleep(-100)"));
    EXPECT_FALSE(engine.getLastError().empty());
    EXPECT_NE(engine.getLastError().find("sleep time cannot be negative"), std::string::npos);
}
