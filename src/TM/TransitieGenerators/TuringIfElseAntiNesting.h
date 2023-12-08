//
// Created by tibov on 07/12/23.
//

#ifndef TOG_TURINGIFELSEANTINESTING_H
#define TOG_TURINGIFELSEANTINESTING_H
#include "TuringGenerator.h"

class TuringIfElseAntiNesting: public TuringGenerator{
public:
    TuringIfElseAntiNesting(int split_nesting, int max_nesting);
    IncompleteSet getTransitions();

private:
    IncompleteSet doAction();
    int split_nesting;
    int max_nesting;
};


#endif //TOG_TURINGIFELSEANTINESTING_H
