//
// Created by tibov on 04/11/23.
//

#include "TuringTools.h"

IncompleteTransition::IncompleteTransition(json &data) {

    state = data["state"].get<string>();
    to_state = data["to_state"].get<string>();
    def_move = data["def_move"].get<int>();

    for (int i = 0; i<data["input"].size(); i ++){
        input.push_back(data["input"][i].get<string>()[0]);
        input_index.push_back(data["input_index"][i].get<int>());
    }

    for (int i = 0; i<data["output"].size(); i ++){
        output.push_back(data["output"][i].get<string>()[0]);
        output_index.push_back(data["output_index"][i].get<int>());
        move.push_back(data["move"][i].get<int>());
    }
}


IncompleteSet::IncompleteSet(const string &state, const string &to_state): state{state}, to_state{to_state} {

}

TuringTools::TuringTools(unsigned int stack_tape) {
    goto_counter = 0;
    counter = 0;
    this->stack_tape = stack_tape;
}



void TuringTools::go_to(IncompleteSet& a, char symbol, int tape_index, int direction) {

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

    IncompleteSet b(moving.state ,moving.state );

    b.transitions.insert(b.transitions.end(), outputs.begin(), outputs.end());
    b.to_state = arrived.to_state;

    link(a, b);
}

void TuringTools::link(IncompleteSet& a, const IncompleteSet& b) {
    IncompleteTransition incomp;
    incomp.state = a.to_state;
    incomp.to_state = b.state;
    incomp.def_move = 0;

    a.to_state = b.to_state;
    a.transitions.push_back(incomp);
    a.transitions.insert(a.transitions.end(), b.transitions.begin(), b.transitions.end());

}

void TuringTools::link_put(IncompleteSet& a, const IncompleteSet& b, const vector<char> &output,
                      const vector<int> &output_index) {
    IncompleteTransition incomp;
    incomp.state = a.to_state;
    incomp.to_state = b.state;
    incomp.def_move = 0;

    incomp.output = output;
    incomp.output_index = output_index;
    incomp.move = {0};

    a.to_state = b.to_state;
    a.transitions.push_back(incomp);
    a.transitions.insert(a.transitions.end(), b.transitions.begin(), b.transitions.end());

}

void TuringTools::link_put(IncompleteSet &a, const vector<char> &output, const vector<int> &output_index) {
    IncompleteSet b(to_string(counter),to_string(counter));
    counter++;
    link_put(a, b, output, output_index);
}

void TuringTools::add(IncompleteSet &a, const IncompleteTransition &transition) {

    a.to_state = transition.to_state;
    a.transitions.push_back(transition);
}

void TuringTools::push(IncompleteTransition &transition, char symbol) {
    transition.output.push_back(symbol);
    transition.output_index.push_back(stack_tape);
    transition.move.push_back(1);
}


