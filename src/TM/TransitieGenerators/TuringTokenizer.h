//
// Created by tibov on 03/11/23.
//

#ifndef TOG_TURINGTOKENIZER_H
#define TOG_TURINGTOKENIZER_H

#include "src/TM/TuringMachine.h"
#include "TuringGenerator.h"

class TuringTokenizer: public TuringGenerator{
public:
    TuringTokenizer();
    IncompleteSet getTransitions();

private:
    IncompleteSet tokenize_runner_productions();
    IncompleteSet tokenize();
};

#endif //TOG_TURINGTOKENIZER_H
