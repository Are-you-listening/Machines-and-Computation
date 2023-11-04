//
// Created by tibov on 03/11/23.
//

#ifndef TOG_TURINGTOKENIZER_H
#define TOG_TURINGTOKENIZER_H
#include "TuringMachine.h"

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


class TuringTokenizer {
public:
    TuringTokenizer();
    json tokenize();
private:
    vector<IncompleteTransition> tokenize_runner_productions();

    int tuple_size;
    Transition make_transition(IncompleteTransition& incomp);
    json add_transition(Transition& transition);
    unsigned int tapes;


};


#endif //TOG_TURINGTOKENIZER_H
