//
// Created by watson on 10/11/23.
//

#include <iostream>
#include <gtest/gtest.h>
#include <string>

#include "src/Orchestrator.h"

//string Orchestrator::file = "input/config/config.json"; //Static variables should be initialised outside the class

int main(int argc, char **argv) {

    std::cout << "main test" << std::endl;

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
