cmake_minimum_required(VERSION 3.26)
project(Toi)

set(CMAKE_CXX_STANDARD 17)

add_executable(Toi main.cpp
        CFG.h
        CFG.cpp
        helper.h
        helper.cpp

        DesignByContract.h
        PDA.h
        State.h
        State.cpp
        PDA.cpp
)
