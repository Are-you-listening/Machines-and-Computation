//
// Created by watson on 11/19/23.
//

#ifndef TOG_ORCHESTRATOR_H
#define TOG_ORCHESTRATOR_H

#include "CFGConstructor.h"
#include "src/ThreadFunction.h"

#include <string>
#include <iostream>
#include <thread>

using namespace std;

/**
 * Facade Class to handle interface
 */
class Orchestrator {
    friend class Config;
public:
    /**
     * Simple constructor
     * @param cppFile
     */
    explicit Orchestrator(const std::string &cppFile);

    /**
     * Formats the output code to a more readable format by adjusting tabs & spaces
     */
    static void tabber();

    /**
     * Run the generated code (with and without Threading) to compare it's speed
     */
    static void threadingTest();
private:
    /**
     * Location of the Config.json file
     */
    static string file;
};

#endif //TOG_ORCHESTRATOR_H
