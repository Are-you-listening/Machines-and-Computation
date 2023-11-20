//
// Created by watson on 11/19/23.
//

#include "Orchestrator.h"

string Orchestrator::file = "input/config/config.json";

Orchestrator::Orchestrator() {
    createCFG()->print();
}
//Static variables should be initialised outside the class
