//
// Created by tibov on 04/11/23.
//

#include "TuringTools.h"

TuringTools::TuringTools() {
    goto_counter = 0;
}

vector<IncompleteTransition> TuringTools::go_to(char symbol, int tape_index, int direction) {
    vector<IncompleteTransition> outputs;

    IncompleteTransition moving;
    moving.state = "go_to_"+ to_string(goto_counter);
    moving.to_state = "go_to_"+ to_string(goto_counter);

    moving.def_move = direction;

    IncompleteTransition arrived;
    arrived.state = "go_to_"+ to_string(goto_counter);
    arrived.to_state = "go_to_"+ to_string(goto_counter+1);

    arrived.input = {symbol};
    arrived.input_index = {tape_index};
    arrived.def_move = 0;

    outputs.push_back(moving);
    outputs.push_back(arrived);

    goto_counter += 2;

    return outputs;
}

IncompleteTransition TuringTools::link(const string &from, const string &to) {
    IncompleteTransition incomp;
    incomp.state = from;
    incomp.to_state = to;
    incomp.def_move = 0;
    return incomp;
}

IncompleteTransition
TuringTools::link_put(const string &from, const string &to, const vector<char> &output,
                      const vector<int> &output_index) {
    IncompleteTransition out = link(from, to);
    out.output = output;
    out.output_index = output_index;
    out.move = {0};
    return out;
}
