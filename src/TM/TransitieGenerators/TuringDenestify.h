//
// Created by tibov on 22/11/23.
//

#ifndef TOG_TURINGDENESTIFY_H
#define TOG_TURINGDENESTIFY_H
#include "TuringGenerator.h"

class TuringDenestify: public TuringGenerator{
public:
    TuringDenestify(int split_nesting, int max_nesting);
    IncompleteSet getTransitions();

private:
    IncompleteSet deNestify();
    void createNewFunction(IncompleteSet& a);
    void storeObjectDefinition(IncompleteSet& a);

    int split_nesting;
    int max_nesting;

};


#endif //TOG_TURINGDENESTIFY_H
