//
// Created by watson on 11/19/23.
//

#ifndef TOG_ORCHESTRATOR_H
#define TOG_ORCHESTRATOR_H

#include <string>

#include "CFGConstructor.h"

using namespace std;

/**
 * Facade Class to handle interface
 */
class Orchestrator {
    friend class Config;
private:
    static string file;

public:
    Orchestrator();
};

#endif //TOG_ORCHESTRATOR_H
