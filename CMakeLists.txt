cmake_minimum_required(VERSION 3.14)
project(Tog)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(.)
#Google Test Setup
include(FetchContent)
include(GoogleTest)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
#set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()
add_executable(
        TESTS
        test/test-main.cpp
)
target_link_libraries(
        TESTS
        GTest::gtest_main
)
gtest_discover_tests(TESTS)
#Google Test Setup END

add_executable(Tog
        main.cpp

        lib/DesignByContract.h

        lib/helper.cpp
        lib/helper.h
        lib/json.hpp
        Tokenazation.cpp 
        Tokenazation.h 
        GNF.cpp GNF.h 
        CFG.cpp CFG.h)

