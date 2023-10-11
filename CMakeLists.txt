cmake_minimum_required(VERSION 3.26)
project(Tog)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Tog
        main.cpp

        test/test-main.cpp

        lib/DesignByContract.h

        lib/helper.cpp
        lib/helper.h
        lib/json.hpp
)
