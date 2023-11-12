//
// Created by tibov on 10/11/23.
//

#ifndef TOG_TMBUILDER_H
#define TOG_TMBUILDER_H
#include "TM/TuringTokenizer.h"
#include "TM/TuringTools.h"

class TMBuilder {
public:
    TMBuilder(unsigned int tuple_size);
    json generateTM();
private:
    unsigned int tape_size;
    Transition make_transition(IncompleteTransition& incomp);
    json add_transition(Transition& transition);

};


#endif //TOG_TMBUILDER_H
