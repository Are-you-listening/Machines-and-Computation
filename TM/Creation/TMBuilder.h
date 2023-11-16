//
// Created by tibov on 10/11/23.
//

#ifndef TOG_TMBUILDER_H
#define TOG_TMBUILDER_H
#include "TM/TransitieGenerators/TuringTokenizer.h"
#include "TM/TuringTools.h"
#include "TM/TransitieGenerators/TuringVarDictionary.h"
class TMBuilder {
public:
    TMBuilder(unsigned int tuple_size);
    json generateTM();
private:
    unsigned int tapes;
    TuringTools* tools;

    Transition make_transition(IncompleteTransition& incomp);
    json add_transition(Transition& transition);

};


#endif //TOG_TMBUILDER_H
