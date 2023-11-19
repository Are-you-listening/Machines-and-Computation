cmake_minimum_required(VERSION 3.14)
project(Tog)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

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

set(TestCaseFiles
        test/test-main.cpp
        test/TuringMachineTest.cpp
        test/ThreadingTest.cpp
)


set(TuringCode
        src/TM/TuringMachine.cpp
        src/TM/TuringMachine.h
        src/TM/Tape.cpp
        src/TM/Tape.h
        src/TM/TuringProduction.cpp
        src/TM/TuringProduction.h
        src/TM/TransitieGenerators/TuringTokenizer.cpp
        src/TM/TransitieGenerators/TuringTokenizer.h
        src/TM/TuringTools.cpp
        src/TM/TuringTools.h
        src/TM/Creation/TMBuilder.cpp
        src/TM/Creation/TMBuilder.h
        src/TM/TransitieGenerators/TuringVarDictionary.cpp
        src/TM/TransitieGenerators/TuringVarDictionary.h
        src/TM/TransitieGenerators/TuringGenerator.cpp
        src/TM/TransitieGenerators/TuringGenerator.h
)

set(CodeFiles
        lib/DesignByContract.h
        lib/helper.cpp
        lib/helper.h
        lib/json.hpp
        lib/kars/CFGKars.h
        lib/kars/CFGKars.cpp


        src/Tokenisation.cpp
        src/Tokenisation.h
        src/ThreadFunction.cpp
        src/ThreadFunction.h
        src/CFG.cpp
        src/CFG.h

        src/Config.h
        src/Config.cpp

        src/Orchestrator.cpp
        src/Orchestrator.h

        ${TuringCode}
        src/CFGConstructor.h
)

add_executable(TESTS
        ${CodeFiles}
        ${TestCaseFiles}
)

target_link_libraries(
        TESTS
        GTest::gtest_main
)
gtest_discover_tests(TESTS)
#Google Test Setup END


add_executable(Tog
        main.cpp
        ${CodeFiles}
        )

set(CMAKE_INSTALL_PREFIX testing)
install(TARGETS TESTS
        DESTINATION ${CMAKE_INSTALL_PREFIX}
)

install(TARGETS Tog
        DESTINATION ${CMAKE_INSTALL_PREFIX}
)


