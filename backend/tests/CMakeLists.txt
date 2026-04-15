include(FetchContent)

# Fetch Google Test
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

# Test executable
add_executable(lua_engine_tests
    test_script_engine.cpp
    test_native_functions.cpp
    ../src/script_engine.cpp
    ../src/native_functions.cpp
)

target_include_directories(lua_engine_tests PRIVATE
    ${LUA_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(lua_engine_tests PRIVATE
    ${LUA_LIBRARIES}
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(lua_engine_tests)
