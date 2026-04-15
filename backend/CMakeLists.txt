cmake_minimum_required(VERSION 3.16)
project(lua_script_engine VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Lua library - cross-platform
set(LUA_FOUND FALSE)

# macOS Homebrew
if(APPLE AND NOT LUA_FOUND)
    execute_process(
        COMMAND brew --prefix lua
        OUTPUT_VARIABLE HOMEBREW_LUA_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
        RESULT_VARIABLE BREW_RESULT
    )
    if(BREW_RESULT EQUAL 0 AND EXISTS "${HOMEBREW_LUA_PREFIX}/lib/liblua.a")
        set(LUA_INCLUDE_DIRS "${HOMEBREW_LUA_PREFIX}/include/lua")
        set(LUA_LIBRARIES "${HOMEBREW_LUA_PREFIX}/lib/liblua.a")
        set(LUA_FOUND TRUE)
        message(STATUS "Found Lua via Homebrew: ${HOMEBREW_LUA_PREFIX}")
    endif()
endif()

# Linux/Unix pkg-config
if(NOT LUA_FOUND)
    find_package(PkgConfig QUIET)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(LUA QUIET lua5.4)
        if(NOT LUA_FOUND)
            pkg_check_modules(LUA QUIET lua)
        endif()
    endif()
endif()

# CMake find_package fallback
if(NOT LUA_FOUND)
    find_package(Lua 5.4 QUIET)
    if(LUA_FOUND)
        set(LUA_INCLUDE_DIRS ${LUA_INCLUDE_DIR})
    endif()
endif()

if(NOT LUA_FOUND AND NOT LUA_LIBRARIES)
    message(FATAL_ERROR "Lua library not found. Please install:\n"
        "  macOS: brew install lua\n"
        "  Ubuntu: sudo apt-get install liblua5.4-dev\n"
        "  Windows: choco install lua")
endif()

message(STATUS "Lua include: ${LUA_INCLUDE_DIRS}")
message(STATUS "Lua libraries: ${LUA_LIBRARIES}")

# Main executable
add_executable(lua_engine
    src/main.cpp
    src/script_engine.cpp
    src/native_functions.cpp
)

target_include_directories(lua_engine PRIVATE ${LUA_INCLUDE_DIRS})
target_link_libraries(lua_engine PRIVATE ${LUA_LIBRARIES})

# Enable testing
option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Install
install(TARGETS lua_engine DESTINATION bin)
install(DIRECTORY scripts/ DESTINATION scripts)
