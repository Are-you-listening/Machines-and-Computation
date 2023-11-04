//
// Created by tibov on 04/11/23.
//

#include "TuringTools.h"

TuringTools::TuringTools() {
    goto_counter = 0;
}

vector<IncompleteTransition> TuringTools::go_to(char symbol, int tape_index, int direction) {
    vector<IncompleteTransition> outputs;
    IncompleteTransition arrived;
    arrived.state = "go_to_"+ to_string(goto_counter);
    arrived.to_state = "go_to_"+ to_string(goto_counter+1);

    arrived.input = {symbol};
    arrived.input_index = {tape_index};
    arrived.def_move = 0;

    outputs.push_back(arrived);

    IncompleteTransition moving;
    moving.state = "go_to_"+ to_string(goto_counter);
    moving.to_state = "go_to_"+ to_string(goto_counter);

    moving.def_move = direction;

    outputs.push_back(moving);

    goto_counter += 2;

    return outputs;
}
