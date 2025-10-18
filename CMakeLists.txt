cmake_minimum_required(VERSION 3.30)

project(PokerEngine VERSION 1.0
                DESCRIPTION "Poker engine supporting various types of poker"
                LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Fetch external libraries
include(FetchContent)

# Fetch GoogleTest
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
)

FetchContent_MakeAvailable(googletest)

########################
# PokerEngine (with source files)
########################
# file(GLOB_RECURSE POKER_ENGINE_SOURCES
#     CONFIGURE_DEPENDS
#     ${CMAKE_CURRENT_SOURCE_DIR}/src/PokerEngine/*.cpp
# )

# add_library(PokerEngine ${POKER_ENGINE_SOURCES})

# target_include_directories(PokerEngine
#     PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
# )

# add_executable(app main.cpp)
# target_link_libraries(app PRIVATE PokerEngine)

########################
# PokerEngine (header-only)
########################
add_library(PokerEngine INTERFACE)

target_include_directories(PokerEngine
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(app main.cpp)
target_link_libraries(app PRIVATE PokerEngine)

###################################
# Enable Tests
###################################
option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()