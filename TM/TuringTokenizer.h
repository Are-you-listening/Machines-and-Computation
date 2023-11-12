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
    IncompleteSet tokenize();
private:
    IncompleteSet tokenize_runner_productions();

    int tuple_size;

    unsigned int tapes;
    TuringTools* tools;
    vector<char> seperators;
    vector<char> special_sep;

    vector<int> get_tuple_index();


};


#endif //TOG_TURINGTOKENIZER_H
