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
    static TuringTools* getInstance(unsigned int stack_tape);

    void go_to(IncompleteSet& a, const vector<char>& symbol, int tape_index, int direction);
    void go_to(IncompleteSet& a, const vector<char>& symbol, int tape_index, int direction, const vector<int>& affected);
    void go_to_clear(IncompleteSet& a, const vector<char>& symbol, int tape_index, int direction, const vector<int>& affected,
                     const vector<int>& cleared);
    static void link(IncompleteSet& a, const IncompleteSet& b);
    static void link_put(IncompleteSet& a, const IncompleteSet& b,
                         const vector<char>& output, const vector<int>& output_index);

    void link_put(IncompleteSet& a, const vector<char>& output, const vector<int>& output_index);
    static void add(IncompleteSet& a, const IncompleteTransition& transition);

    void push(IncompleteTransition& transition, char symbol);
    void push(IncompleteSet& a, char symbol);
    void stack_replace(IncompleteSet& a, const vector<char>&input, const vector<char>& output);
    void move(IncompleteSet& a, const vector<int>& tape, int direction);
    void copy(IncompleteSet& a, unsigned int from_tape, unsigned int to_tape);
    void link_on(IncompleteSet& a, const IncompleteSet& b, const vector<char>&input, const vector<int>& input_index);
    void link_on_multiple(IncompleteSet& a, const IncompleteSet& b, const vector<vector<char>>&input, const vector<int>& input_index);
    void clear_stack(IncompleteSet& a);
    void make_loop(IncompleteSet& a);
    string branch_on(IncompleteSet& a, const vector<char>&input, const vector<int>& input_index);
    void write_on(IncompleteSet& a, const vector<char>&input, const vector<int>& input_index,
                  const vector<char>&output, const vector<int>& output_index);

    void heap_push_function(IncompleteSet& a, const vector<int>&tuple_indexes);
    static void reset();
private:

    TuringTools(unsigned int stack_tape);
    inline static unique_ptr<TuringTools> _instance;
    inline static bool _instance_flag;


    unsigned long goto_counter;
    unsigned long counter;
    unsigned int stack_tape;
    unsigned int branch_counter;
};



#endif //TOG_TURINGTOOLS_H