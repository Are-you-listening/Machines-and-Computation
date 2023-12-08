//
// Created by tibov on 10/11/23.
//

#ifndef TOG_TMBUILDER_H
#define TOG_TMBUILDER_H
#include "src/TM/TransitieGenerators/TuringTokenizer.h"
#include "src/TM/TuringTools.h"
#include "src/TM/TransitieGenerators/TuringVarDictionary.h"
#include "src/TM/TransitieGenerators/TuringDenestify.h"
#include "src/TM/TransitieGenerators/TuringIfElseAntiNesting.h"

struct TMBuilder_output{
    vector<string> states;
    string start_state;
    string input;
    int tape_size;
    vector<Transition> productions;

};

class TMBuilder {
public:
    TMBuilder(unsigned int tuple_size);
    TMBuilder_output generateTM();
private:
    unsigned int tapes;
    TuringTools* tools;

    Transition make_transition(IncompleteTransition& incomp);
    json add_transition(Transition& transition);

};


#endif //TOG_TMBUILDER_H