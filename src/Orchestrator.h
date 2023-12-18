//
// Created by watson on 11/19/23.
//

#ifndef TOG_ORCHESTRATOR_H
#define TOG_ORCHESTRATOR_H

#include "CFGConstructor.h"

#include <string>

using namespace std;

/**
 * Facade Class to handle interface
 */
class Orchestrator {
    friend class Config;
private:
    /**
     * Location of the Config.json file
     */
    static string file;

public:
    Orchestrator();
};

#endif //TOG_ORCHESTRATOR_H
