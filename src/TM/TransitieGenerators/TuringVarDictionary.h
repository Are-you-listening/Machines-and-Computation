//
// Created by tibov on 16/11/23.
//

#ifndef TOG_TURINGVARDICTIONARY_H
#define TOG_TURINGVARDICTIONARY_H

#include "TuringGenerator.h"

class TuringVarDictionary: public TuringGenerator{
public:
    TuringVarDictionary(int tuple_size);
    IncompleteSet getTransitions();
private:
    IncompleteSet storeVar();
    void check_defined(IncompleteSet &a);
    void store_defined(IncompleteSet &a);
};

#endif //TOG_TURINGVARDICTIONARY_H