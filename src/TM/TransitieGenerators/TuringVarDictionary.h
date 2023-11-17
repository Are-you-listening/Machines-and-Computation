//
// Created by tibov on 16/11/23.
//

#ifndef TOG_TURINGVARDICTIONARY_H
#define TOG_TURINGVARDICTIONARY_H
#include "TuringGenerator.h"

class TuringVarDictionary: public TuringGenerator{
public:
    TuringVarDictionary();
    IncompleteSet getTransitions();

private:
    IncompleteSet storeVar();
};


#endif //TOG_TURINGVARDICTIONARY_H
