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
    IncompleteTransition(json data);
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
    void tokenize();
private:
    int tuple_size;
    Transition make_transition(IncompleteTransition& incomp);
    unsigned int tapes;


};


#endif //TOG_TURINGTOKENIZER_H
