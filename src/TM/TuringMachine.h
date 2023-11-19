//
// Created by tibov on 02/11/23.
//

#ifndef TOG_TURINGMACHINE_H
#define TOG_TURINGMACHINE_H
#include "Tape.h"
#include <vector>
#include <map>
#include "TuringProduction.h"
#include "lib/json.hpp"
#include <fstream>
#include <set>

using json = nlohmann::json;

class TuringMachine {
public:
    TuringMachine() = default;
    explicit TuringMachine(const string &path);
    ~TuringMachine();

    void load(const string &path);
    void load(json &data);
    void load_input(const string& input, int index);
    string getTapeData(unsigned int index) const;
    void move();

    [[nodiscard]] bool isHalted() const;
    [[nodiscard]] unsigned int getTapeAmount() const;

    const string &getCurrentState() const;

private:
    vector<Tape*> tapes;
    map<string, TuringProduction*> production_trees;
    set<string> states;
    string start_state;
    string current_state;

    bool halted;
};


#endif //TOG_TURINGMACHINE_H
