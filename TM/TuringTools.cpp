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



void TuringTools::go_to(IncompleteSet& a, const vector<char>& symbol, int tape_index, int direction) {

    IncompleteSet b("go_to_"+ to_string(goto_counter) ,"go_to_"+ to_string(goto_counter+1));
    vector<IncompleteTransition> outputs;

    IncompleteTransition moving;
    moving.state = "go_to_"+ to_string(goto_counter);
    moving.to_state = "go_to_"+ to_string(goto_counter);

    moving.def_move = direction;

    moving.output = {'\u0001'};
    moving.output_index = {(int) stack_tape};
    moving.move = {0};

    outputs.push_back(moving);

    for (char sym: symbol){
        IncompleteTransition arrived;
        arrived.state = "go_to_"+ to_string(goto_counter);
        arrived.to_state = "go_to_"+ to_string(goto_counter+1);

        arrived.input = {sym};
        arrived.input_index = {tape_index};
        arrived.def_move = 0;
        outputs.push_back(arrived);
    }

    goto_counter += 2;

    b.transitions.insert(b.transitions.end(), outputs.begin(), outputs.end());

    link(a, b);
}

void TuringTools::go_to(IncompleteSet &a, const vector<char>& symbol, int tape_index, int direction, const vector<int> &affected) {
    IncompleteSet b("go_to_"+ to_string(goto_counter) ,"go_to_"+ to_string(goto_counter+1));
    vector<IncompleteTransition> outputs;

    IncompleteTransition moving;
    moving.state = "go_to_"+ to_string(goto_counter);
    moving.to_state = "go_to_"+ to_string(goto_counter);

    moving.def_move = 0;

    moving.output_index = affected;

    for (int i =0; i<moving.output_index.size(); i++){
        moving.output.push_back('\u0001');
        moving.move.push_back(direction);

    }

    outputs.push_back(moving);

    for (char sym: symbol){
        IncompleteTransition arrived;
        arrived.state = "go_to_"+ to_string(goto_counter);
        arrived.to_state = "go_to_"+ to_string(goto_counter+1);

        arrived.input = {sym};
        arrived.input_index = {tape_index};
        arrived.def_move = 0;


        outputs.push_back(arrived);
    }

    goto_counter += 2;



    b.transitions.insert(b.transitions.end(), outputs.begin(), outputs.end());

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

void TuringTools::stack_replace(IncompleteSet &a, const vector<char> &input, const vector<char> &output) {
    string final_state = to_string(counter+input.size()+output.size()+1);
    IncompleteSet b(to_string(counter), final_state);

    IncompleteTransition to_start_check;
    to_start_check.state = to_string(counter);
    to_start_check.to_state = to_string(counter+1);
    to_start_check.def_move = 0;
    to_start_check.output = {'\u0001'};
    to_start_check.output_index = {(int) stack_tape};
    to_start_check.move = {-(int) input.size()};

    counter++;

    b.transitions.push_back(to_start_check);

    for (int i=0; i< input.size(); i++){
        char c = input[i];

        IncompleteTransition check_transition;
        check_transition.state = to_string(counter);
        check_transition.to_state = to_string(counter+1);
        check_transition.def_move = 0;
        check_transition.input = {c};
        check_transition.input_index = {(int) stack_tape};
        check_transition.output = {'\u0001'};
        check_transition.output_index = {(int) stack_tape};
        check_transition.move = {1};

        IncompleteTransition fail_transition;
        fail_transition.state = to_string(counter);
        fail_transition.to_state = final_state;
        fail_transition.def_move = 0;
        fail_transition.output = {'\u0001'};
        fail_transition.output_index = {(int) stack_tape};
        fail_transition.move = {(int) input.size()-i};

        counter++;

        b.transitions.push_back(check_transition);
        b.transitions.push_back(fail_transition);
    }

    for (int i=0; i< output.size(); i++){
        char c = output[i];

        IncompleteTransition write_transition;
        write_transition.state = to_string(counter);
        write_transition.to_state = to_string(counter+1);
        write_transition.def_move = 0;
        write_transition.output = {c};
        write_transition.output_index = {(int) stack_tape};
        write_transition.move = {1};

        counter++;


        b.transitions.push_back(write_transition);
    }
    counter++;

    link(a, b);
}

void TuringTools::push(IncompleteSet &a, char symbol) {

    link_put(a, {symbol}, {(int) stack_tape});

    a.transitions[a.transitions.size()-1].move = {1};

}

void TuringTools::move(IncompleteSet &a, unsigned int tape, int direction) {
    IncompleteTransition move;
    move.state = a.to_state;
    move.to_state = to_string(counter);
    move.def_move = 0;
    move.output = {'\u0001'};
    move.output_index = {(int) tape};
    move.move = {direction};

    counter += 1;

    add(a, move);

}

void TuringTools::copy(IncompleteSet &a, unsigned int from_tape, unsigned int to_tape) {
    IncompleteSet copy_set(to_string(counter), to_string(counter+1));
    for (int j =32; j<127; j++){
        char c = (char) j;
        IncompleteTransition copy;
        copy.state = copy_set.state;
        copy.to_state = copy_set.to_state;
        copy.def_move = 0;
        copy.input = {c};
        copy.input_index = {(int) from_tape};
        copy.output = {c};
        copy.output_index = {(int) to_tape};
        copy.move = {0};

        copy_set.transitions.push_back(copy);
    }
    counter += 2;

    link(a,copy_set);
}



