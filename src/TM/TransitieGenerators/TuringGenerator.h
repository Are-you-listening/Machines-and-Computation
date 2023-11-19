//
// Created by tibov on 16/11/23.
//

#ifndef TOG_TURINGGENERATOR_H
#define TOG_TURINGGENERATOR_H

#include "src/TM/TuringTools.h"

class TuringGenerator {
public:
    explicit TuringGenerator(int tuple_size);
    virtual IncompleteSet getTransitions() = 0;

protected:
    int tuple_size;
    unsigned int tapes;
    TuringTools* tools;
    vector<char> seperators;
    vector<char> special_sep;

    vector<int> get_tuple_index();
};

#endif //TOG_TURINGGENERATOR_H
