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

#include <thread>
#include <future>
#include <chrono>

struct TMBuilder_output{
    vector<string> states;
    string start_state;
    string input;
    int tape_size;
    vector<Transition> productions;

};

class TMBuilder {
public:
    TMBuilder(unsigned int tuple_size, bool if_else_antinesting, int split_nesting, int max_nesting);
    TMBuilder_output generateTM();
private:
    unsigned int tapes;
    TuringTools* tools;

    bool if_else_anti;

    json add_transition(Transition& transition);

    int max_nesting = 4;
    int split_nesting = 2;



};


#endif //TOG_TMBUILDER_H