//
// Created by tibov on 03/11/23.
//

#ifndef TOG_TURINGTOKENIZER_H
#define TOG_TURINGTOKENIZER_H
#include "TuringMachine.h"
#include "TuringTools.h"


class TuringTokenizer {
public:
    TuringTokenizer();
    json tokenize();
private:
    IncompleteSet tokenize_runner_productions();

    int tuple_size;
    Transition make_transition(IncompleteTransition& incomp);
    json add_transition(Transition& transition);
    unsigned int tapes;
    TuringTools* tools;
    vector<char> seperators;
    vector<char> special_sep;


};


#endif //TOG_TURINGTOKENIZER_H
