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
    string state;
    string to_state;
    int def_move;
    vector<char> input;
    vector<int> input_index;
    vector<char> output;
    vector<int> output_index;
    vector<int> move;
};


struct IncompleteSet{
    IncompleteSet(const string& state, const string& to_state);
    string state;
    string to_state;
    vector<IncompleteTransition> transitions;
};

class TuringTools {
public:
    TuringTools(unsigned int stack_tape);
    void go_to(IncompleteSet& a, const vector<char>& symbol, int tape_index, int direction);
    void go_to(IncompleteSet& a, const vector<char>& symbol, int tape_index, int direction, const vector<int>& affected);
    static void link(IncompleteSet& a, const IncompleteSet& b);
    static void link_put(IncompleteSet& a, const IncompleteSet& b,
                         const vector<char>& output, const vector<int>& output_index);

    void link_put(IncompleteSet& a, const vector<char>& output, const vector<int>& output_index);
    static void add(IncompleteSet& a, const IncompleteTransition& transition);

    void push(IncompleteTransition& transition, char symbol);
    void push(IncompleteSet& a, char symbol);
    void stack_replace(IncompleteSet& a, const vector<char>&input, const vector<char>& output);
    void move(IncompleteSet& a, unsigned int tape, int direction);
    void copy(IncompleteSet& a, unsigned int from_tape, unsigned int to_tape);
    void link_on(IncompleteSet& a, const IncompleteSet& b, const vector<char>&input, const vector<int>& input_index);
    void clear_stack(IncompleteSet& a);
private:
    unsigned long goto_counter;
    unsigned long counter;
    unsigned int stack_tape;
};


#endif //TOG_TURINGTOOLS_H
