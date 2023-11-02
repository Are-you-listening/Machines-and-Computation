//
// Created by tibov on 02/11/23.
//

#ifndef TOG_TURINGMACHINE_H
#define TOG_TURINGMACHINE_H
#include "TM/Tape.h"
#include <vector>
#include <map>
#include "TM/TuringProduction.h"
#include "lib/json.hpp"
#include <fstream>
#include <set>

using json = nlohmann::json;

class TuringMachine {
public:
    TuringMachine(const string &path);
    void load(const string &path);
    string getTapeData(unsigned int index) const;
    void move();

    bool isHalted() const;

private:
    vector<Tape*> tapes;
    map<string, TuringProduction*> production_trees;
    set<string> states;
    string start_state;
    string current_state;

    bool halted;
};


#endif //TOG_TURINGMACHINE_H
