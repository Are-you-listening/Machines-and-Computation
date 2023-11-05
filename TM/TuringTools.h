//
// Created by tibov on 04/11/23.
//

#ifndef TOG_TURINGTOOLS_H
#define TOG_TURINGTOOLS_H

#include <iostream>
#include <vector>
#include "lib/json.hpp"
#include "TuringProduction.h"
using namespace std;
using json = nlohmann::json;

struct Transition{
    Production production;
    string state;
    vector<char> input;
};

struct IncompleteTransition{
    IncompleteTransition(json &data);
    IncompleteTransition() = default;
    void push(char symbol, int tape_size);
    string state;
    string to_state;
    int def_move;
    vector<char> input;
    vector<int> input_index;
    vector<char> output;
    vector<int> output_index;
    vector<int> move;
};

class TuringTools {
public:
    TuringTools();
    vector<IncompleteTransition> go_to(char symbol, int tape_index, int direction);
    static IncompleteTransition link(const string& from, const string& to);
    static IncompleteTransition link_put(const string& from, const string& to,
                                         const vector<char>& output, const vector<int>& output_index);
private:
    unsigned long goto_counter;
};


#endif //TOG_TURINGTOOLS_H
