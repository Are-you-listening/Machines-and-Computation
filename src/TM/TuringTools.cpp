//
// Created by tibov on 04/11/23.
//

#include <set>
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

bool IncompleteTransition::operator==(const IncompleteTransition &other) const{
    return other.state == this->state && other.input == this->input && other.input_index == this->input_index;
}

bool IncompleteTransition::operator<(const IncompleteTransition &other) const{
    return this->input_index < other.input_index;
}


IncompleteSet::IncompleteSet(const string& state, const string& to_state): state{state}, to_state{to_state} {

}

TuringTools::TuringTools(unsigned int stack_tape) {
    goto_counter = 0;
    counter = 0;
    branch_counter = 0;
    stack_symbol = '-';
    this->stack_tape = stack_tape;
    nesting_tokens = {'F', '0', '1', '2'};
}

TuringTools* TuringTools::getInstance(unsigned int stack_tape) {
    if (!_instance_flag){
        _instance_flag = true;
        _instance = unique_ptr<TuringTools>(new TuringTools(stack_tape));
    }
    return _instance.get();
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

void TuringTools::go_to_multiple(IncompleteSet &a, const vector<vector<char>> &symbol, const vector<int> &tape_index,
                                 int direction, const vector<int>& affected) {
    IncompleteSet b("go_to_"+ to_string(goto_counter) ,"go_to_"+ to_string(goto_counter+1));
    vector<IncompleteTransition> outputs;

    IncompleteTransition moving;
    moving.state = "go_to_"+ to_string(goto_counter);
    moving.to_state = "go_to_"+ to_string(goto_counter);

    moving.def_move = 0;

    moving.output_index = affected;

    for (int i =0; i<moving.output_index.size(); i++){
        char c = '\u0001';

        moving.output.push_back(c);
        moving.move.push_back(direction);

    }

    outputs.push_back(moving);
    for (int i=0; i<tape_index.size(); i++){
        for (char sym: symbol[i]){
            IncompleteTransition arrived;
            arrived.state = "go_to_"+ to_string(goto_counter);
            arrived.to_state = "go_to_"+ to_string(goto_counter+1);

            arrived.input = {sym};
            arrived.input_index = {tape_index[i]};
            arrived.def_move = 0;


            outputs.push_back(arrived);
        }
    }


    goto_counter += 2;



    b.transitions.insert(b.transitions.end(), outputs.begin(), outputs.end());

    link(a, b);

}

void TuringTools::go_to(IncompleteSet &a, const vector<char>& symbol, int tape_index, int direction, const vector<int> &affected) {
    go_to_clear(a, symbol, tape_index, direction, affected, {});
}

void TuringTools::go_to_clear(IncompleteSet &a, const vector<char> &symbol, int tape_index, int direction,
                              const vector<int> &affected, const vector<int> &cleared) {
    IncompleteSet b("go_to_"+ to_string(goto_counter) ,"go_to_"+ to_string(goto_counter+1));
    vector<IncompleteTransition> outputs;

    IncompleteTransition moving;
    moving.state = "go_to_"+ to_string(goto_counter);
    moving.to_state = "go_to_"+ to_string(goto_counter);

    moving.def_move = 0;

    moving.output_index = affected;

    for (int i =0; i<moving.output_index.size(); i++){
        char c = '\u0001';
        if (find(cleared.begin(), cleared.end(), moving.output_index[i]) != cleared.end()){
            c = '\u0000';
        }

        moving.output.push_back(c);
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

void TuringTools::go_to_not(IncompleteSet &a, const vector<char> &symbol, int tape_index, int direction,
                            const vector<int> &affected) {
    IncompleteSet b("go_to_"+ to_string(goto_counter) ,"go_to_"+ to_string(goto_counter+1));
    vector<IncompleteTransition> outputs;

    for (char sym: symbol){
        IncompleteTransition moving;
        moving.state = "go_to_"+ to_string(goto_counter);
        moving.to_state = "go_to_"+ to_string(goto_counter);

        moving.def_move = 0;

        moving.output_index = affected;

        for (int i =0; i<moving.output_index.size(); i++){
            char c = '\u0001';

            moving.output.push_back(c);
            moving.move.push_back(direction);

        }

        moving.input = {sym};
        moving.input_index = {tape_index};

        outputs.push_back(moving);
    }

    IncompleteTransition arrived;
    arrived.state = "go_to_"+ to_string(goto_counter);
    arrived.to_state = "go_to_"+ to_string(goto_counter+1);


    arrived.def_move = 0;


    outputs.push_back(arrived);


    goto_counter += 2;

    b.transitions.insert(b.transitions.end(), outputs.begin(), outputs.end());

    link(a, b);

}

void TuringTools::go_to_copy(IncompleteSet &a, const vector<char> &symbol, int tape_index, int direction,
                             const vector<int> &affected, int copy_to_tape, int copy_to_direction, const vector<int>& copy_affected) {

    vector<int> affected_using = affected;
    affected_using.insert(affected_using.end(), copy_affected.begin(), copy_affected.end());
    sort(affected_using.begin(), affected_using.end());

    IncompleteSet b("go_to_"+ to_string(goto_counter) ,"go_to_"+ to_string(goto_counter+1));
    vector<IncompleteTransition> outputs;

    IncompleteSet copy_linker{"go_to_copy_"+ to_string(goto_counter),"go_to_copy_"+ to_string(goto_counter)};
    copy(copy_linker, tape_index, copy_to_tape);

    outputs = copy_linker.transitions;

    IncompleteTransition copy_go_back;
    copy_go_back.state = copy_linker.to_state;
    copy_go_back.to_state = "go_to_"+ to_string(goto_counter);

    copy_go_back.def_move = 0;

    copy_go_back.output_index = affected_using;

    for (int i =0; i<copy_go_back.output_index.size(); i++){
        char c = '\u0001';
        int move_direction = direction;

        if (find(copy_affected.begin(), copy_affected.end(), copy_go_back.output_index[i]) != copy_affected.end()){
            move_direction = copy_to_direction;
        }

        copy_go_back.output.push_back(c);
        copy_go_back.move.push_back(move_direction);

    }

    outputs.push_back(copy_go_back);

    IncompleteTransition moving;
    moving.state = "go_to_"+ to_string(goto_counter);
    moving.to_state = "go_to_copy_"+ to_string(goto_counter);

    moving.def_move = 0;

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

void
TuringTools::go_to_copy_multiple(IncompleteSet &a, const vector<vector<char>> &symbol, const vector<int> &tape_index,
                                 int direction, const vector<int> &affected, int copy_to_tape, int copy_to_direction,
                                 const vector<int> &copy_affected, int index) {
    vector<int> affected_using = affected;
    affected_using.insert(affected_using.end(), copy_affected.begin(), copy_affected.end());
    sort(affected_using.begin(), affected_using.end());

    IncompleteSet b("go_to_"+ to_string(goto_counter) ,"go_to_"+ to_string(goto_counter+1));
    vector<IncompleteTransition> outputs;

    IncompleteSet copy_linker{"go_to_copy_"+ to_string(goto_counter),"go_to_copy_"+ to_string(goto_counter)};
    copy(copy_linker, index, copy_to_tape);

    outputs = copy_linker.transitions;

    IncompleteTransition copy_go_back;
    copy_go_back.state = copy_linker.to_state;
    copy_go_back.to_state = "go_to_"+ to_string(goto_counter);

    copy_go_back.def_move = 0;

    copy_go_back.output_index = affected_using;

    for (int i =0; i<copy_go_back.output_index.size(); i++){
        char c = '\u0001';
        int move_direction = direction;

        if (find(copy_affected.begin(), copy_affected.end(), copy_go_back.output_index[i]) != copy_affected.end()){
            move_direction = copy_to_direction;
        }

        copy_go_back.output.push_back(c);
        copy_go_back.move.push_back(move_direction);

    }

    outputs.push_back(copy_go_back);

    IncompleteTransition moving;
    moving.state = "go_to_"+ to_string(goto_counter);
    moving.to_state = "go_to_copy_"+ to_string(goto_counter);

    moving.def_move = 0;

    outputs.push_back(moving);

    for (int i=0; i<tape_index.size(); i++){
        for (char sym: symbol[i]){
            IncompleteTransition arrived;
            arrived.state = "go_to_"+ to_string(goto_counter);
            arrived.to_state = "go_to_"+ to_string(goto_counter+1);

            arrived.input = {sym};
            arrived.input_index = {tape_index[i]};
            arrived.def_move = 0;


            outputs.push_back(arrived);
        }
    }

    goto_counter += 2;

    b.transitions.insert(b.transitions.end(), outputs.begin(), outputs.end());

    link(a, b);

}



void TuringTools::go_to_move(IncompleteSet &a, const vector<char> &symbol, int tape_index, int direction,
                             const vector<int> &affected, int copy_to_tape, int copy_to_direction,
                             const vector<int> &copy_affected) {
    vector<int> affected_using = affected;
    affected_using.insert(affected_using.end(), copy_affected.begin(), copy_affected.end());
    sort(affected_using.begin(), affected_using.end());

    IncompleteSet b("go_to_"+ to_string(goto_counter) ,"go_to_"+ to_string(goto_counter+1));
    vector<IncompleteTransition> outputs;

    IncompleteSet copy_linker{"go_to_copy_"+ to_string(goto_counter),"go_to_copy_"+ to_string(goto_counter)};
    copy(copy_linker, tape_index, copy_to_tape);

    outputs = copy_linker.transitions;

    IncompleteTransition copy_go_back;
    copy_go_back.state = copy_linker.to_state;
    copy_go_back.to_state = "go_to_"+ to_string(goto_counter);

    copy_go_back.def_move = 0;

    copy_go_back.output_index = affected_using;

    for (int i =0; i<copy_go_back.output_index.size(); i++){
        char c = '\u0001';
        if (tape_index == copy_go_back.output_index[i]){
            c = '\u0000';
        }

        int move_direction = direction;

        if (find(copy_affected.begin(), copy_affected.end(), copy_go_back.output_index[i]) != copy_affected.end()){

            move_direction = copy_to_direction;
        }

        copy_go_back.output.push_back(c);
        copy_go_back.move.push_back(move_direction);

    }

    outputs.push_back(copy_go_back);

    IncompleteTransition moving;
    moving.state = "go_to_"+ to_string(goto_counter);
    moving.to_state = "go_to_copy_"+ to_string(goto_counter);

    moving.def_move = 0;

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
    for (int i=0; i<output_index.size(); i++){
        incomp.move.push_back(0);
    }


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

void TuringTools::move(IncompleteSet &a, const vector<int>& tape, int direction) {
    IncompleteTransition move;
    move.state = a.to_state;
    move.to_state = to_string(counter);
    move.def_move = 0;
    for (int i =0; i<tape.size(); i++){
        move.output.push_back('\u0001');
        move.move.push_back(direction);
    }

    move.output_index = tape;

    counter += 1;

    add(a, move);

}

void TuringTools::copy(IncompleteSet &a, unsigned int from_tape, unsigned int to_tape) {
    IncompleteSet copy_set(to_string(counter), to_string(counter+1));
    for (int j =31; j<127; j++){
        char c = (char) j;

        if (j == 31){
            c = '\n';
        }

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

void TuringTools::link_on(IncompleteSet &a, const IncompleteSet &b, const vector<char> &input,
                          const vector<int> &input_index) {

    string end_state = to_string(counter);
    counter++;

    IncompleteTransition condition_transition;
    condition_transition.state = a.to_state;
    condition_transition.to_state = b.state;
    condition_transition.def_move = 0;
    condition_transition.input = input;
    condition_transition.input_index = input_index;

    a.transitions.push_back(condition_transition);
    a.transitions.insert(a.transitions.end(), b.transitions.begin(), b.transitions.end());

    IncompleteTransition transition1;
    transition1.state = a.to_state;
    transition1.to_state = end_state;
    transition1.def_move = 0;

    IncompleteTransition transition2;
    transition2.state = b.to_state;
    transition2.to_state = end_state;
    transition2.def_move = 0;

    a.transitions.push_back(transition1);
    a.transitions.push_back(transition2);

    a.to_state = end_state;

}

void TuringTools::link_on_not(IncompleteSet &a, const IncompleteSet &b, const vector<char> &input,
                              const vector<int> &input_index) {
    string end_state = to_string(counter);
    counter++;

    IncompleteTransition condition_transition;
    condition_transition.state = a.to_state;
    condition_transition.to_state = b.state;
    condition_transition.def_move = 0;


    a.transitions.push_back(condition_transition);
    a.transitions.insert(a.transitions.end(), b.transitions.begin(), b.transitions.end());

    //if element in not it will skip
    IncompleteTransition transition1;
    transition1.state = a.to_state;
    transition1.to_state = end_state;
    transition1.def_move = 0;
    transition1.input = input;
    transition1.input_index = input_index;

    IncompleteTransition transition2;
    transition2.state = b.to_state;
    transition2.to_state = end_state;
    transition2.def_move = 0;

    a.transitions.push_back(transition1);
    a.transitions.push_back(transition2);

    a.to_state = end_state;

}


void TuringTools::link_on_multiple(IncompleteSet &a, const IncompleteSet &b, const vector<vector<char>> &input,
                                   const vector<int> &input_index) {
    string end_state = to_string(counter);
    counter++;

    for (int i=0; i<input.size(); i++){
        vector<char> sub_input = input[i];

        IncompleteTransition condition_transition;
        condition_transition.state = a.to_state;
        condition_transition.to_state = b.state;
        condition_transition.def_move = 0;
        condition_transition.input = sub_input;
        condition_transition.input_index = input_index;

        a.transitions.push_back(condition_transition);
    }

    a.transitions.insert(a.transitions.end(), b.transitions.begin(), b.transitions.end());

    IncompleteTransition transition1;
    transition1.state = a.to_state;
    transition1.to_state = end_state;
    transition1.def_move = 0;

    IncompleteTransition transition2;
    transition2.state = b.to_state;
    transition2.to_state = end_state;
    transition2.def_move = 0;

    a.transitions.push_back(transition1);
    a.transitions.push_back(transition2);

    a.to_state = end_state;

}


void TuringTools::clear_stack(IncompleteSet &a) {
    IncompleteSet b("clear_stack_"+ to_string(goto_counter) ,"clear_stack_"+ to_string(goto_counter+1));
    vector<IncompleteTransition> outputs;

    IncompleteTransition moving;
    moving.state = "clear_stack_"+ to_string(goto_counter);
    moving.to_state = "clear_stack_"+ to_string(goto_counter);

    moving.def_move = 0;

    moving.output_index = {(int) stack_tape};
    moving.output = {'\u0000'};
    moving.move = {-1};

    outputs.push_back(moving);

    IncompleteTransition arrived;
    arrived.state = "clear_stack_"+ to_string(goto_counter);
    arrived.to_state = "clear_stack_"+ to_string(goto_counter+1);

    arrived.input = {stack_symbol};
    arrived.input_index = {(int) stack_tape};
    arrived.def_move = 0;


    outputs.push_back(arrived);

    goto_counter += 2;



    b.transitions.insert(b.transitions.end(), outputs.begin(), outputs.end());
    move(b, {(int) stack_tape}, 1);
    link(a, b);

}

void TuringTools::make_loop(IncompleteSet &a) {
    IncompleteTransition make_loop;
    make_loop.state = a.to_state;
    make_loop.to_state = a.state;
    make_loop.def_move = 0;

    a.transitions.push_back(make_loop);

}

string TuringTools::branch_on(IncompleteSet &a, const vector<char> &input, const vector<int> &input_index) {
    string resulting = "branch_on_"+to_string(branch_counter);
    branch_counter++;

    IncompleteTransition leave_condition;
    leave_condition.state = a.to_state;
    leave_condition.to_state = resulting;
    leave_condition.def_move = 0;
    leave_condition.input_index = input_index;
    leave_condition.input = input;

    a.transitions.push_back(leave_condition);

    return resulting;
}

void TuringTools::write_on(IncompleteSet &a, const vector<char> &input, const vector<int> &input_index,
                           const vector<char> &output, const vector<int> &output_index) {
    string resulting = to_string(counter);
    counter++;
    IncompleteTransition do_on;
    do_on.state = a.to_state;
    do_on.to_state = resulting;
    do_on.def_move = 0;
    do_on.input = input;
    do_on.input_index = input_index;
    do_on.output = output;
    do_on.output_index = output_index;
    for (int i = 0; i< output_index.size(); i++){
        do_on.move.push_back(0);
    }

    IncompleteTransition do_not;
    do_not.state = a.to_state;
    do_not.to_state = resulting;
    do_not.def_move = 0;

    a.to_state = resulting;
    a.transitions.push_back(do_on);
    a.transitions.push_back(do_not);
}

void TuringTools::copy_to_working(IncompleteSet &a, const vector<int> &tuple_indexes, bool output) {
    if (heap_mode){
        throw "heap mode is not allowed";
    }

    //copy from marker 'A' or 'S' till marker 'E'
    IncompleteSet copier{"copy_to_working_main_"+ to_string(counter), "copy_to_working_main_"+ to_string(counter)};
    counter++;

    //make sure we don't copy spaces at the start
    push(copier, 'P');

    IncompleteSet copier_do{"copier_do_"+ to_string(counter), "copier_do_"+ to_string(counter)};
    counter++;

    IncompleteSet copier_do_sub{"copier_do_sub_"+ to_string(counter), "copier_do_sub_"+ to_string(counter)};
    counter++;


    for (int i = 0; i< tuple_indexes.size(); i++){
        if (i < 2){
            continue;
        }

        //copy_to_working_check has as task to not copy the prefix spaces to working
        IncompleteSet copy_to_working_check{"copy_to_working_check_" + to_string(counter), "copy_to_working_check_" + to_string(counter)};
        counter++;

        IncompleteSet copy_to_working_checkN{"copy_to_working_checkN_" + to_string(counter), "copy_to_working_checkN_" + to_string(counter)};
        counter++;

        IncompleteSet copy_to_working("copy_to_working_" + to_string(counter), "copy_to_working_" + to_string(counter));
        counter++;

        //copy to start_tapes
        copy(copy_to_working, tuple_indexes[i], 1);
        move(copy_to_working, {0, 1}, 1);

        move(copy_to_working_check, {(int) stack_tape}, -1);
        IncompleteSet pop_p{"copy_op_"+ to_string(counter), "copy_op_"+ to_string(counter)};
        counter++;

        write_on(pop_p, {'P'}, {(int) stack_tape}, {'\u0000'}, {(int) stack_tape});

        link_on_not(copy_to_working_check, pop_p, {' '}, {tuple_indexes[i]});

        link_on_not(copy_to_working_check, copy_to_working, {'P'}, {(int) stack_tape});
        go_to(copy_to_working_check, {'\u0000'},(int) stack_tape, 1, {(int) stack_tape});

        link_on_not(copy_to_working_checkN, copy_to_working_check, {'\n'}, {tuple_indexes[i]});

        if (output){
            //only copy '\n' to working for output
            link_on_not(copier_do_sub, copy_to_working_check, {'\u0000'}, {tuple_indexes[i]});
        } else{
            link_on_not(copier_do_sub, copy_to_working_checkN, {'\u0000'}, {tuple_indexes[i]});
        }


    }

    move(copier_do_sub, tuple_indexes, 1);

    link_on_multiple(copier_do, copier_do_sub, {{'A'}, {'S'}, {'\u0000'}}, {tuple_indexes[0]});
    string end_loop = branch_on(copier_do, {'E'}, {tuple_indexes[0]});
    make_loop(copier_do);
    copier_do.to_state = end_loop;
    link(copier, copier_do);
    go_to(copier, {'S', 'A'}, tuple_indexes[0], -1, tuple_indexes);

    //writes end marker
    //guaranteeing that spaces on the end will not be taken into account
    move(copier, {0,1}, -1);
    go_to_not(copier, {' '}, 1, -1, {0,1});
    move(copier, {0,1}, 1);
    go_to_clear(copier, {'\u0000'}, 1, 1, {0,1}, {0,1});
    go_to_not(copier, {'\u0000'}, 1, -1, {0,1});
    move(copier, {0,1}, 1);
    link_put(copier, {'E'}, {0});

    //clear spaces check stack
    //don't use clear to keep 'N' stored on stack
    move(copier, {(int) stack_tape}, -1);
    write_on(copier, {'P'}, {(int) stack_tape}, {'\u0000'}, {(int) stack_tape});
    go_to(copier, {'\u0000'}, stack_tape, 1, {(int) stack_tape});
    //clear_stack(copier);

    link(a,copier);

}


void TuringTools::heap_push_definer(IncompleteSet& a, const vector<int>&tuple_indexes, bool function, bool has_bracket) {
    //requires that string that is marked is seperated by at least 1 space
    IncompleteSet push_heap_action{"push_heap_"+ to_string(counter), "push_heap_"+ to_string(counter)};
    counter++;

    copy_to_working(push_heap_action, tuple_indexes);

    //from here on we will copy data from working tape and put it on the heap
    if (has_bracket){
        go_to(push_heap_action, {'('}, 1, -1, {0,1});
    }

    go_to_multiple(push_heap_action, {{'S', 'A'}, {' '}}, {0, 1}, -1, {0,1});
    write_on(push_heap_action, {'\u0000'}, {0}, {'P'}, {0});
    //link_put(push_heap_action, {'P'}, {0});

    //manipulate hierarchy in case of object type
    //make sure class::func equals class{func}
    IncompleteSet object_hierarchy{"object_hierarchy_"+ to_string(counter), "object_hierarchy_"+ to_string(counter)};
    counter++;
    //copy class object to stack

    IncompleteSet move_right_on_p{"move_right_on_p_"+ to_string(counter), "move_right_on_p_"+ to_string(counter)};
    counter++;

    move(move_right_on_p, {0,1}, 1);

    link_on(object_hierarchy, move_right_on_p, {'P'}, {0});

    go_to_copy(object_hierarchy, {':'}, 1, 1, {0, 1}, stack_tape, 1, {(int) stack_tape});
    push(object_hierarchy, '{');

    //copy return type on stack
    go_to(object_hierarchy, {'A', 'S', 'P'}, 0, -1, {0,1});

    IncompleteSet move_right_on_p2{"move_right_on_p2_"+ to_string(counter), "move_right_on_p2_"+ to_string(counter)};
    counter++;

    go_to(move_right_on_p2, {'A', 'S'}, 0, -1, {0,1});
    go_to_copy(move_right_on_p2, {' '}, 1, 1, {0, 1}, stack_tape, 1, {(int) stack_tape});
    push(move_right_on_p2, ' ');

    link_on(object_hierarchy, move_right_on_p2, {'P'}, {0});

    go_to(object_hierarchy, {'E'}, 0, 1, {0,1});
    go_to(object_hierarchy, {':'}, 1, -1, {0,1});
    move(object_hierarchy, {0,1}, 1);
    go_to_copy(object_hierarchy, {'\u0000'}, 1, 1, {0, 1}, stack_tape, 1, {(int) stack_tape});
    clear_working(object_hierarchy);

    //read data from bottom till top of stack (used as fifo)
    //use '#' in case return type is a ptr
    go_to(object_hierarchy, {'#'}, stack_tape, -1, {(int) stack_tape});
    move(object_hierarchy, {(int) stack_tape}, 2);
    go_to_move(object_hierarchy, {'\u0000'}, stack_tape, 1, {(int) stack_tape}, 1, 1, {0, 1});
    link_put(object_hierarchy, {'E'}, {0});

    go_to(object_hierarchy, {stack_symbol}, stack_tape, -1, {(int) stack_tape});
    move(object_hierarchy, {(int) stack_tape}, 1);

    //put old markings
    go_to_multiple(object_hierarchy, {{'S', 'A'}, {' ', ')'}}, {0, 1}, -1, {0,1});

    IncompleteSet skip_brackets{"skip_brackets_"+ to_string(counter), "skip_brackets_"+ to_string(counter)};
    skip_nesting(skip_brackets, stack_tape, 1, 1, -1, {0,1}, ')', '(');
    link_on(object_hierarchy, skip_brackets, {')'}, {1});

    go_to_multiple(object_hierarchy, {{'S', 'A'}, {' '}}, {0, 1}, -1, {0,1});
    write_on(object_hierarchy, {'\u0000'}, {0}, {'P'}, {0});
    go_to_multiple(object_hierarchy, {{'P'}, {'('}}, {0, 1}, 1, {0,1});
    //go_to(object_hierarchy, {' '}, 1, -1, {0,1});
    //link_put(object_hierarchy, {'P'}, {0});
    go_to(object_hierarchy, {'{'}, 1, -1, {0,1});
    move(object_hierarchy, {0,1}, 1);
    link_put(object_hierarchy, {'S'}, {0});

    //push on stack the fact that this is an object, and on bracket close we need to double remove nesting
    push(object_hierarchy, 'O');

    //string object_break = branch_on(object_hierarchy, {'O'}, {tuple_indexes[1]});
    link_on(push_heap_action, object_hierarchy, {'O'}, {tuple_indexes[1]});

    //go to heap mode
    set_heap_mode(push_heap_action, true);

    //check if this starts with A or S
    go_to(push_heap_action, {'S', 'A'}, 0, -1, {0,1});

    //do entire copy to heap
    heap_push_working(push_heap_action, function);

    link_on_multiple(a, push_heap_action, {{'A'}, {'S'}}, {tuple_indexes[0]});
}

void TuringTools::clear_working(IncompleteSet &a) {
    //clear working tapes
    go_to(a, {'E'}, 0, 1, {0,1});
    go_to_clear(a, {'A', 'S'}, 0, -1, {0,1}, {0,1});
    link_put(a, {'\u0000'}, {1});
}


void TuringTools::heap_push_working(IncompleteSet &push_heap_action, bool function) {
    //store part on heap after insert on working tape after new data

    IncompleteSet move_heap{"move_heap_"+ to_string(counter), "move_heap_"+ to_string(counter)};
    counter++;

    go_to(move_heap, {'A'}, 0, -1, {0,1});
    find_match_heap(move_heap, 'A', 'S', 0, 1);

    //go to end of nesting
    //this will end on '{' at the end of the nesting
    go_to(move_heap, {'E'}, 0, 1, {0,1});
    move(move_heap, {0,1}, 1);
    skip_nesting(move_heap, 1, 1, stack_tape, -1, {(int) stack_tape});
    go_to(move_heap, {'A'}, 0, -1, {0,1});

    move(move_heap, {(int) stack_tape}, 1);

    go_to(move_heap, {'E'}, 0, 1, {0,1});
    move(move_heap, {(int) stack_tape}, -1);
    go_to_move(move_heap, {'\u0000'}, stack_tape, -1, {(int) stack_tape}, 1, 1, {0,1});
    link_put(move_heap, {'H'}, {0});
    go_to(move_heap, {'}'}, stack_tape, 1, {(int) stack_tape});
    //this to make sure we don't continue
    go_to(move_heap, {'P', 'S', 'A'}, 0, -1, {0,1});

    link_on(push_heap_action, move_heap, {'S'}, {0});


    go_to(push_heap_action, {'P', 'E'}, 0, 1, {0,1});
    IncompleteSet on_E{"on_E_"+ to_string(counter), "on_E_"+ to_string(counter)};
    counter++;
    go_to(on_E, {'A', 'S'}, 0, -1, {0,1});
    move(on_E, {0,1}, -1);

    link_on(push_heap_action, on_E, {'E'}, {0});

    move(push_heap_action, {0,1}, 1);


    go_to(push_heap_action, {'\u0000'}, stack_tape, -1, {(int) stack_tape});

    IncompleteSet copy_to_heap_key("copy_to_heap_key_" + to_string(counter), "copy_to_heap_key_" + to_string(counter));
    counter++;
    //copies classname/function name
    copy(copy_to_heap_key, 1, stack_tape);
    move(copy_to_heap_key, {0, 1}, 1);
    move(copy_to_heap_key, {(int) stack_tape}, -1);
    make_loop_on(copy_to_heap_key, '\u0000', 0);

    link(push_heap_action, copy_to_heap_key);

    link_put(push_heap_action, {heap_sep}, {(int) stack_tape});
    move(push_heap_action, {(int) stack_tape}, -1);

    go_to(push_heap_action, {'S', 'A'}, 0, -1, {0,1});


    IncompleteSet copy_to_heap_type("copy_to_heap_type_" + to_string(counter), "copy_to_heap_type_" + to_string(counter));
    counter++;
    //copies classname/function name
    copy(copy_to_heap_type, 1, stack_tape);
    move(copy_to_heap_type, {0, 1}, 1);
    move(copy_to_heap_type, {(int) stack_tape}, -1);
    make_loop_on(copy_to_heap_type, '\u0000', 0);

    link(push_heap_action, copy_to_heap_type);

    //put right syntax in place
    link_put(push_heap_action, {'!'}, {(int) stack_tape});
    move(push_heap_action, {(int) stack_tape}, -1);
    if (function){
        link_put(push_heap_action, {'}'}, {(int) stack_tape});
        move(push_heap_action, {(int) stack_tape}, -1);
        link_put(push_heap_action, {'#'}, {(int) stack_tape});
        move(push_heap_action, {(int) stack_tape}, -1);
        link_put(push_heap_action, {'{'}, {(int) stack_tape});
        move(push_heap_action, {(int) stack_tape}, -1);
        link_put(push_heap_action, {'#'}, {(int) stack_tape});
        move(push_heap_action, {(int) stack_tape}, -1);
    }



    //check if part heap is still on working tape
    go_to(push_heap_action, {'E'}, 0, 1, {0,1});

    IncompleteSet fix_heap{"fix_heap_"+ to_string(counter), "fix_heap_"+ to_string(counter)};
    counter++;

    go_to_move(fix_heap, {'\u0000'}, 1, 1, {0,1}, stack_tape, -1, {(int) stack_tape});
    go_to_clear(fix_heap, {'E'}, 0, -1, {0,1}, {0,1});


    //string branch_break = branch_on(fix_heap, {'E'}, {0});
    link_on_not(push_heap_action, fix_heap, {'\u0000'}, {1});

    clear_working(push_heap_action);

    //store definer again on working tape
    go_to(push_heap_action, {'!'}, (int) stack_tape, 1, {(int) stack_tape});
    go_to(push_heap_action, {'#'}, (int) stack_tape, 1, {(int) stack_tape});
    move(push_heap_action, {(int) stack_tape}, -1);

    //make nesting on working tape
    if (function){
        //remove 'S' markers
        write_on(push_heap_action, {'S'}, {0}, {'\u0000'}, {0});
        go_to_copy(push_heap_action, {heap_sep}, stack_tape, -1, {(int) stack_tape}, 1, 1, {0, 1});
        link_put(push_heap_action, {'S'}, {0});
    }

    //change '!' to '#'
    go_to(push_heap_action, {'!'}, (int) stack_tape, -1, {(int) stack_tape});
    link_put(push_heap_action, {'#'}, {(int) stack_tape});

    //go back to stack mode
    //move(push_heap_action, {(int) stack_tape}, 1);
    set_heap_mode(push_heap_action, false);
}


void TuringTools::reset() {
    _instance_flag = false;
    _instance = nullptr;

}

void TuringTools::link_on_sequence(IncompleteSet &a, const IncompleteSet &b, const vector<char> &input_sequence,
                                   int input_index) {
    string final_state = to_string(counter+input_sequence.size()+2);
    IncompleteSet d(to_string(counter), final_state);

    IncompleteTransition to_start_check;
    to_start_check.state = to_string(counter);
    to_start_check.to_state = to_string(counter+1);
    to_start_check.def_move = 0;
    to_start_check.output = {'\u0001'};
    to_start_check.output_index = {input_index};
    to_start_check.move = {-(int) input_sequence.size()};

    counter++;

    d.transitions.push_back(to_start_check);

    for (int i=0; i< input_sequence.size(); i++){
        char c = input_sequence[i];

        IncompleteTransition check_transition;
        check_transition.state = to_string(counter);
        check_transition.to_state = to_string(counter+1);
        check_transition.def_move = 0;
        check_transition.input = {c};
        check_transition.input_index = {input_index};;
        check_transition.output = {'\u0001'};
        check_transition.output_index = {input_index};;
        check_transition.move = {1};

        IncompleteTransition fail_transition;
        fail_transition.state = to_string(counter);
        fail_transition.to_state = final_state;
        fail_transition.def_move = 0;
        fail_transition.output = {'\u0001'};
        fail_transition.output_index = {input_index};;
        fail_transition.move = {(int) input_sequence.size()-i};

        counter++;

        d.transitions.push_back(check_transition);
        d.transitions.push_back(fail_transition);
    }

    d.to_state = to_string(counter);
    link(d, b);

    IncompleteTransition link_back;
    link_back.state = d.to_state;
    link_back.to_state = final_state;
    link_back.def_move = 0;

    d.transitions.push_back(link_back);
    d.to_state = final_state;

    counter+=2;

    link(a, d);

}

void TuringTools::make_loop_on(IncompleteSet &a, char input, int input_index) {
    IncompleteTransition loop_transition;
    loop_transition.to_state = a.state;
    loop_transition.state = a.to_state;
    loop_transition.def_move = 0;
    loop_transition.input = {input};
    loop_transition.input_index = {input_index};

    a.transitions.push_back(loop_transition);

    IncompleteTransition end_loop;
    end_loop.state = a.to_state;
    end_loop.to_state = to_string(counter);
    counter++;
    end_loop.def_move = 0;
    add(a, end_loop);

}

void TuringTools::make_loop_on_sequence(IncompleteSet &a, const vector<char> &input_sequence, int input_index) {
    IncompleteSet go_to_start{to_string(counter), to_string(counter)};
    counter++;

    string final_state = to_string(counter+input_sequence.size()+2);
    IncompleteSet d(to_string(counter), final_state);

    IncompleteTransition to_start_check;
    to_start_check.state = to_string(counter);
    to_start_check.to_state = to_string(counter+1);
    to_start_check.def_move = 0;
    to_start_check.output = {'\u0001'};
    to_start_check.output_index = {input_index};
    to_start_check.move = {-(int) input_sequence.size()};

    counter++;

    d.transitions.push_back(to_start_check);

    for (int i=0; i< input_sequence.size(); i++){
        char c = input_sequence[i];

        IncompleteTransition check_transition;
        check_transition.state = to_string(counter);
        check_transition.to_state = to_string(counter+1);
        check_transition.def_move = 0;
        check_transition.input = {c};
        check_transition.input_index = {input_index};;
        check_transition.output = {'\u0001'};
        check_transition.output_index = {input_index};;
        check_transition.move = {1};

        IncompleteTransition fail_transition;
        fail_transition.state = to_string(counter);
        fail_transition.to_state = final_state;
        fail_transition.def_move = 0;
        fail_transition.output = {'\u0001'};
        fail_transition.output_index = {input_index};;
        fail_transition.move = {(int) input_sequence.size()-i};

        counter++;

        d.transitions.push_back(check_transition);
        d.transitions.push_back(fail_transition);
    }

    d.to_state = to_string(counter);
    counter++;

    IncompleteTransition to_start;
    to_start.state = d.to_state;
    to_start.to_state = a.state;
    to_start.def_move = 0;

    d.transitions.push_back(to_start);

    d.to_state = final_state;
    counter++;

    link(a, d);

}

void TuringTools::find_match_heap(IncompleteSet &a, char start_marker, char end_marker, int marker_tape, int data_tape, bool search_template, bool full) {
    //precondition: in heap mode
    if (!heap_mode){
        throw "not in heap mode";
    }
    IncompleteSet searcher{"heap_search_find"+ to_string(counter), "heap_search_find"+ to_string(counter)};
    counter++;

    go_to(searcher, {'#'}, stack_tape, -1, {(int) stack_tape});
    move(searcher, {(int) stack_tape}, -1);

    IncompleteSet check_match{"check_match_"+ to_string(counter), "check_match_"+ to_string(counter+1)};


    for (int j =31; j<127; j++){
        char c = (char) j;
        if (j == 31){
            c = '\n';
        }

        IncompleteTransition check_equal;
        check_equal.state = "check_match_"+ to_string(counter);
        check_equal.to_state = "check_match_"+ to_string(counter);
        check_equal.def_move = 0;

        check_equal.input = {c, c};
        check_equal.input_index = {data_tape, (int) stack_tape};
        check_equal.output = {'\u0001', '\u0001', '\u0001'};
        check_equal.output_index = {marker_tape, data_tape, (int) stack_tape};
        check_equal.move = {1, 1, -1};

        check_match.transitions.push_back(check_equal);

        IncompleteTransition end_check;
        end_check.state = "check_match_"+ to_string(counter);
        end_check.to_state = "check_match_"+ to_string(counter+1);
        end_check.def_move = 0;

        end_check.input = {c, '\u0001'};
        end_check.input_index = {data_tape, (int) stack_tape};

        check_match.transitions.push_back(end_check);
    }


    counter+=2;

    link(searcher, check_match);
    move(searcher, {marker_tape, data_tape}, 1);


    string branch_phase_1;
    if (full){
        IncompleteSet link_on_branch{"link_on_branch_"+ to_string(counter), "link_on_branch_"+ to_string(counter)};
        counter++;
        //branch on found
        branch_phase_1 = branch_on(link_on_branch, {heap_sep}, {(int) stack_tape});
        link_on(searcher, link_on_branch, {end_marker}, {marker_tape});
    }else{
        branch_phase_1 = branch_on(searcher, {end_marker}, {marker_tape});
    }



    //check if current not found is a template
    if (search_template){
        go_to(searcher, {heap_sep}, stack_tape, -1, {(int) stack_tape});
        IncompleteSet found_template{"found_template_"+ to_string(counter), "found_template_"+ to_string(counter)};
        counter++;

        IncompleteTransition to_end;
        to_end.state = found_template.to_state;
        to_end.to_state = branch_phase_1;
        to_end.def_move = 0;
        add(found_template, to_end);

        link_on_sequence(searcher, found_template, {'e','t','a','l','p','m','e','t'}, stack_tape);
    }

    string end_tape3 = "unreached";
    if (full){
        end_tape3 = branch_on(searcher, {'S'}, {marker_tape});
        //prevent brach issues
        move(searcher, {marker_tape, data_tape}, 1);
        move(searcher, {marker_tape, data_tape}, -1);
    }



    //on not found
    string end_tape = branch_on(searcher, {'\u0000'}, {(int) stack_tape});
    string end_tape2 = branch_on(searcher, {'{'}, {(int) stack_tape});

    IncompleteSet go_to_end{"find_match_to_end_"+ to_string(counter), "find_match_to_end_"+ to_string(counter)};
    counter++;
    for (auto& b: {end_tape, end_tape2, end_tape3}){
        IncompleteTransition to_end;
        to_end.state = b;
        to_end.to_state = go_to_end.state;
        to_end.def_move = 0;

        go_to_end.transitions.push_back(to_end);
    }
    go_to(go_to_end, {'\u0000'}, stack_tape, -1, {(int) stack_tape});
    move(go_to_end, {(int) stack_tape}, 2);

    IncompleteTransition to_branch;
    to_branch.state = go_to_end.to_state;
    to_branch.to_state = branch_phase_1;
    to_branch.def_move = 0;

    go_to_end.transitions.push_back(to_branch);

    searcher.transitions.insert(searcher.transitions.end(), go_to_end.transitions.begin(), go_to_end.transitions.end());

    //recreate search loop
    move(searcher, {marker_tape, data_tape}, -1);

    IncompleteSet change_stack_pos{"change_stack_pos_"+ to_string(counter), "change_stack_pos_"+ to_string(counter)};
    counter++;

    //point to first character withing the nesting
    go_to(change_stack_pos, {'#'}, stack_tape, -1, {(int) stack_tape});
    move(change_stack_pos, {(int) stack_tape}, -1);
    move(change_stack_pos, {marker_tape, data_tape}, 1);
    link_put(change_stack_pos, {'C'}, {marker_tape});

    link_on(searcher, change_stack_pos, {'{'}, {data_tape});

    make_loop_on(searcher, '}', stack_tape);

    go_to(searcher, {'\u0000'}, 1, 1, {0,1});
    move(searcher, {0,1}, 1);

    //skip nesting that we don't care about
    //only skip if it has a nesting
    go_to(searcher, {'#'}, stack_tape, -1, {(int) stack_tape});
    move(searcher, {(int) stack_tape}, -1);

    IncompleteSet find_match_skip_nesting{"find_match_skip_nesting_"+ to_string(counter), "find_match_skip_nesting_"+ to_string(counter)};
    move(find_match_skip_nesting, {(int) stack_tape}, 1);
    skip_nesting(find_match_skip_nesting, 1, 1, stack_tape,-1, {(int) stack_tape});
    move(find_match_skip_nesting, {(int) stack_tape}, -1);

    link_on(searcher, find_match_skip_nesting, {'}'}, {(int) stack_tape});
    move(searcher, {(int) stack_tape}, 1);

    go_to(searcher, {start_marker, 'C'}, 0, -1, {0,1});
    //loop somewhere here
    make_loop(searcher);
    //after loop on found
    searcher.to_state = branch_phase_1;
    go_to(searcher, {'#'}, stack_tape, -1, {(int) stack_tape});
    move(searcher, {(int) stack_tape}, -1);

    //clear 'C' characters on marker tape
    move(searcher, {marker_tape, data_tape}, -1);
    go_to_clear(searcher, {start_marker}, marker_tape, -1, {marker_tape, data_tape}, {marker_tape});
    go_to(searcher, {end_marker}, marker_tape, 1, {marker_tape, data_tape});
    link(a, searcher);

}

void TuringTools::find_match_heap_traverse(IncompleteSet &a, char start_marker, char end_marker, int marker_tape,
                                           int data_tape, bool full) {
    //requires heap mode
    //also check for variables before and after nesting
    //after should not matter for variables, just for functions
    //for functions we want to guarantee uniqueness
    find_match_heap(a, start_marker, end_marker, marker_tape, data_tape, false, full);


    IncompleteSet do_traverse_check{"do_traverse_check_"+ to_string(counter), "do_traverse_check_"+ to_string(counter)};
    counter++;

    go_to_not(do_traverse_check, {'\u0000'}, stack_tape, 1, {(int) stack_tape});

    go_to(do_traverse_check, {'A'}, marker_tape, -1, {marker_tape, data_tape});

    set_heap_mode(do_traverse_check, false);
    push(do_traverse_check, stack_sep);
    go_to_copy(do_traverse_check, {'\u0000'}, data_tape, 1, {marker_tape, data_tape}, stack_tape, 1, {(int) stack_tape});

    set_heap_mode(do_traverse_check, true);

    IncompleteSet traverse_loop{"traverse_loop_"+ to_string(counter), "traverse_loop_"+ to_string(counter)};
    counter++;

    set_heap_mode(traverse_loop, false);

    link_put(traverse_loop, {'\u0000'}, {marker_tape});
    move(traverse_loop, {marker_tape, data_tape}, -1);
    copy(traverse_loop, data_tape, stack_tape);
    move(traverse_loop, {(int) stack_tape}, 1);
    link_put(traverse_loop, {'\u0000'}, {data_tape});
    move(traverse_loop, {marker_tape, data_tape}, -1);
    go_to_move(traverse_loop, {'{'}, data_tape, -1, {marker_tape, data_tape}, stack_tape, 1, {(int) stack_tape});

    //now clear 1 var from the path
    link_put(traverse_loop, {'\u0000'}, {data_tape});
    move(traverse_loop, {marker_tape, data_tape}, -1);
    go_to_clear(traverse_loop, {'{', '\u0000'}, data_tape, -1, {marker_tape, data_tape}, {data_tape});
    move(traverse_loop, {marker_tape, data_tape}, 1);

    //push lowest root again on working
    move(traverse_loop, {(int) stack_tape}, -1);
    go_to_move(traverse_loop, {'{', stack_sep}, stack_tape, -1, {(int) stack_tape}, 1, 1, {marker_tape, data_tape});
    link_put(traverse_loop, {'{', '\u0000'}, {data_tape, (int) stack_tape});

    move(traverse_loop, {marker_tape, data_tape}, 1);
    link_put(traverse_loop, {'S'}, {marker_tape});

    go_to(traverse_loop, {'A'}, 0, -1, {marker_tape, data_tape});
    set_heap_mode(traverse_loop, true);
    find_match_heap(traverse_loop, start_marker, end_marker, marker_tape, data_tape, false, full);
    //still need to move last key

    go_to(traverse_loop, {'A'}, marker_tape, -1, {marker_tape, data_tape});
    go_to(traverse_loop, {'{'}, data_tape, 1, {marker_tape, data_tape});
    move(traverse_loop, {0, 1}, 1);
    string not_found_branch = branch_on(traverse_loop, {'S', '\u0000'}, {marker_tape, (int) stack_tape});

    //in case the last one and also found target
    string issue_fixer = branch_on(traverse_loop, {'S'}, {marker_tape});

    IncompleteSet betweener{"betweener_"+ to_string(counter), "betweener_"+ to_string(counter)};
    link(traverse_loop,betweener);

    IncompleteTransition fix_transition;
    fix_transition.state = issue_fixer;
    fix_transition.to_state = traverse_loop.to_state;
    fix_transition.def_move = 0;

    traverse_loop.transitions.push_back(fix_transition);

    //string not_found_branch = branch_on(traverse_loop, {'\u0000'}, {(int) stack_tape});
    go_to(traverse_loop, {'S'}, marker_tape, 1, {marker_tape, data_tape});


    make_loop_on(traverse_loop, '\u0000', stack_tape);

    IncompleteTransition not_found_to_now;
    not_found_to_now.state = not_found_branch;
    not_found_to_now.to_state = traverse_loop.to_state;
    not_found_to_now.def_move = 0;

    traverse_loop.transitions.push_back(not_found_to_now);


    link(do_traverse_check, traverse_loop);

    //clear stack
    move(do_traverse_check, {(int) stack_tape}, 1);
    link_put(do_traverse_check, {'!'}, {(int) stack_tape});

    set_heap_mode(do_traverse_check, false);


    go_to_clear(do_traverse_check, {'A'}, marker_tape, -1, {marker_tape, data_tape}, {marker_tape, data_tape});
    go_to(do_traverse_check, {stack_sep}, stack_tape, -1, {(int) stack_tape});
    move(do_traverse_check, {(int) stack_tape}, 1);
    go_to_move(do_traverse_check, {'\u0000'}, stack_tape, 1, {(int) stack_tape}, data_tape, 1, {marker_tape, data_tape});
    go_to(do_traverse_check, {stack_sep}, stack_tape, -1, {(int) stack_tape});
    link_put(do_traverse_check, {'S', '\u0000'}, {marker_tape, (int) stack_tape});

    //go back to old heap pos
    set_heap_mode(do_traverse_check, true);
    go_to(do_traverse_check, {'!'}, stack_tape, -1, {(int) stack_tape});
    link_put(do_traverse_check, {'#'}, {(int) stack_tape});
    move(do_traverse_check, {(int) stack_tape}, -1);


    link_on(a, do_traverse_check, {'\u0000'}, {(int) stack_tape});


}

void TuringTools::skip_nesting(IncompleteSet &a, int new_stack_tape, int
stack_direction, int skip_tape, int skip_direction, const vector<int>& affected, char i, char j) {
    /*
     * case 1: found '{' has '0' on stack -> pop 0
     * case 2: found '{' has not '0' on stack -> push 1
     * case 3: found '}' has '1' on stack -> pop 1
     * case 4: found '}' has not '1' on stack -> push 0
     * i = '{'
     * j = '}'
     * */
    move(a, {new_stack_tape}, stack_direction);
    link_put(a, {'#'}, {new_stack_tape});

    IncompleteSet result{"skip_nesting_"+ to_string(counter), "skip_nesting_"+ to_string(counter)};
    counter++;

    go_to(result, {i, j}, skip_tape, skip_direction, affected);

    IncompleteSet case_1{"skip_nesting_"+ to_string(counter), "skip_nesting_"+ to_string(counter)};
    IncompleteSet case_2{"skip_nesting_"+ to_string(counter+1), "skip_nesting_"+ to_string(counter+1)};
    IncompleteSet case_3{"skip_nesting_"+ to_string(counter+2), "skip_nesting_"+ to_string(counter+2)};
    IncompleteSet case_4{"skip_nesting_"+ to_string(counter+3), "skip_nesting_"+ to_string(counter+3)};

    IncompleteSet case_handler_1{"skip_nesting_"+ to_string(counter+4), "skip_nesting_"+ to_string(counter+4)};
    IncompleteSet case_handler_2{"skip_nesting_"+ to_string(counter+5), "skip_nesting_"+ to_string(counter+5)};
    counter+=6;


    link_put(case_1, {'\u0000'}, {new_stack_tape});
    move(case_1, {new_stack_tape}, -1*stack_direction);
    write_on(case_1, {'#'}, {new_stack_tape}, {'\u0000'}, {new_stack_tape});

    move(case_2, {new_stack_tape}, stack_direction);
    link_put(case_2, {'1'}, {new_stack_tape});

    link_put(case_3, {'\u0000'}, {new_stack_tape});
    move(case_3, {new_stack_tape}, -1*stack_direction);
    write_on(case_3, {'#'}, {new_stack_tape}, {'\u0000'}, {new_stack_tape});

    move(case_4, {new_stack_tape}, stack_direction);
    link_put(case_4, {'0'}, {new_stack_tape});

    link_on(case_handler_1, case_1, {'0'}, {new_stack_tape});
    link_on_multiple(case_handler_1, case_2, {{'1'}, {'#'}}, {new_stack_tape});

    link_on(case_handler_2, case_3, {'1'}, {new_stack_tape});
    link_on_multiple(case_handler_2, case_4, {{'0'}, {'#'}}, {new_stack_tape});

    link_on(result, case_handler_1, {i}, {skip_tape});
    link_on(result, case_handler_2, {j}, {skip_tape});

    string loop_end = branch_on(result, {'\u0000'}, {new_stack_tape});
    move(result, affected, skip_direction);
    make_loop(result);
    result.to_state = loop_end;
    move(result, {new_stack_tape}, -1*stack_direction);
    link(a, result);
}

void TuringTools::push_on_sequence(IncompleteSet &a, const vector<char> &input_sequence,
                                   int input_index, char push_char) {
    IncompleteSet pusher("push_on_sequence"+ to_string(counter), "push_on_sequence"+ to_string(counter));
    counter++;
    push(pusher, push_char);
    link_on_sequence(a, pusher, input_sequence, input_index);

}

void TuringTools::set_heap_mode(IncompleteSet &a, bool to_heap) {
    if (to_heap){
        if (heap_mode){
            throw "already heap";
        }
        IncompleteSet to_heap_mode{"to_heap_mode_"+ to_string(counter), "to_heap_mode_"+ to_string(counter)};
        counter++;
        go_to(to_heap_mode, {stack_symbol}, stack_tape, -1, {(int) stack_tape});
        go_to(to_heap_mode, {'#'}, stack_tape, -1, {(int) stack_tape});
        heap_mode = true;
        link(a, to_heap_mode);
    }else{
        if (!heap_mode){
            throw "already stack";
        }
        IncompleteSet from_heap_mode{"from_heap_mode_"+ to_string(counter), "from_heap_mode_"+ to_string(counter)};
        counter++;
        go_to(from_heap_mode, {'#'}, stack_tape, 1, {(int) stack_tape});
        go_to(from_heap_mode, {'\u0000'}, stack_tape, 1, {(int) stack_tape});
        heap_mode = false;
        link(a, from_heap_mode);
    }


}

void
TuringTools::nesting_marker(IncompleteSet &a, const vector<int> &tuple_indexes, int split_nesting, int max_nesting) {
    IncompleteSet result{"nesting_marking_"+ to_string(counter), "nesting_marking_"+ to_string(counter+max_nesting+1)};
    int original_counter = counter;
    counter += max_nesting+2;
    vector<IncompleteTransition> end_loops;
    end_loops.reserve(max_nesting+1);

    //create states till max nesting
    for (int i=0; i<max_nesting+1; i++){

        IncompleteSet forward_action{"forward_action_"+ to_string(original_counter), "forward_action_"+ to_string(original_counter)};
        if (i <= split_nesting || true){
            move(forward_action, tuple_indexes, -1);
            mark_definer(forward_action, tuple_indexes);

            IncompleteSet store_definer{"store_definer_"+ to_string(counter), "store_definer_"+ to_string(counter)};
            counter++;

            make_working_nesting(store_definer, tuple_indexes);


            //clear old S marker
            move(store_definer, {0,1}, -1);
            go_to(store_definer, {'A', 'S'}, 0, -1, {0,1});
            write_on(store_definer, {'S'}, {0}, {'\u0000'}, {0});
            go_to(store_definer, {'S'}, 0, 1, {0,1});


            link_on_multiple(forward_action, store_definer, {{'C'}, {'U'}, {'O'}}, {tuple_indexes[1]});

            add_nesting_working(forward_action);

            //marker cleanup
            go_to(forward_action, {'S', 'A'}, tuple_indexes[0], -1, tuple_indexes);
            write_on(forward_action, {'S'}, {tuple_indexes[0]}, {'\u0000'}, {tuple_indexes[0]});
            move(forward_action, tuple_indexes, 1);
            go_to_clear(forward_action, {'E'}, tuple_indexes[0], 1, tuple_indexes, {tuple_indexes[0]});
            write_on(forward_action, {'E'}, {tuple_indexes[0]}, {'\u0000'}, {tuple_indexes[0]});

            if (i == split_nesting){
                write_on(forward_action, {'\u0000'}, {tuple_indexes[0]}, {'N'}, {tuple_indexes[0]});
            }

            move(forward_action, tuple_indexes, 1);
        }


        IncompleteTransition go_next;
        go_next.state = forward_action.to_state;
        go_next.def_move = 0;
        go_next.to_state = "nesting_marking_"+ to_string(original_counter+1);

        forward_action.transitions.push_back(go_next);

        IncompleteSet backward_action{"backward_action_"+ to_string(original_counter+1), "backward_action_"+ to_string(original_counter+1)};
        //move(backward_action, tuple_indexes, -1);
        remove_nesting_working(backward_action);
        //move(backward_action, tuple_indexes, 1);
        IncompleteTransition go_back;
        go_back.state = backward_action.to_state;
        go_back.def_move = 0;
        go_back.to_state = "nesting_marking_"+ to_string(original_counter);

        backward_action.transitions.push_back(go_back);

        IncompleteTransition go_forward;
        go_forward.state = "nesting_marking_"+ to_string(original_counter);
        go_forward.to_state = forward_action.state;
        go_forward.def_move = 0;

        go_forward.input = {'{'};
        go_forward.input_index = {tuple_indexes[1]};

        for (auto t: tuple_indexes){
            char c = '\u0001';
            go_forward.output.push_back(c);
            go_forward.output_index.push_back(t);
            go_forward.move.push_back(1);
        }

        IncompleteTransition stay;
        stay.state = "nesting_marking_"+ to_string(original_counter);
        stay.to_state = "nesting_marking_"+ to_string(original_counter);
        stay.def_move = 0;

        for (auto t: tuple_indexes){
            stay.output.push_back('\u0001');
            stay.output_index.push_back(t);
            stay.move.push_back(1);
        }


        IncompleteTransition go_backwards;
        go_backwards.state = "nesting_marking_"+ to_string(original_counter+1);
        go_backwards.to_state = backward_action.state;
        go_backwards.def_move = 0;

        go_backwards.input = {'}'};
        go_backwards.input_index = {tuple_indexes[1]};

        for (auto t: tuple_indexes){
            go_backwards.output.push_back('\u0001');
            go_backwards.output_index.push_back(t);
            go_backwards.move.push_back(1);
        }

        IncompleteTransition end_loop;
        end_loop.state = "nesting_marking_"+ to_string(original_counter);
        end_loop.to_state = "nesting_marking_"+ to_string(original_counter);
        end_loop.def_move = 0;

        end_loop.input = {'\u0000'};
        end_loop.input_index = {tuple_indexes[1]};

        end_loops.push_back(end_loop);


        result.transitions.push_back(go_forward);
        result.transitions.push_back(stay);
        result.transitions.push_back(go_backwards);

        result.transitions.insert(result.transitions.end(), forward_action.transitions.begin(), forward_action.transitions.end());
        result.transitions.insert(result.transitions.end(), backward_action.transitions.begin(), backward_action.transitions.end());


        original_counter++;
    }

    original_counter+=2;


    //go till end of nesting

    //go back loop
    IncompleteSet go_back{"go_back_"+ to_string(counter), "go_back_"+ to_string(counter)};
    counter++;

    go_to_multiple(go_back, {{'N'}, {'{'}}, {tuple_indexes[0], tuple_indexes[1]}, -1, tuple_indexes);
    string go_back_branch = branch_on(go_back, {'N'}, {tuple_indexes[0]});

    remove_nesting_working(go_back);
    move(go_back, tuple_indexes, -1);

    make_loop(go_back);
    go_back.to_state = go_back_branch;
    link(result, go_back);

    skip_nesting(result, stack_tape, 1, tuple_indexes[1], 1, tuple_indexes);

    string final = result.to_state;

    for (auto& e: end_loops){
        e.to_state = final;
        result.transitions.push_back(e);

    }

    link_put(result, {'U'}, {tuple_indexes[0]});

    link(a, result);

}

void TuringTools::make_working_nesting(IncompleteSet &a, const vector<int> &tuple_indexes) {

    IncompleteSet working_nesting{"make_working_nesting_"+ to_string(counter), "make_working_nesting_"+ to_string(counter)};
    counter++;

    copy_to_working(working_nesting, tuple_indexes);

    move(working_nesting, {0, 1}, -1);

    IncompleteSet on_bracket{"on_bracket_"+ to_string(counter), "on_bracket_"+ to_string(counter)};
    counter++;



    go_to_copy(on_bracket, {'('}, 1, -1, {0,1}, stack_tape, 1, {(int) stack_tape});
    copy(on_bracket, 1, stack_tape);
    move(on_bracket, {(int) stack_tape}, 1);
    move(on_bracket, {0, 1}, -1);
    link_on(working_nesting, on_bracket, {')'}, {1});

    go_to_copy(working_nesting, {' ', ':'}, 1, -1, {0,1}, stack_tape, 1, {(int) stack_tape});

    IncompleteSet copy_object{"make_working_nesting_"+ to_string(counter), "make_working_nesting_"+ to_string(counter)};
    counter++;

    push(copy_object, ':');
    go_to_not(copy_object, {':'}, 1, -1, {0, 1});
    go_to_copy_multiple(copy_object, {{'A', 'S'}, {' '}}, {0, 1}, -1, {0, 1}, stack_tape, 1, {(int) stack_tape}, 1);

    IncompleteSet copy_last_char{"copy_last_char_"+ to_string(counter), "copy_last_char_"+ to_string(counter)};
    copy(copy_last_char, 1, stack_tape);
    move(copy_last_char, {(int) stack_tape}, 1);

    link_on_multiple(copy_object, copy_last_char, {{'S'}, {'A'}}, {0});

    link_on(working_nesting, copy_object, {':'}, {1});

    go_to(working_nesting, {'E'}, 0, 1, {0,1});
    go_to_clear(working_nesting, {'S', 'A'}, 0, -1, {0,1}, {0,1});

    link_put(working_nesting, {'\u0000'}, {1});

    move(working_nesting, {(int) stack_tape}, -1);

    go_to_move(working_nesting, {stack_symbol, '{', ':', '('}, stack_tape, -1, {(int) stack_tape}, 1, 1, {0,1});


    IncompleteSet replace_double_dot{"replace_double_dot_"+ to_string(counter), "replace_double_dot_"+ to_string(counter)};
    counter++;
    //replaces ':' on stack by '{' on working tape
    link_put(replace_double_dot, {'{'}, {1});
    move(replace_double_dot, {0, 1}, 1);
    link_put(replace_double_dot, {'\u0000'}, {(int) stack_tape});
    move(replace_double_dot, {(int) stack_tape}, -1);

    go_to_move(replace_double_dot, {stack_symbol, '{'}, stack_tape, -1, {(int) stack_tape}, 1, 1, {0,1});
    move(replace_double_dot, {(int) stack_tape}, 1);
    push(replace_double_dot, 'O');
    move(replace_double_dot, {(int) stack_tape}, -1);

    link_on(working_nesting, replace_double_dot, {':'}, {(int) stack_tape});

    //ignores double dot between brackets coming from nemaspaces
    IncompleteSet skip_brackets{"skip_brackets_"+ to_string(counter), "skip_brackets_"+ to_string(counter)};
    counter++;
    go_to_move(skip_brackets, {stack_symbol, '{'}, stack_tape, -1, {(int) stack_tape}, 1, 1, {0,1});
    link_on(working_nesting, skip_brackets, {'('}, {(int) stack_tape});


    move(working_nesting, {(int) stack_tape}, 1);

    link_put(working_nesting, {'S'}, {0});

    go_to(working_nesting, {'E'}, tuple_indexes[0], 1, tuple_indexes);


    link(a, working_nesting);

}

void TuringTools::mark_definer(IncompleteSet &a, const vector<int> &tuple_indexes) {
    //precondition current symbol is an '{'
    IncompleteSet working_nesting{"mark_definer_"+ to_string(counter), "mark_definer_"+ to_string(counter)};
    counter++;
    write_on(working_nesting, {'\u0000'}, {tuple_indexes[0]}, {'E'}, {tuple_indexes[0]});
    write_on(working_nesting, {'S'}, {tuple_indexes[0]}, {'E'}, {tuple_indexes[0]});

    move(working_nesting, tuple_indexes, -1);
    std::vector<char> declares = {'O','C','U','I'};
    declares.insert(declares.end(), nesting_tokens.begin(), nesting_tokens.end());
    go_to(working_nesting, declares, tuple_indexes[1], -1, tuple_indexes);

    write_on(working_nesting, {'\u0000'}, {tuple_indexes[0]}, {'S'}, {tuple_indexes[0]});
    write_on(working_nesting, {'E'}, {tuple_indexes[0]}, {'S'}, {tuple_indexes[0]});

    link(a, working_nesting);
}

void TuringTools::add_nesting_working(IncompleteSet &a) {
    IncompleteSet add_nest{"add_nesting_"+ to_string(counter), "add_nesting_"+ to_string(counter)};
    counter++;
    //create own sub nesting
    IncompleteSet create_key{"create_key_"+ to_string(counter), "create_key_"+ to_string(counter)};
    counter++;
    move(create_key, {0,1}, 1);

    push(create_key, 'N');
    push(create_key, '{');


    move(create_key, {(int) stack_tape}, -2);
    go_to(create_key, {stack_symbol, '{', 'O'}, stack_tape, -1, {(int) stack_tape});
    move(create_key, {(int) stack_tape}, 1);
    write_on(create_key, {'S'}, {0}, {'\u0000'}, {0});
    go_to_copy(create_key, {'{'}, stack_tape, 1, {(int) stack_tape}, 1, 1, {0, 1});
    move(create_key, {0,1}, -1);
    move(create_key, {(int) stack_tape}, 1);

    //check if already curly bracket present
    move(add_nest, {0,1}, -1);
    link_on(add_nest, create_key, {'{'}, {1});
    move(add_nest, {0,1}, 1);

    link_put(add_nest, {'{'}, {1});
    write_on(add_nest, {'S'}, {0}, {'\u0000'}, {0});
    move(add_nest, {0,1}, 1);
    link_put(add_nest, {'S'}, {0});

    link(a, add_nest);

}

void TuringTools::remove_nesting_working(IncompleteSet &a) {
    //clear nesting bracket itself, and all within
    go_to_clear(a, {'{'}, 1, -1, {0,1}, {0,1});
    link_put(a, {'\u0000'}, {1});


    //check current stack symbol
    move(a, {(int) stack_tape}, -1);

    //if top is '{' of stack -> POP
    IncompleteSet pop{"pop_curly_"+ to_string(counter), "pop_curly_"+ to_string(counter)};
    counter++;
    go_to_clear(pop, {'{', 'O', stack_symbol}, stack_tape, -1, {(int) stack_tape}, {(int) stack_tape});
    write_on(pop, {'{'}, {(int) stack_tape}, {'\u0000'}, {(int) stack_tape});
    go_to_not(pop, {'\u0000'}, (int) stack_tape, -1, {(int) stack_tape});

    link_on_multiple(a, pop, {{'{'}, {'N'}}, {(int) stack_tape});

    //check if stack has 'O' charcter saying we need to pop
    IncompleteSet clear_again{"clear_again_"+ to_string(counter), "clear_again_"+ to_string(counter)};
    counter++;
    move(clear_again, {0,1}, -1);
    go_to_clear(clear_again, {'{'}, 1, -1, {0,1}, {0,1});
    link_put(clear_again, {'\u0000'}, {1});
    link_put(clear_again, {'\u0000'}, {(int) stack_tape});

    link_on(a, clear_again, {'O'}, {(int) stack_tape});

    //put stack back on right spot
    move(a, {(int) stack_tape}, 1);

    //clears what is before cleared bracket
    go_to_multiple(a, {{'A'}, {'{'}}, {0,1}, -1, {0,1});

    move(a, {0,1}, 1);
    go_to_clear(a, {'\u0000'}, 1, 1, {0,1}, {0,1});
    go_to_multiple(a, {{'A'}, {'{'}}, {0,1}, -1, {0,1});

    IncompleteSet remove_first_char{"remove_first_char_"+to_string(counter), "remove_first_char_"+to_string(counter)};
    counter++;
    link_put(remove_first_char, {'\u0000'}, {1});

    link_on(a, remove_first_char, {'A'}, {0});

    IncompleteSet move_right{"remove_nesting_move_right_"+to_string(counter), "remove_nesting_move_right_"+to_string(counter)};
    counter++;

    move(move_right, {0,1}, 1);

    link_on(a, move_right, {'{'}, {1});

    write_on(a, {'\u0000'}, {0}, {'S'}, {0});

}

void TuringTools::copy_any(IncompleteSet &a, unsigned int from_tape, unsigned int to_tape) {
    //also copies blank
    IncompleteSet copy_set(to_string(counter), to_string(counter+1));
    for (int j =31; j<128; j++){
        char c = (char) j;
        if (j == 127){
            c = '\u0000';
        }
        if (j == 31){
            c = '\n';
        }

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

void TuringTools::copy_till(IncompleteSet& a, const vector<char>&symbols, int check_tape, int from_tape,
                            int to_tape, int direction, const vector<int>& affected) {

    vector<int> affected_using = affected;
    sort(affected_using.begin(), affected_using.end());

    IncompleteSet b("go_to_"+ to_string(goto_counter) ,"go_to_"+ to_string(goto_counter+1));
    vector<IncompleteTransition> outputs;

    IncompleteSet copy_linker{"go_to_copy_"+ to_string(goto_counter),"go_to_copy_"+ to_string(goto_counter)};
    copy_any(copy_linker, from_tape, to_tape);

    outputs = copy_linker.transitions;

    IncompleteTransition copy_go_back;
    copy_go_back.state = copy_linker.to_state;
    copy_go_back.to_state = "go_to_"+ to_string(goto_counter);

    copy_go_back.def_move = 0;

    copy_go_back.output_index = affected_using;

    for (int i =0; i<copy_go_back.output_index.size(); i++){
        char c = '\u0001';
        int move_direction = direction;

        copy_go_back.output.push_back(c);
        copy_go_back.move.push_back(move_direction);

    }

    outputs.push_back(copy_go_back);

    IncompleteTransition moving;
    moving.state = "go_to_"+ to_string(goto_counter);
    moving.to_state = "go_to_copy_"+ to_string(goto_counter);

    moving.def_move = 0;

    outputs.push_back(moving);

    for (char sym: symbols){
        IncompleteTransition arrived;
        arrived.state = "go_to_"+ to_string(goto_counter);
        arrived.to_state = "go_to_"+ to_string(goto_counter+1);

        arrived.input = {sym};
        arrived.input_index = {check_tape};
        arrived.def_move = 0;


        outputs.push_back(arrived);
    }

    goto_counter += 2;

    b.transitions.insert(b.transitions.end(), outputs.begin(), outputs.end());

    link(a, b);

}

void TuringTools::write_function_header(IncompleteSet &a, const vector<int>&tuple_indexes) {
    //requires: startpos S on working tape
    //requires: startpos N on tuple tape for later var check
    IncompleteSet write_function_header{"write_function_header_"+ to_string(counter), "write_function_header_"+ to_string(counter)};
    counter++;

    link_put(write_function_header, {'A'}, {1});
    write_on(write_function_header, {'S'}, {0}, {'\u0000'}, {0});
    move(write_function_header, {0,1}, 1);
    link_put(write_function_header, {'{'}, {1});
    move(write_function_header, {0,1}, 1);
    write_on(write_function_header, {'\u0000'}, {0}, {'S'}, {0});

    set_heap_mode(write_function_header, true);
    go_to(write_function_header, {'A'}, 0, -1, {0,1});

    IncompleteSet b{"b", "br"};
    //link(write_function_header, b);

    find_match_heap_traverse(write_function_header, 'A', 'S', 0, 1, false);

    string creatable = branch_on(write_function_header, {'\u0000'}, {(int) stack_tape});

    set_heap_mode(write_function_header, false);

    write_on(write_function_header, {'S'}, {0}, {'\u0000'}, {0});
    move(write_function_header, {0,1}, -1);

    make_loop(write_function_header);

    write_function_header.to_state = creatable;


    //set heapmode was only disabled after branch
    heap_mode = true;
    set_heap_mode(write_function_header, false);

    //copy function token to tokenize
    go_to(write_function_header, {'H'}, tuple_indexes[0], 1, tuple_indexes);

    //move S marker
    write_on(write_function_header, {'S'}, {0}, {'\u0000'}, {0});
    move(write_function_header, {0,1}, -1);
    write_on(write_function_header, {'{'}, {1}, {'\u0000'}, {1});

    go_to(write_function_header, {'{'}, 1, -1, {0,1});
    move(write_function_header, {0,1}, 1);
    write_on(write_function_header, {'\u0000'}, {0}, {'S'}, {0});

    go_to(write_function_header, {'\u0000'}, 1, 1, {0,1});
    write_on(write_function_header, {'\u0000'}, {0}, {'E'}, {0});
    go_to(write_function_header, {'S'}, 0, -1, {0,1});


    //make sure to add void type in front of the function name
    //we are on working currently marked by S marker

    push(write_function_header, stack_sep);
    go_to_move(write_function_header, {'\u0000'}, 1, 1, {0,1}, stack_tape, 1, {(int) stack_tape});
    write_on(write_function_header, {'E'}, {0}, {'\u0000'}, {0});
    go_to(write_function_header, {'S'}, 0, -1, {0,1});

    //check for templates on the path
    //also copy template if found
    go_to(write_function_header, {'A'}, 0, -1, {0,1});
    set_heap_mode(write_function_header, true);

    find_match_heap(write_function_header, 'A', 'S', 0, 1, true);
    IncompleteSet found_template{"found_template_action_"+ to_string(counter), "found_template_action_"+ to_string(counter)};
    counter++;
    go_to(found_template, {'S'}, 0, 1, {0,1});

    go_to(found_template, {'#'}, stack_tape, 1, {(int) stack_tape});
    go_to(found_template, {heap_sep}, stack_tape, 1, {(int) stack_tape});
    move(found_template, {(int) stack_tape}, -1);
    go_to_copy(found_template, {'#'}, stack_tape, -1, {(int) stack_tape}, 1, 1, {0,1});

    link_put(found_template, {' '}, {1});
    move(found_template, {0,1}, 1);

    go_to(found_template, {heap_sep}, stack_tape, 1, {(int) stack_tape});
    go_to(found_template, {'#'}, stack_tape, 1, {(int) stack_tape});
    move(found_template, {(int) stack_tape}, -1);
    go_to_copy(found_template, {heap_sep}, stack_tape, -1, {(int) stack_tape}, 1, 1, {0,1});
    //add endsymbols after template
    string template_end = "> ";
    for (char v: template_end){
        link_put(found_template, {v}, {1});
        move(found_template, {0,1}, 1);
    }

    link_put(found_template, {'T'}, {0});

    go_to(found_template, {'A'}, 0, -1, {0,1});

    link_on_not(write_function_header, found_template, {'}'}, {(int) stack_tape});

    set_heap_mode(write_function_header, false);
    go_to(write_function_header, {'\u0000'}, 1, 1, {0,1});

    //add 'void ' on working tape
    string void_word = "\nvoid ";
    for (char v: void_word){
        link_put(write_function_header, {v}, {1});
        move(write_function_header, {0,1}, 1);
    }

    //add class specifier if needed
    //add start working tape is at end of function
    //and stack is at emd
    go_to(write_function_header, {stack_symbol}, stack_tape, -1, {(int) stack_tape});
    move(write_function_header, {(int) stack_tape}, 1);

    IncompleteSet set_specifier{"set_specifier_"+ to_string(counter), "set_specifier_"+ to_string(counter)};
    counter++;
    go_to(set_specifier, {'\u0000'}, stack_tape, 1, {(int) stack_tape});

    set_heap_mode(set_specifier, true);

    IncompleteSet find_class_loop{"find_class_loop_"+ to_string(counter), "find_class_loop_"+ to_string(counter)};
    counter++;
    //search for class
    //requires that Class exists in hierarchy
    //move T to next position if already exists

    //recalibrate stack
    set_heap_mode(find_class_loop, false);
    set_heap_mode(find_class_loop, true);

    go_to(find_class_loop, {'A', 'Q'}, 0, -1, {0,1});
    write_on(find_class_loop, {'Q'}, {0}, {'\u0000'}, {0});

    go_to_multiple(find_class_loop, {{'S'},{'{'}}, {0, 1}, 1, {0,1});
    move(find_class_loop, {0,1}, 1);
    string no_class = branch_on(find_class_loop, {'S'}, {0});
    string no_class2 = branch_on(find_class_loop, {'A'}, {0});
    write_on(find_class_loop, {'\u0000'}, {0}, {'Q'}, {0});
    go_to(find_class_loop, {'A'}, 0, -1, {0,1});
    find_match_heap(find_class_loop, 'A', 'Q', 0, 1);

    //make_loop_on(find_class_loop, '\u0000', stack_tape);

    go_to(find_class_loop, {heap_sep}, stack_tape, 1, {(int) stack_tape});

    //copies class on working
    IncompleteSet copy_to_working_stack{"copy_to_working_"+ to_string(counter), "copy_to_working_"+ to_string(counter)};
    counter++;
    go_to(copy_to_working_stack, {'#'}, stack_tape, 1, {(int) stack_tape});
    move(copy_to_working_stack, {(int) stack_tape}, -1);
    go_to(copy_to_working_stack, {'\u0000'}, 1, 1, {0,1});

    go_to_copy(copy_to_working_stack, {heap_sep}, stack_tape, -1, {(int) stack_tape}, 1, 1 , {0,1});

    link_put(copy_to_working_stack, {':'}, {1});
    move(copy_to_working_stack, {0,1}, 1);
    link_put(copy_to_working_stack, {':'}, {1});
    move(copy_to_working_stack, {0,1}, 1);

    string end_copy = copy_to_working_stack.to_state;
    copy_to_working_stack.to_state = "never_reached_" + to_string(counter-1);

    link_on_sequence(find_class_loop, copy_to_working_stack, {'s', 's', 'a', 'l', 'c'}, stack_tape);

    make_loop(find_class_loop);
    find_class_loop.to_state = end_copy;

    link(set_specifier, find_class_loop);

    //link no class found to here
    IncompleteTransition no_class_found;
    no_class_found.state = no_class;
    no_class_found.to_state = set_specifier.to_state;
    no_class_found.def_move = 0;
    set_specifier.transitions.push_back(no_class_found);

    //cleanup
    set_heap_mode(set_specifier, false);
    go_to(set_specifier, {'S'}, 0, -1, {0,1});
    move(set_specifier, {0,1}, -1);
    go_to_clear(set_specifier, {'A'}, 0, -1, {0,1}, {0});
    go_to(set_specifier, {'\u0000'}, 1, 1, {0,1});

    link(write_function_header, set_specifier);


    //link_on(write_function_header, set_specifier, {'O'}, {(int) stack_tape});

    move(write_function_header, {(int) stack_tape}, -1);
    go_to(write_function_header, {'\u0000'}, stack_tape, 1, {(int) stack_tape});

    //put stack back on working
    go_to(write_function_header, {stack_sep}, stack_tape, -1, {(int) stack_tape});
    link_put(write_function_header, {'\u0000'}, {(int) stack_tape});
    move(write_function_header, {(int) stack_tape}, 1);
    go_to_move(write_function_header, {'\u0000'}, stack_tape, 1, {(int) stack_tape}, 1, 1, {0,1});
    go_to_not(write_function_header, {'\u0000'}, stack_tape, -1, {(int) stack_tape});
    move(write_function_header, {(int) stack_tape}, 1);

    write_on(write_function_header, {'\u0000'}, {0}, {'E'}, {0});
    go_to(write_function_header, {'S'}, 0, -1, {0,1});

    //make extra token if template
    //check first if template present
    go_to(write_function_header, {'T', 'E'}, 0, 1, {0,1});

    IncompleteSet seperate_template_token{"seperate_template_token_"+ to_string(counter), "seperate_template_token_"+ to_string(counter)};
    counter++;
    //need to store last '>' as seperator
    move(seperate_template_token, {0,1}, -2);

    link_put(seperate_template_token, {'E'}, {0});
    go_to(seperate_template_token, {'S'}, 0, -1, {0,1});
    make_token(seperate_template_token, tuple_indexes, 'T');
    go_to(seperate_template_token, {'E'}, 0, 1, {0,1});

    //stores last '>' as seperator
    write_on(seperate_template_token, {'E'}, {0}, {'T'}, {0});
    move(seperate_template_token, {0,1}, 1);
    link_put(seperate_template_token, {'E'}, {0});
    move(seperate_template_token, {0,1}, -1);
    make_token(seperate_template_token, tuple_indexes, 'S');
    go_to(seperate_template_token, {'E'}, 0, 1, {0,1});
    write_on(seperate_template_token, {'E'}, {0}, {'T'}, {0});

    link_on(write_function_header, seperate_template_token, {'T'}, {0});

    //go to S if no template
    //else go to T
    go_to(write_function_header, {'S', 'T'}, 0, -1, {0,1});

    //make function caller token
    make_token(write_function_header, tuple_indexes, 'U', 'C');

    //clear first working tape
    go_to_clear(write_function_header, {'S'}, 0, -1, {0,1}, {0,1});
    link_put(write_function_header, {'\u0000'}, {1});
    push(write_function_header, stack_sep);


    //store all parameters on stack
    go_to(write_function_header, {'N'}, tuple_indexes[0], -1, tuple_indexes);


    IncompleteSet check_var_loop{"check_define_loop_"+ to_string(counter), "check_define_loop_"+ to_string(counter)};
    counter++;

    move(check_var_loop, tuple_indexes, 1);
    string branch = branch_on(check_var_loop, {'U'}, {tuple_indexes[0]});

    IncompleteSet do_check{"do_check_"+ to_string(counter), "do_check_"+ to_string(counter)};
    counter++;
    check_var_define_location(do_check, tuple_indexes);

    link_on(check_var_loop, do_check, {'A'}, {tuple_indexes[1]});

    make_loop(check_var_loop);
    check_var_loop.to_state = branch;

    link(write_function_header, check_var_loop);


    //add '(' for function definition
    link_put(write_function_header, {'('}, {1});
    move(write_function_header, {0,1}, 1);
    link_put(write_function_header, {'E'}, {0});
    go_to(write_function_header, {'S'}, 0, -1, {0,1});

    go_to(write_function_header, {'H'}, tuple_indexes[0], 1, tuple_indexes);
    make_token(write_function_header, tuple_indexes, '(');

    write_on(write_function_header, {'E'}, {0}, {'\u0000'}, {0});
    go_to(write_function_header, {'S'}, 0, -1, {0,1});
    link_put(write_function_header, {'\u0000'}, {1});

    //putting stack data on working tape
    //start dot
    //copy to working loop
    IncompleteSet copy_to_working{"copy_to_working_"+ to_string(counter), "copy_to_working_"+ to_string(counter)};
    counter++;

    go_to(copy_to_working, {stack_sep}, stack_tape, -1, {(int) stack_tape});
    move(copy_to_working, {(int) stack_tape}, -1);

    go_to(copy_to_working, {stack_sep, stack_symbol}, stack_tape, -1, {(int) stack_tape});
    string end_branch = branch_on(copy_to_working, {stack_symbol}, {(int) stack_tape});
    IncompleteSet copy_data{"copy_data_"+ to_string(counter), "copy_data_"+ to_string(counter)};
    counter++;

    go_to(copy_data, {heap_sep}, stack_tape, 1, {(int) stack_tape});

    move(copy_data, {(int) stack_tape}, 1);
    go_to_copy(copy_data, {stack_sep}, stack_tape, 1, {(int) stack_tape}, 1, 1, {0,1});
    move(copy_data, {(int) stack_tape}, -1);
    go_to(copy_data, {stack_sep}, stack_tape, -1, {(int) stack_tape});
    move(copy_data, {(int) stack_tape}, 1);

    //add by refrence and space on working tape
    //if not already reference or ptr present
    move(copy_data, {0,1}, -1);
    IncompleteSet check1{"check_1_"+ to_string(counter), "check_1_"+ to_string(counter)};
    counter++;
    IncompleteSet check2{"check_2_"+ to_string(counter), "check_2_"+ to_string(counter)};
    counter++;
    move(check2, {0,1}, 1);
    link_put(check2, {'&'}, {1});

    link_on_not(check1, check2, {'*'}, {1});
    link_on_not(copy_data, check1, {'&'}, {1});

    move(copy_data, {0,1}, 1);
    //part of the check above

    link_put(copy_data, {' '}, {1});
    move(copy_data, {0,1}, 1);

    go_to_copy(copy_data, {heap_sep}, stack_tape, 1, {(int) stack_tape}, 1, 1, {0,1});

    link_put(copy_data, {','}, {1});
    move(copy_data, {0,1}, 1);

    link_put(copy_data, {' '}, {1});
    move(copy_data, {0,1}, 1);

    //check if we don't copy a space
    IncompleteSet copy_check{"copy_check_"+ to_string(counter), "copy_check_"+ to_string(counter)};
    counter++;
    move(copy_check, {(int) stack_tape}, 1);
    link_on_not(copy_check, copy_data, {' '}, {(int) stack_tape});

    link_on(copy_to_working, copy_check, {stack_sep}, {(int) stack_tape});

    make_loop(copy_to_working);
    copy_to_working.to_state = end_branch;

    //remove last ',' and ' '
    IncompleteSet remove_last{"remove_last_"+ to_string(counter), "remove_last_"+ to_string(counter)};
    counter++;
    move(remove_last, {0,1}, -1);
    link_put(remove_last, {'\u0000'}, {1});
    move(remove_last, {0,1}, -1);
    link_put(remove_last, {'\u0000'}, {1});

    link_put(remove_last, {'E'}, {0});
    link_on_not(copy_to_working, remove_last, {'S'}, {0});

    IncompleteSet no_last{"no_last_"+ to_string(counter), "no_last_"+ to_string(counter)};
    counter++;
    link_put(no_last, {' '}, {1});
    move(no_last, {0,1}, 1);
    link_put(no_last, {'E'}, {0});

    link_on(copy_to_working, no_last, {'S'}, {0});

    link(write_function_header, copy_to_working);

    //clears all stack overhead
    go_to(write_function_header, {'\u0000'}, stack_tape, 1, {(int) stack_tape});
    go_to(write_function_header, {stack_symbol}, stack_tape, -1, {(int) stack_tape});
    go_to(write_function_header, {stack_sep, '\u0000'}, stack_tape, 1, {(int) stack_tape});
    go_to_clear(write_function_header, {'\u0000'}, stack_tape, 1, {(int) stack_tape}, {(int) stack_tape});
    go_to_not(write_function_header, {'\u0000'}, stack_tape, -1, {(int) stack_tape});
    //go_to_clear(write_function_header, {'{'}, stack_tape, -1, {(int) stack_tape}, {(int) stack_tape});
    move(write_function_header, {(int) stack_tape}, 1);

    go_to(write_function_header, {'S'}, 0, -1, {0,1});



    //we dont store spaces as token for arguments
    IncompleteSet if_definer{"if_definer_"+ to_string(counter), "if_definer_"+ to_string(counter)};
    counter++;
    make_token_splitter(if_definer, tuple_indexes, 'D', ',');
    link_on_not(write_function_header, if_definer, {' '}, {1});

    go_to(write_function_header, {'S'}, 0, -1, {0,1});
    //store every value that is a parameter on stack for later
    IncompleteSet stack_loop{"stack_loop_"+ to_string(counter), "stack_loop_"+ to_string(counter)};
    counter++;

    push(stack_loop, stack_sep);


    //go to space seperating type with var
    go_to_multiple(stack_loop, {{'E'},{','}}, {0, 1}, 1, {0,1});
    go_to(stack_loop, {' '}, 1, -1, {0,1});

    move(stack_loop, {0,1}, 1);
    go_to_copy(stack_loop, {',', '\u0000'}, 1, 1, {0,1}, stack_tape, 1, {(int) stack_tape});

    string stack_loop_branch = branch_on(stack_loop, {'\u0000'}, {1});
    move(stack_loop, {0,1}, 2);
    make_loop(stack_loop);

    stack_loop.to_state = stack_loop_branch;

    link(write_function_header, stack_loop);

    //clears working
    go_to_clear(write_function_header, {'S'}, 0, -1, {0,1}, {0,1});
    link_put(write_function_header, {'\u0000'}, {1});

    //store last ')'
    go_to(write_function_header, {'H'}, tuple_indexes[0], 1, tuple_indexes);
    link_put(write_function_header, {'\u0000', ')', ')'}, {tuple_indexes[0], tuple_indexes[1], tuple_indexes[2]});
    move(write_function_header, tuple_indexes, 1);
    link_put(write_function_header, {'H'}, {tuple_indexes[0]});

    go_to(write_function_header, {'N'}, tuple_indexes[0], -1, tuple_indexes);

    link(a, write_function_header);

}

void TuringTools::make_token(IncompleteSet &a, const vector<int> &tuple_indexes, char def_token, char replace) {
    //requires 'S' and 'E' marker on working tape

    link_put(a, {def_token}, {tuple_indexes[1]});

    IncompleteSet copy_to_tuple{"copy_to_tuple_"+ to_string(counter), "copy_to_tuple_"+ to_string(counter)};
    counter++;

    vector<string> branch_states;

    for (int i=2; i<tuple_indexes.size(); i++){
        copy(copy_to_tuple, 1, tuple_indexes[i]);
        move(copy_to_tuple, {0,1}, 1);

        string branch = branch_on(copy_to_tuple, {'E'}, {0});
        branch_states.push_back(branch);
        if (i == tuple_indexes.size()-1){
            move(copy_to_tuple, tuple_indexes, 1);
            link_put(copy_to_tuple, {'E'}, {tuple_indexes[1]});
            make_loop(copy_to_tuple);
        }
    }

    copy_to_tuple.to_state = branch_states[0];
    for (int i=1; i<branch_states.size(); i++){
        string b = branch_states[i];

        IncompleteTransition to_end;
        to_end.state = b;
        to_end.to_state = copy_to_tuple.to_state;
        to_end.def_move = 0;

        copy_to_tuple.transitions.push_back(to_end);
    }

    link(a, copy_to_tuple);
    move(a, tuple_indexes, 1);

    link_put(a, {'H'}, {tuple_indexes[0]});
    move(a, tuple_indexes, -1);

    go_to(a, {'H'}, tuple_indexes[0], -1, tuple_indexes);
    link_put(a, {replace}, {tuple_indexes[0]});
    go_to(a, {'H'}, tuple_indexes[0], 1, tuple_indexes);

}

void TuringTools::make_token_splitter(IncompleteSet &a, const vector<int> &tuple_indexes, char def_token, char splitter) {

    IncompleteSet make_token_splitter{"make_token_splitter_"+ to_string(counter), "make_token_splitter_"+ to_string(counter)};
    counter++;

    IncompleteSet move_on{"move_on_"+ to_string(counter), "move_on_"+ to_string(counter)};
    counter++;
    move(move_on, {0,1}, 1);
    link_on(make_token_splitter, move_on, {splitter}, {1});

    link_put(make_token_splitter, {def_token}, {tuple_indexes[1]});

    IncompleteSet copy_to_tuple{"copy_to_tuple_"+ to_string(counter), "copy_to_tuple_"+ to_string(counter)};
    counter++;

    vector<string> branch_states;

    for (int i=2; i<tuple_indexes.size(); i++){
        copy(copy_to_tuple, 1, tuple_indexes[i]);
        move(copy_to_tuple, {0,1}, 1);

        string branch = branch_on(copy_to_tuple, {'E'}, {0});
        move(copy_to_tuple, {0,1}, -1);
        string branch2 = branch_on(copy_to_tuple, {splitter}, {1});
        move(copy_to_tuple, {0,1}, 1);
        branch_states.push_back(branch);
        branch_states.push_back(branch2);
        if (i == tuple_indexes.size()-1){
            move(copy_to_tuple, tuple_indexes, 1);
            link_put(copy_to_tuple, {'E'}, {tuple_indexes[1]});
            make_loop(copy_to_tuple);
        }
    }

    copy_to_tuple.to_state = branch_states[0];
    for (int i=1; i<branch_states.size(); i++){
        string b = branch_states[i];

        IncompleteTransition to_end;
        to_end.state = b;
        to_end.to_state = copy_to_tuple.to_state;
        to_end.def_move = 0;

        copy_to_tuple.transitions.push_back(to_end);
    }

    link(make_token_splitter, copy_to_tuple);
    move(make_token_splitter, tuple_indexes, 1);
    link_put(make_token_splitter, {'H'}, {tuple_indexes[0]});
    move(make_token_splitter, tuple_indexes, -1);

    go_to(make_token_splitter, {'H'}, tuple_indexes[0], -1, tuple_indexes);
    link_put(make_token_splitter, {'\u0000'}, {tuple_indexes[0]});

    go_to(make_token_splitter, {'H'}, tuple_indexes[0], 1, tuple_indexes);

    make_loop_on(make_token_splitter, ',', 1);

    link(a, make_token_splitter);

}

void TuringTools::check_var_define_location(IncompleteSet &a, const vector<int> &tuple_indexes) {
    //check every A token en check if the data of the token is defined
    //required stack mode
    //startpos is 'N' and end is marked with 'U'
    IncompleteSet find_var{"find_var_"+ to_string(counter), "find_var_"+ to_string(counter)};
    counter++;
    go_to(find_var, {'A'}, tuple_indexes[1], 1, tuple_indexes);

    //set markers for copy to working
    link_put(find_var, {'S'}, {tuple_indexes[0]});
    move(find_var, tuple_indexes, 1);
    go_to_not(find_var, {'E'}, tuple_indexes[1], 1, tuple_indexes);
    link_put(find_var, {'E'}, {tuple_indexes[0]});

    go_to(find_var, {'S'}, tuple_indexes[0], -1, tuple_indexes);

    copy_to_working(find_var, tuple_indexes);

    //here check if present on stack
    //we dont want double parameter for same apramater

    IncompleteSet check_var_loop{"check_var_loop_"+ to_string(counter), "check_var_loop_"+ to_string(counter)};
    counter++;

    //split the substring for every non variable char
    go_to(check_var_loop, {'S'}, 0, -1, {0,1});

    //marks var first splitter by 'P'
    //stops at 'P'
    check_var_char_working(check_var_loop);

    //in case under 'S' marker it starts with a non-variable symbol
    IncompleteSet remove_useless{"remove_useless_"+ to_string(counter), "remove_useless_"+ to_string(counter)};
    link_put(remove_useless, {'\u0000'}, {1});
    move(remove_useless, {0,1}, 1);
    //copy rest on stack
    go_to_move(remove_useless, {'\u0000'}, 1, 1, {0,1}, stack_tape, 1, {(int) stack_tape});
    write_on(remove_useless, {'E'}, {0}, {'\u0000'}, {0});
    go_to(remove_useless, {'S'}, 0, -1, {0,1});
    go_to(remove_useless, {stack_sep}, stack_tape, -1, {(int) stack_tape});
    move(remove_useless, {(int) stack_tape}, 1);
    go_to_move(remove_useless, {'\u0000'}, (int) stack_tape, 1, {(int) stack_tape}, 1, 1, {0,1});
    go_to(remove_useless, {stack_sep}, stack_tape, -1, {(int) stack_tape});
    move(remove_useless, {(int) stack_tape}, 1);

    write_on(remove_useless, {'\u0000'}, {0}, {'E'}, {0});
    go_to(remove_useless, {'S'}, 0, -1, {0,1});

    check_var_char_working(remove_useless);

    string remove_useless_quit = branch_on(remove_useless, {'S', '\u0000'}, {0, 1});
    make_loop_on(remove_useless, 'S', 0);

    link_on(check_var_loop, remove_useless, {'S'}, {0});

    go_to(check_var_loop, {'S'}, 0, -1, {0,1});

    //move part from 'P' temporarly on stack
    push(check_var_loop, stack_sep);
    go_to(check_var_loop, {'P', 'E'}, 0, 1, {0,1});
    go_to_move(check_var_loop, {'\u0000'}, 1, 1, {0, 1}, stack_tape, 1, {(int) stack_tape});

    //clear markers till 'S'
    go_to_clear(check_var_loop, {'S', 'A'}, 0, -1, {0,1}, {0});

    //set var in nesting
    write_on(check_var_loop, {'S'}, {0}, {'\u0000'}, {0});
    go_to(check_var_loop, {'\u0000'}, 1, 1, {0,1});
    link_put(check_var_loop, {'{'}, {1});
    move(check_var_loop, {0,1}, 1);
    link_put(check_var_loop, {'S'}, {0});

    //TODO: only do if normal find does not find it
    //setup find match traverse

    string skip_store = check_stack_double(check_var_loop);

    IncompleteSet on_skip_store{skip_store, skip_store};
    go_to(on_skip_store, {'A'}, 0, -1, {0,1});
    go_to(on_skip_store, {'S'}, 0, 1, {0,1});
    set_heap_mode(on_skip_store, true);
    go_to(on_skip_store, {'\u0000'}, stack_tape, -1, {(int) stack_tape});
    check_var_loop.transitions.insert(check_var_loop.transitions.end(), on_skip_store.transitions.begin(), on_skip_store.transitions.end());


    go_to(check_var_loop, {'A'}, 0, -1, {0,1});
    heap_mode = false;
    set_heap_mode(check_var_loop, true);

    find_match_heap_traverse(check_var_loop, 'A', 'S', 0, 1);



    IncompleteTransition bypassTraverse;
    bypassTraverse.state = on_skip_store.to_state;
    bypassTraverse.to_state = check_var_loop.to_state;
    bypassTraverse.def_move = 0;

    check_var_loop.transitions.push_back(bypassTraverse);

    //split nesting check
    IncompleteSet check_split_nesting_call{"check_split_nesting_call"+to_string(counter), "check_split_nesting_call"+to_string(counter)};
    counter++;

    check_split_nesting(check_split_nesting_call);

    link_on_not(check_var_loop, check_split_nesting_call, {'\u0000'}, {(int) stack_tape});

    //string not_found = branch_on(check_var_loop, {'\u0000'}, {(int) stack_tape});

    IncompleteSet on_found{"on_found_"+ to_string(counter), "on_found_"+ to_string(counter)};
    //store heap definer on working
    go_to(on_found, {'#'}, stack_tape, 1, {(int) stack_tape});
    move(on_found, {(int) stack_tape}, 1);
    go_to(on_found, {'#'}, stack_tape, 1, {(int) stack_tape});
    move(on_found, {(int) stack_tape}, -1);
    go_to_copy(on_found, {'#'}, stack_tape, -1, {(int) stack_tape}, 1, 1, {0,1});
    move(on_found, {0,1}, -1);

    link_on_not(check_var_loop, on_found, {'\u0000'}, {(int) stack_tape});

    //place holder for a not found character
    write_on(check_var_loop, {'\u0000'}, {1}, {' '}, {1});

    move(check_var_loop, {0,1}, 1);
    set_heap_mode(check_var_loop, false);

    //pop other var on stack that are seperated by a operator
    //push heap definer
    link_put(check_var_loop, {'P'}, {0});

    //put data back on working
    go_to(check_var_loop, {stack_sep}, stack_tape, -1, {(int) stack_tape});
    move(check_var_loop, {(int) stack_tape}, 1);
    go_to_move(check_var_loop, {'\u0000'}, stack_tape, 1, {(int) stack_tape}, 1, 1, {0,1});
    go_to(check_var_loop, {stack_sep}, stack_tape, -1, {(int) stack_tape});
    link_put(check_var_loop, {'\u0000'}, {(int) stack_tape});

    //overrides P if their are no values on stack
    link_put(check_var_loop, {'E'}, {0});

    //store heap data on stack
    //go to S in case P doesnt exist
    go_to(check_var_loop, {'P', 'S'}, 0, -1, {0,1});

    write_on(check_var_loop, {'P'}, {0}, {'\u0000'}, {1});

    go_to(check_var_loop, {'S'}, 0, -1, {0,1});
    go_to_move(check_var_loop, {'\u0000'}, 1, 1, {0, 1},  stack_tape, 1, {(int) stack_tape});
    push(check_var_loop, stack_sep);

    //push remaining stuff on stack
    write_on(check_var_loop, {'P'}, {0}, {'\u0000'}, {0});

    //prevent move if marker is 'E'
    IncompleteSet move_on{"move_on_"+ to_string(counter), "move_on_"+ to_string(counter)};
    move(move_on, {0,1}, 1);
    link_on(check_var_loop, move_on, {'\u0000'}, {0});

    go_to_move(check_var_loop, {'\u0000'}, 1, 1, {0,1}, stack_tape, 1, {(int) stack_tape});
    write_on(check_var_loop, {'E'}, {0}, {'\u0000'}, {0});


    //remove 1 nesting
    go_to(check_var_loop, {'S'}, 0, -1, {0,1});
    link_put(check_var_loop, {'\u0000'}, {0});
    go_to(check_var_loop, {'{'}, 1, -1, {0,1});
    link_put(check_var_loop, {'\u0000'}, {1});
    move(check_var_loop, {0,1}, -1);
    go_to_clear(check_var_loop, {'{'}, 1, -1, {0,1}, {1});
    move(check_var_loop, {0,1}, 1);
    link_put(check_var_loop, {'S'}, {0});

    //copy remaining stuff back on right spot
    go_to(check_var_loop, {stack_sep}, stack_tape, -1, {(int) stack_tape});
    move(check_var_loop, {(int) stack_tape}, 1);
    go_to_move(check_var_loop, {'\u0000'}, stack_tape, 1, {(int) stack_tape}, 1, 1, {0,1});
    go_to(check_var_loop, {stack_sep}, stack_tape, -1, {(int) stack_tape});
    move(check_var_loop, {(int) stack_tape}, 1);
    write_on(check_var_loop, {'\u0000'}, {0}, {'E'}, {0});
    //this line is double but that is not a problem and is extra redundancy
    go_to(check_var_loop, {'S'}, 0, -1, {0,1});
    string branch_done = branch_on(check_var_loop, {'\u0000'}, {1});

    make_loop(check_var_loop);
    check_var_loop.to_state = branch_done;

    IncompleteTransition from_remove_useless;
    from_remove_useless.state = remove_useless_quit;
    from_remove_useless.to_state = branch_done;
    from_remove_useless.def_move = 0;

    check_var_loop.transitions.push_back(from_remove_useless);

    link(find_var, check_var_loop);

    go_to(find_var, {'E'}, tuple_indexes[0], 1, tuple_indexes);
    link_put(find_var, {'\u0000'}, {tuple_indexes[0]});
    go_to(find_var, {'S'}, tuple_indexes[0], -1, tuple_indexes);
    link_put(find_var, {'\u0000'}, {tuple_indexes[0]});

    link(a, find_var);
}

void TuringTools::check_var_char_working(IncompleteSet &a) {
    IncompleteSet var_char_check{"var_char_check_"+ to_string(counter), "var_char_check_"+ to_string(counter)};
    counter++;

    vector<char> var_char;
    for (int i=48; i<= 57; i++){
        var_char.push_back((char) i);
    }

    for (int i=65; i<= 90; i++){
        var_char.push_back((char) i);
    }

    for (int i=97; i<= 122; i++){
        var_char.push_back((char) i);
    }

    var_char.push_back('_');

    var_char.push_back('"');
    var_char.push_back('\'');

    go_to_not(var_char_check, var_char, 1, 1, {0,1});

    //string loop_branch = branch_on(var_char_check, {'E'}, {0});

    //makes sure to to override wrong tokens
    write_on(var_char_check, {'\u0000'}, {0}, {'P'}, {0});


    //make_loop(var_char_check);
    //var_char_check.to_state = loop_branch;

    //go back to start


    link(a, var_char_check);

}

void TuringTools::clear_heap(IncompleteSet &a) {
    set_heap_mode(a, true);
    move(a, {(int) stack_tape}, -1);
    go_to_clear(a, {'\u0000'}, stack_tape, -1, {(int) stack_tape}, {(int) stack_tape});


    go_to(a, {stack_symbol}, stack_tape, 1, {(int) stack_tape});
    go_to(a, {'\u0000'}, stack_tape, 1, {(int) stack_tape});
    heap_mode = false;

}

void
TuringTools::compare_single_tape(IncompleteSet &a, char s1, char e1, char s2, char e2, int marker_tape, int data_tape) {
    //if equal end position: 'e1'
    //if not found end position: 'e2'
    //startpositon must be s1
    IncompleteSet compare{"compare_"+ to_string(counter), "compare_"+ to_string(counter)};
    counter++;

    go_to_not(compare, {'\u0000'}, data_tape, 1, {marker_tape, data_tape});

    //TODO: after branch still check end is also fully cleared
    string branch_end_first = branch_on(compare, {e1}, {marker_tape});

    //when 1 char is not successfully compared
    IncompleteSet compare_failed{"compare_failed"+ to_string(counter), "compare_failed"+ to_string(counter)};
    counter++;

    go_to(compare_failed, {e2}, marker_tape, 1, {marker_tape, data_tape});


    //when 1 char is successfully compared
    IncompleteSet compare_fine{"compare_fine"+ to_string(counter), "compare_fine"+ to_string(counter)};
    counter++;

    //construction using a couple new states
    //for each character their is an own state
    //state_check_a ->(when reached 's2' marker) state_check_a_2
    //state_check_a_2 on same symbol replace symbol link to compare
    //state_check_a_2 on different symbol, transition to compared_failed state

    for (int j =31; j<127; j++){
        char c = (char) j;
        if (j == 31){
            c = '\n';
        }

        IncompleteTransition to_state_check_a;
        to_state_check_a.state = compare.to_state;
        to_state_check_a.to_state = "state_check_"+to_string(c)+"_"+ to_string(counter);
        to_state_check_a.def_move = 0;
        to_state_check_a.input = {c};
        to_state_check_a.input_index = {data_tape};
        to_state_check_a.output = {'\u0001', '\u0000'};
        to_state_check_a.output_index = {marker_tape, data_tape};
        to_state_check_a.move = {0, 0};

        IncompleteTransition to_state_check_2a_no_marker;
        to_state_check_2a_no_marker.state = "state_check_"+to_string(c)+"_"+ to_string(counter);
        to_state_check_2a_no_marker.to_state = "state_check_"+to_string(c)+"_"+ to_string(counter);
        to_state_check_2a_no_marker.def_move = 0;
        to_state_check_2a_no_marker.output = {'\u0001', '\u0001'};
        to_state_check_2a_no_marker.output_index = {marker_tape, data_tape};
        to_state_check_2a_no_marker.move = {1, 1};

        IncompleteTransition to_state_check_2a_marker;
        to_state_check_2a_marker.state = "state_check_"+to_string(c)+"_"+ to_string(counter);
        to_state_check_2a_marker.to_state = "state_check_2"+to_string(c)+"_"+ to_string(counter);
        to_state_check_2a_marker.def_move = 0;
        to_state_check_2a_marker.input = {s2};
        to_state_check_2a_marker.input_index = {marker_tape};

        //loops while blank

        IncompleteTransition to_state_check_3a_no_marker;
        to_state_check_3a_no_marker.state = "state_check_2"+to_string(c)+"_"+ to_string(counter);
        to_state_check_3a_no_marker.to_state = "state_check_2"+to_string(c)+"_"+ to_string(counter);
        to_state_check_3a_no_marker.def_move = 0;
        to_state_check_3a_no_marker.input = {'\u0000'};
        to_state_check_3a_no_marker.input_index = {data_tape};
        to_state_check_3a_no_marker.output = {'\u0001', '\u0001'};
        to_state_check_3a_no_marker.output_index = {marker_tape, data_tape};
        to_state_check_3a_no_marker.move = {1, 1};

        IncompleteTransition to_state_check_3a_not_equal;
        to_state_check_3a_not_equal.state = "state_check_2"+to_string(c)+"_"+ to_string(counter);
        to_state_check_3a_not_equal.to_state = compare_failed.state;
        to_state_check_3a_not_equal.def_move = 0;

        //incase first string is longer than second
        IncompleteTransition to_state_check_3a_not_equal2;
        to_state_check_3a_not_equal2.state = "state_check_2"+to_string(c)+"_"+ to_string(counter);
        to_state_check_3a_not_equal2.to_state = compare_failed.state;
        to_state_check_3a_not_equal2.def_move = 0;
        to_state_check_3a_not_equal2.input = {e2};
        to_state_check_3a_not_equal2.input_index = {marker_tape};

        IncompleteTransition to_state_check_3a_equal;
        to_state_check_3a_equal.state = "state_check_2"+to_string(c)+"_"+ to_string(counter);
        to_state_check_3a_equal.to_state = compare_fine.state;
        to_state_check_3a_equal.def_move = 0;
        to_state_check_3a_equal.input = {c};
        to_state_check_3a_equal.input_index = {data_tape};
        to_state_check_3a_equal.output = {'\u0001', '\u0000'};
        to_state_check_3a_equal.output_index = {marker_tape, data_tape};
        to_state_check_3a_equal.move = {0, 0};


        compare.transitions.push_back(to_state_check_a);
        compare.transitions.push_back(to_state_check_2a_no_marker);
        compare.transitions.push_back(to_state_check_2a_marker);
        compare.transitions.push_back(to_state_check_3a_no_marker);
        compare.transitions.push_back(to_state_check_3a_not_equal);
        compare.transitions.push_back(to_state_check_3a_not_equal2);
        compare.transitions.push_back(to_state_check_3a_equal);

    }
    counter++;

    compare.to_state = compare_fine.to_state;
    go_to(compare, {s1}, marker_tape, -1, {marker_tape, data_tape});
    make_loop(compare);

    compare.to_state = branch_end_first;

    IncompleteTransition failed_to_quit;
    failed_to_quit.state = compare_failed.to_state;
    failed_to_quit.to_state = compare.to_state;
    failed_to_quit.def_move = 0;

    compare.transitions.push_back(failed_to_quit);
    compare.transitions.insert(compare.transitions.end(), compare_failed.transitions.begin(), compare_failed.transitions.end());
    compare.transitions.insert(compare.transitions.end(), compare_fine.transitions.begin(), compare_fine.transitions.end());

    link(a, compare);

}

void TuringTools::makeAntiNesting(IncompleteSet &a, const vector<int>&tuple_indexes) {
    //requirements
    //current unique marker 'V'
    //on left 'S' marker
    //on right 'E' marker

    //first check if are new top code will be from the if statement or the else statement
    go_to(a, {'0', '1'}, tuple_indexes[1], -1, tuple_indexes);

    IncompleteSet do_else{"do_else_"+ to_string(counter), "do_else_"+ to_string(counter)};
    counter++;
    go_to(do_else, {'V'}, tuple_indexes[0], 1, tuple_indexes);
    makeAntiNestingElse(do_else, tuple_indexes);

    link_on(a, do_else, {'1'}, {tuple_indexes[1]});

    IncompleteSet do_if{"do_if_"+ to_string(counter), "do_if_"+ to_string(counter)};
    counter++;
    makeAntiNestingIf(do_if, tuple_indexes);
    link_on(a, do_if, {'0'}, {tuple_indexes[1]});

}

void TuringTools::makeAntiNestingElse(IncompleteSet &a, const vector<int> &tuple_indexes) {
    IncompleteSet else_anti_nesting{"else_anti_nesting_"+ to_string(counter), "else_anti_nesting_"+ to_string(counter)};
    counter++;
    //put some default stuff on working tape
    string s = "\nif";
    for (char c: s){
        link_put(else_anti_nesting, {c}, {1});
        move(else_anti_nesting, {0,1}, 1);
    }
    link_put(else_anti_nesting, {'E'}, {0});

    go_to(else_anti_nesting, {'\u0000'}, tuple_indexes[1], 1, tuple_indexes);
    link_put(else_anti_nesting, {'H'}, {tuple_indexes[0]});

    go_to(else_anti_nesting, {'A'}, 0, -1, {0,1});
    make_token(else_anti_nesting, tuple_indexes, '0', 'I');
    go_to_clear(else_anti_nesting, {'A'}, 0, -1, {0,1}, {0,1});
    link_put(else_anti_nesting, {'\u0000'}, {1});

    go_to(else_anti_nesting, {'S'}, tuple_indexes[0], -1, tuple_indexes);

    string s2 = "(!";
    for (char c: s2){
        link_put(else_anti_nesting, {c}, {1});
        move(else_anti_nesting, {0,1}, 1);
    }
    link_put(else_anti_nesting, {'E'}, {0});

    go_to(else_anti_nesting, {'H'}, tuple_indexes[0], 1, tuple_indexes);

    go_to(else_anti_nesting, {'A'}, 0, -1, {0,1});
    make_token(else_anti_nesting, tuple_indexes, '(');

    go_to_clear(else_anti_nesting, {'A'}, 0, -1, {0,1}, {0,1});
    link_put(else_anti_nesting, {'\u0000'}, {1});

    go_to(else_anti_nesting, {'S'}, tuple_indexes[0], -1, tuple_indexes);

    //first we are going to take the condition
    go_to(else_anti_nesting, {'('}, tuple_indexes[1], 1, tuple_indexes);
    link_put(else_anti_nesting, {'S'}, {tuple_indexes[0]});
    skip_nesting(else_anti_nesting, stack_tape, 1, tuple_indexes[1], 1, tuple_indexes, '(', ')');
    move(else_anti_nesting, tuple_indexes, 1);
    link_put(else_anti_nesting, {'E'}, {tuple_indexes[0]});
    go_to(else_anti_nesting, {'S'}, tuple_indexes[0], -1, tuple_indexes);

    //copy_to_working(else_anti_nesting, tuple_indexes);
    copyTupleRight(else_anti_nesting, tuple_indexes, 'S', 'E');

    //clear old copy markers
    write_on(else_anti_nesting, {'S'}, {tuple_indexes[0]}, {'\u0000'}, {tuple_indexes[0]});
    go_to(else_anti_nesting, {'E'}, tuple_indexes[0], 1, tuple_indexes);
    write_on(else_anti_nesting, {'E'}, {tuple_indexes[0]}, {'\u0000'}, {tuple_indexes[0]});

    //link_put(else_anti_nesting, {'\u0000'}, {0});

    s = ")";
    for (char c: s){
        link_put(else_anti_nesting, {c}, {1});
        move(else_anti_nesting, {0,1}, 1);
    }
    link_put(else_anti_nesting, {'E'}, {0});


    //we are going to make new tokens from now on
    //first new token will be the augmented if statement
    go_to(else_anti_nesting, {'\u0000'}, tuple_indexes[1], 1, tuple_indexes);
    link_put(else_anti_nesting, {'H'}, {tuple_indexes[0]});
    go_to(else_anti_nesting, {'A'}, 0, -1, {0,1});

    make_token(else_anti_nesting, tuple_indexes, ')');


    //clear working tape and store everything in else statement on working
    //.but first make token for curtly bracket
    go_to_clear(else_anti_nesting, {'A'}, 0, -1, {0,1}, {0, 1});
    link_put(else_anti_nesting, {'\u0000'}, {1});

    link_put(else_anti_nesting, {'{'}, {1});
    move(else_anti_nesting, {0,1}, 1);
    link_put(else_anti_nesting, {'E'}, {0});
    move(else_anti_nesting, {0,1}, -1);
    make_token(else_anti_nesting, tuple_indexes, '{');

    go_to_clear(else_anti_nesting, {'A'}, 0, -1, {0,1}, {0, 1});
    link_put(else_anti_nesting, {'\u0000'}, {1});

    go_to(else_anti_nesting, {'V'}, tuple_indexes[0], -1, tuple_indexes);
    go_to(else_anti_nesting, {'}'}, tuple_indexes[1], 1, tuple_indexes);
    link_put(else_anti_nesting, {'C'}, {tuple_indexes[0]});
    go_to(else_anti_nesting, {'V'}, tuple_indexes[0], -1, tuple_indexes);
    move(else_anti_nesting, tuple_indexes, 1);
    link_put(else_anti_nesting, {'B'}, {tuple_indexes[0]});

    go_to(else_anti_nesting, {'B'}, tuple_indexes[0], -1, tuple_indexes);
    copyTupleRight(else_anti_nesting, tuple_indexes, 'B', 'C');

    //removes B and C
    link_put(else_anti_nesting, {'\u0000'}, {tuple_indexes[0]});
    go_to(else_anti_nesting, {'C'}, tuple_indexes[0], 1, tuple_indexes);
    link_put(else_anti_nesting, {'\u0000'}, {tuple_indexes[0]});

    move(else_anti_nesting, tuple_indexes, 1);

    IncompleteSet on_not_end{"on_not_end_"+ to_string(counter), "on_not_end_"+ to_string(counter)};
    counter++;
    link_put(on_not_end, {'B'}, {tuple_indexes[0]});
    copyTupleRight(on_not_end, tuple_indexes, 'B', 'E');
    link_put(on_not_end, {'\u0000'}, {tuple_indexes[0]});
    go_to(on_not_end, {'E'}, tuple_indexes[0], 1, tuple_indexes);

    link_on_not(else_anti_nesting, on_not_end, {'E'}, {tuple_indexes[0]});

    //add extra return
    go_to(else_anti_nesting, {'I'}, tuple_indexes[0], 1, tuple_indexes);

    //check here for return
    go_to(else_anti_nesting, {'V'}, tuple_indexes[0], -1, tuple_indexes);
    go_to_multiple(else_anti_nesting, {{'E'},{'R'}}, {tuple_indexes[0], tuple_indexes[1]}, 1, {tuple_indexes});


    IncompleteSet add_empty_return{"add_empty_return_"+ to_string(counter), "add_empty_return_"+ to_string(counter)};
    counter++;
    //push empty return
    addToken(add_empty_return, tuple_indexes, "return", 'R');
    addToken(add_empty_return, tuple_indexes, ";", 'S');
    go_to(add_empty_return, {'V'}, tuple_indexes[0], -1, tuple_indexes);
    link_on(else_anti_nesting, add_empty_return, {'E'}, {tuple_indexes[0]});
    go_to(else_anti_nesting, {'E'}, tuple_indexes[0], 1, tuple_indexes);


    //add '}' token
    go_to(else_anti_nesting, {'H'},  tuple_indexes[0], 1, tuple_indexes);
    link_put(else_anti_nesting, {'}'}, {1});
    move(else_anti_nesting, {0,1}, 1);
    link_put(else_anti_nesting, {'E'}, {0});
    move(else_anti_nesting, {0,1}, -1);
    make_token(else_anti_nesting, tuple_indexes, '}');

    //clear working tape
    go_to_clear(else_anti_nesting, {'A'}, 0, -1, {0,1}, {0,1});
    link_put(else_anti_nesting, {'\u0000'}, {1});

    //from here but everything inside if statement after old else statement
    go_to(else_anti_nesting, {'V'}, tuple_indexes[0], -1, tuple_indexes);
    go_to(else_anti_nesting, {'}'}, tuple_indexes[1], -1, tuple_indexes);
    link_put(else_anti_nesting, {'C'}, {tuple_indexes[0]});
    skip_nesting(else_anti_nesting, stack_tape, 1, tuple_indexes[1], -1, tuple_indexes);
    move(else_anti_nesting, tuple_indexes, 1);
    link_put(else_anti_nesting, {'B'}, {tuple_indexes[0]});
    copyTupleRight(else_anti_nesting, tuple_indexes, 'B', 'C');
    link_put(else_anti_nesting, {'\u0000'}, {tuple_indexes[0]});
    go_to(else_anti_nesting, {'C'}, tuple_indexes[0], 1, tuple_indexes);
    link_put(else_anti_nesting, {'\u0000'}, {tuple_indexes[0]});
    go_to(else_anti_nesting, {'V'}, tuple_indexes[0], 1, tuple_indexes);
    skip_nesting(else_anti_nesting, stack_tape, 1, tuple_indexes[1], 1, tuple_indexes);
    move(else_anti_nesting, tuple_indexes, 1);
    IncompleteSet on_not_end2{"on_not_end_"+ to_string(counter), "on_not_end_"+ to_string(counter)};
    counter++;
    link_put(on_not_end2, {'B'}, {tuple_indexes[0]});
    copyTupleRight(on_not_end2, tuple_indexes, 'B', 'E');
    link_put(on_not_end2, {'\u0000'}, {tuple_indexes[0]});
    go_to(on_not_end, {'E'}, tuple_indexes[0], 1, tuple_indexes);

    link_on_not(else_anti_nesting, on_not_end2, {'E'}, {tuple_indexes[0]});

    //from now on
    //clear everythin from S till E
    //but everyting from I till H between
    //copy tup from I till H on working
    //copy from E till I on working
    //clear from S till H
    //go to S
    //paste from working
    go_to(else_anti_nesting, {'I'}, tuple_indexes[0], 1, tuple_indexes);

    auto temp = tuple_indexes;
    temp.push_back(0);
    temp.push_back(1);



    for (int i=1; i<tuple_indexes.size(); i++){

        int index = tuple_indexes[i];
        copy_till(else_anti_nesting, {'H'}, tuple_indexes[0], index, 1, 1, temp);

        go_to(else_anti_nesting, {'E'}, tuple_indexes[0], -1, tuple_indexes);
        copy_till(else_anti_nesting, {'I'}, tuple_indexes[0], index, 1, 1, temp);

        link_put(else_anti_nesting, {'E'}, {0});
        go_to(else_anti_nesting, {'A'}, 0, -1, {0, 1});
        go_to(else_anti_nesting, {'H'}, tuple_indexes[0], 1, tuple_indexes);
        go_to_clear(else_anti_nesting, {'S'}, tuple_indexes[0], -1, tuple_indexes, {index});
        copy_till(else_anti_nesting, {'E'}, 0, 1, index, 1, temp);
        go_to(else_anti_nesting, {'S'}, tuple_indexes[0], -1, tuple_indexes);
        go_to(else_anti_nesting, {'I'}, tuple_indexes[0], 1, tuple_indexes);

        //clear working tape
        go_to_clear(else_anti_nesting, {'A'}, 0, -1, {0,1}, {0,1});
        link_put(else_anti_nesting, {'\u0000'}, {1});
    }
    go_to(else_anti_nesting, {'H'}, tuple_indexes[0], 1, tuple_indexes);
    go_to_clear(else_anti_nesting, {'A'}, tuple_indexes[0], -1, tuple_indexes, {tuple_indexes[0]});
    link(a, else_anti_nesting);

}

void TuringTools::copyTupleRight(IncompleteSet &a, const vector<int> &tuple_indexes, char start_token, char end_token) {
    auto temp = tuple_indexes;
    temp.push_back(0);
    temp.push_back(1);

    for (int i=1; i<tuple_indexes.size(); i++){
        int index = tuple_indexes[i];
        copy_till(a, {end_token}, tuple_indexes[0], index, 1, 1, temp);
        link_put(a, {'E'}, {0});
        go_to(a, {'H'}, tuple_indexes[0], 1, tuple_indexes);
        go_to(a, {'A'}, 0, -1, {0,1});
        copy_till(a, {'E'}, 0, 1, index, 1, temp);
        go_to(a, {start_token}, tuple_indexes[0], -1, tuple_indexes);
        go_to_clear(a, {'A'}, 0, -1, {0,1}, {0,1});
        link_put(a, {'\u0000'}, {1});

    }
    go_to(a, {'H'}, tuple_indexes[0], 1, tuple_indexes);
    link_put(a, {'\u0000'}, {tuple_indexes[0]});
    go_to(a, {'\u0000'}, tuple_indexes[1], 1, tuple_indexes);
    link_put(a, {'H'}, {tuple_indexes[0]});
    go_to(a, {start_token}, tuple_indexes[0], -1, tuple_indexes);

}

void TuringTools::makeAntiNestingIf(IncompleteSet &a, const vector<int> &tuple_indexes) {
    IncompleteSet if_anti_nesting{"if_anti_nesting_"+ to_string(counter), "if_anti_nesting_"+ to_string(counter)};
    counter++;
    skip_nesting(if_anti_nesting, stack_tape, 1, tuple_indexes[1], 1, tuple_indexes);
    link_put(if_anti_nesting, {'K'}, {tuple_indexes[0]});
    move(if_anti_nesting, {tuple_indexes}, 1);
    link_put(if_anti_nesting, {'N'}, {tuple_indexes[0]});
    go_to(if_anti_nesting, {'{'}, tuple_indexes[1], 1, tuple_indexes);
    move(if_anti_nesting, {tuple_indexes}, 1);
    link_put(if_anti_nesting, {'L'}, {tuple_indexes[0]});
    move(if_anti_nesting, {tuple_indexes}, -1);
    skip_nesting(if_anti_nesting, stack_tape, 1, tuple_indexes[1], 1, tuple_indexes);
    move(if_anti_nesting, tuple_indexes, 1);
    link_put(if_anti_nesting, {'M'}, {tuple_indexes[0]});

    go_to(if_anti_nesting, {'\u0000'}, tuple_indexes[1], 1, tuple_indexes);
    link_put(if_anti_nesting, {'H'}, {tuple_indexes[0]});
    go_to(if_anti_nesting, {'S'}, tuple_indexes[0], -1, tuple_indexes);
    go_to(if_anti_nesting, {'M'}, tuple_indexes[0], 1, tuple_indexes);
    //copyTupleRight(if_anti_nesting, tuple_indexes, 'M', 'H');

    //check if we need to add a token before
    go_to_multiple(if_anti_nesting, {{'E'},{'R'}}, {tuple_indexes[0], tuple_indexes[1]}, 1, {tuple_indexes});

    IncompleteSet add_empty_return{"add_empty_return_"+ to_string(counter), "add_empty_return_"+ to_string(counter)};
    counter++;
    //push empty return
    addToken(add_empty_return, tuple_indexes, "return", 'R', 'O');
    addToken(add_empty_return, tuple_indexes, ";", 'S');
    go_to(add_empty_return, {'M'}, tuple_indexes[0], -1, tuple_indexes);

    link_on(if_anti_nesting, add_empty_return, {'E'}, {tuple_indexes[0]});
    go_to(if_anti_nesting, {'M'}, tuple_indexes[0], -1, tuple_indexes);


    auto temp = tuple_indexes;
    temp.push_back(0);
    temp.push_back(1);


    for (int i=1; i<tuple_indexes.size(); i++){
        int index = tuple_indexes[i];
        go_to(if_anti_nesting, {'O', 'H'}, tuple_indexes[0], 1, tuple_indexes);
        IncompleteSet onO{"onO_"+ to_string(counter), "onO_"+ to_string(counter)};
        copy_till(onO, {'H'}, tuple_indexes[0], index, 1, 1, temp);
        link_on(if_anti_nesting, onO, {'O'}, {tuple_indexes[0]});
        go_to(if_anti_nesting, {'M'}, tuple_indexes[0], -1, tuple_indexes);

        copy_till(if_anti_nesting, {'E'}, tuple_indexes[0], index, 1, 1, temp);
        go_to(if_anti_nesting, {'K'}, tuple_indexes[0], -1, tuple_indexes);
        copy_till(if_anti_nesting, {'N'}, tuple_indexes[0], index, 1, 1, temp);

        go_to(if_anti_nesting, {'L'}, tuple_indexes[0], 1, tuple_indexes);
        copy_till(if_anti_nesting, {'M'}, tuple_indexes[0], index, 1, 1, temp);
        move(if_anti_nesting, {0,1}, -1);

        go_to(if_anti_nesting, {'M'}, tuple_indexes[0], 1, tuple_indexes);
        copy_till(if_anti_nesting, {'E'}, tuple_indexes[0], index, 1, 1, temp);


        go_to(if_anti_nesting, {'E'}, tuple_indexes[0], 1, tuple_indexes);
        copy_till(if_anti_nesting, {'H'}, tuple_indexes[0], index, 1, 1, temp);

        link_put(if_anti_nesting, {'E'}, {0});
        go_to(if_anti_nesting, {'K'}, tuple_indexes[0], -1, tuple_indexes);
        go_to(if_anti_nesting, {'A'}, 0, -1, {0, 1});

        copy_till(if_anti_nesting, {'E'}, 0, 1, index, 1, temp);


        go_to(if_anti_nesting, {'A'}, tuple_indexes[0], -1, tuple_indexes);
        go_to(if_anti_nesting, {'O', 'H'}, tuple_indexes[0], 1, tuple_indexes);

        go_to(if_anti_nesting, {'M'}, tuple_indexes[0], -1, tuple_indexes);


        go_to_clear(if_anti_nesting, {'A'}, 0, -1, {0,1}, {0,1});
        link_put(if_anti_nesting, {'\u0000'}, {1});

    }


    //clear everything after O
    go_to(if_anti_nesting, {'O','H'}, tuple_indexes[0], 1, tuple_indexes);
    IncompleteSet onO2{"onO2_"+ to_string(counter), "onO2_"+ to_string(counter)};
    counter++;
    move(onO2, tuple_indexes, 1);
    go_to_clear(onO2, {'H'}, tuple_indexes[0], 1, tuple_indexes, tuple_indexes);

    link_on(if_anti_nesting, onO2, {'O'}, {tuple_indexes[0]});

    go_to(if_anti_nesting, {'H'}, tuple_indexes[0], 1, tuple_indexes);
    go_to_clear(if_anti_nesting, {'A'}, tuple_indexes[0], -1, tuple_indexes, {tuple_indexes[0]});

    link(a, if_anti_nesting);
}

void TuringTools::addToken(IncompleteSet& a, const vector<int>&tuple_indexes, const string& s, char ch, char replace) {
    for (char c: s){
        link_put(a, {c}, {1});
        move(a, {0,1}, 1);
    }
    link_put(a, {'E'}, {0});

    go_to(a, {'H'}, tuple_indexes[0], 1, tuple_indexes);
    go_to(a, {'A'}, 0, -1, {0,1});
    make_token(a, tuple_indexes, ch, replace);

    go_to_clear(a, {'A'}, 0, -1, {0,1}, {0, 1});
    link_put(a, {'\u0000'}, {1});
}

void TuringTools::makeAntiNestingIfSolo(IncompleteSet &a, const vector<int> &tuple_indexes) {
    IncompleteSet solo_anti_nesting{"solo_anti_nesting_"+ to_string(counter), "solo_anti_nesting_"+ to_string(counter)};
    counter++;

    go_to(solo_anti_nesting, {'\u0000'}, tuple_indexes[1], 1, tuple_indexes);
    link_put(solo_anti_nesting, {'H'}, {tuple_indexes[0]});

    addToken(solo_anti_nesting, tuple_indexes, "\nif", '0', 'F');
    addToken(solo_anti_nesting, tuple_indexes, "(!", '(');

    go_to(solo_anti_nesting, {'H'}, tuple_indexes[0], 1, tuple_indexes);

    go_to(solo_anti_nesting, {'V'}, tuple_indexes[0], -1, tuple_indexes);
    move(solo_anti_nesting, tuple_indexes, -1);
    link_put(solo_anti_nesting, {'C'}, {tuple_indexes[0]});
    skip_nesting(solo_anti_nesting, stack_tape, 1, tuple_indexes[1], -1, tuple_indexes, ')', '(');
    link_put(solo_anti_nesting, {'B'}, {tuple_indexes[0]});
    copyTupleRight(solo_anti_nesting, tuple_indexes, 'B', 'C');

    addToken(solo_anti_nesting, tuple_indexes, ")", ')');
    addToken(solo_anti_nesting, tuple_indexes, ")", ')');

    //clear B and C marker
    go_to(solo_anti_nesting, {'B'}, tuple_indexes[0], -1, tuple_indexes);
    link_put(solo_anti_nesting, {'\u0000'}, {tuple_indexes[0]});
    go_to(solo_anti_nesting, {'C'}, tuple_indexes[0], 1, tuple_indexes);
    link_put(solo_anti_nesting, {'\u0000'}, {tuple_indexes[0]});

    go_to(solo_anti_nesting, {'V'}, tuple_indexes[0], 1, tuple_indexes);
    skip_nesting(solo_anti_nesting, stack_tape, 1, tuple_indexes[1], 1, tuple_indexes);
    link_put(solo_anti_nesting, {'B'}, {tuple_indexes[0]});
    move(solo_anti_nesting, tuple_indexes, 1);
    link_put(solo_anti_nesting, {'C'}, {tuple_indexes[0]});
    go_to(solo_anti_nesting, {'}'}, tuple_indexes[1], 1, tuple_indexes);
    link_put(solo_anti_nesting, {'D'}, {tuple_indexes[0]});

    addToken(solo_anti_nesting, tuple_indexes, "{", '{');

    go_to(solo_anti_nesting, {'H'}, tuple_indexes[0], 1, tuple_indexes);

    go_to(solo_anti_nesting, {'C'}, tuple_indexes[0], -1, tuple_indexes);

    go_to_multiple(solo_anti_nesting, {{'D'},{'R'}}, {tuple_indexes[0], tuple_indexes[1]}, 1, {tuple_indexes});
    IncompleteSet add_empty_return{"add_empty_return_"+ to_string(counter), "add_empty_return_"+ to_string(counter)};
    counter++;
    //push empty return
    addToken(add_empty_return, tuple_indexes, "return", 'R', 'O');
    addToken(add_empty_return, tuple_indexes, ";", 'S');
    go_to(add_empty_return, {'C'}, tuple_indexes[0], -1, tuple_indexes);

    link_on(solo_anti_nesting, add_empty_return, {'D'}, {tuple_indexes[0]});


    go_to(solo_anti_nesting, {'C'}, tuple_indexes[0], -1, tuple_indexes);
    copyTupleRight(solo_anti_nesting, tuple_indexes, 'C', 'D');

    addToken(solo_anti_nesting, tuple_indexes, "}", '}');

    go_to(solo_anti_nesting, {'V'}, tuple_indexes[0], -1, tuple_indexes);
    move(solo_anti_nesting, tuple_indexes, 1);
    link_put(solo_anti_nesting, {'E'}, {tuple_indexes[0]});
    copyTupleRight(solo_anti_nesting, tuple_indexes, 'E', 'B');

    go_to(solo_anti_nesting, {'H'}, tuple_indexes[0], 1, tuple_indexes);
    go_to(solo_anti_nesting, {'F'}, tuple_indexes[0], -1, tuple_indexes);



    //first copy F-H than C-F
    auto temp = tuple_indexes;
    temp.push_back(0);
    temp.push_back(1);


    for (int i=1; i<tuple_indexes.size(); i++){
        int index = tuple_indexes[i];

        copy_till(solo_anti_nesting, {'H'}, tuple_indexes[0], index, 1, 1, temp);
        go_to(solo_anti_nesting, {'C'}, tuple_indexes[0], -1, tuple_indexes);
        copy_till(solo_anti_nesting, {'F'}, tuple_indexes[0], index, 1, 1, temp);

        go_to(solo_anti_nesting, {'S'}, tuple_indexes[0], -1, tuple_indexes);

        link_put(solo_anti_nesting, {'E'}, {0});
        go_to(solo_anti_nesting, {'A'}, 0, -1, {0, 1});

        copy_till(solo_anti_nesting, {'H'}, tuple_indexes[0], 1, index, 1, temp);

        go_to(solo_anti_nesting, {'H'}, tuple_indexes[0], 1, tuple_indexes);
        go_to(solo_anti_nesting, {'F'}, tuple_indexes[0], -1, tuple_indexes);

        go_to_clear(solo_anti_nesting, {'A'}, 0, -1, {0,1}, {0,1});

    }
    go_to(solo_anti_nesting, {'H'}, tuple_indexes[0], 1, tuple_indexes);
    go_to_clear(solo_anti_nesting, {'A'}, tuple_indexes[0], -1, tuple_indexes, {tuple_indexes[0]});
    link(a, solo_anti_nesting);
}

set<IncompleteTransition> TuringTools::mergeToSingle(const set<IncompleteTransition> &m) {
    set<IncompleteTransition> out;
    set<IncompleteTransition> temp_out = m;
    set<IncompleteTransition> temp_out2;

    vector<char> inputs;
    vector<int> inputs_index;
    vector<char> outputs;
    vector<int> outputs_index;
    vector<int> move_index;

    vector<int> increase_index;
    vector<int> increase_amount;
    IncompleteTransition a;
    IncompleteTransition b;
    IncompleteTransition new_t;


    while (temp_out.size() > 1){
        for (auto it_i = temp_out.begin(); it_i != temp_out.end(); it_i++){
            for (auto it_j = it_i; it_j != temp_out.end(); it_j++){

                if (it_i == it_j){
                    continue;
                }
                a = *it_i;
                b = *it_j;

                if (a.state != b.state || a.to_state != b.to_state){
                    continue;
                }

                inputs.clear();
                inputs_index.clear();
                outputs.clear();
                outputs_index.clear();
                move_index.clear();
                increase_amount.clear();
                increase_index.clear();

                int a_c = 0;
                int b_c = 0;
                int a_index;
                int b_index;
                while (a_c < a.input.size() && b_c < b.input.size()){
                    a_index = a.input_index[a_c];
                    b_index = b.input_index[b_c];
                    if (a_index == b_index){

                        inputs.push_back(a.input[a_c]);
                        inputs_index.push_back(a_index);

                        a_c += 1;
                        b_c += 1;
                        continue;
                    }

                    if (a_index < b_index){

                        inputs.push_back(a.input[a_c]);
                        inputs_index.push_back(a_index);
                        a_c += 1;
                    }else{
                        inputs.push_back(b.input[b_c]);
                        inputs_index.push_back(b_index);
                        b_c += 1;
                    }
                }

                if (a_c < a.input.size()){
                    inputs.insert(inputs.end(), a.input.begin()+a_c, a.input.end());
                    inputs_index.insert(inputs_index.end(), a.input_index.begin()+a_c, a.input_index.end());
                }else if (b_c < b.input.size()){
                    inputs.insert(inputs.end(), b.input.begin()+b_c, b.input.end());
                    inputs_index.insert(inputs_index.end(), b.input_index.begin()+b_c, b.input_index.end());
                }

                a_c = 0;
                b_c = 0;

                while (a_c < a.output.size() && b_c < b.output.size()){
                    a_index = a.output_index[a_c];
                    b_index = b.output_index[b_c];
                    if (a_index == b_index){

                        outputs.push_back(a.output[a_c]);
                        outputs_index.push_back(a.output_index[a_c]);
                        move_index.push_back(a.move[a_c]);
                        a_c += 1;
                        b_c += 1;
                        continue;
                    }

                    if (a_index < b_index){

                        outputs.push_back(a.output[a_c]);
                        outputs_index.push_back(a_index);
                        move_index.push_back(a.move[a_c]);
                        a_c += 1;
                    }else{
                        outputs.push_back(b.output[b_c]);
                        outputs_index.push_back(b_index);
                        move_index.push_back(b.move[b_c]);
                        b_c += 1;
                    }
                }

                if (a_c < a.output.size()){
                    outputs.insert(outputs.end(), a.output.begin()+a_c, a.output.end());
                    outputs_index.insert(outputs_index.end(), a.output_index.begin()+a_c, a.output_index.end());
                    move_index.insert(move_index.end(), a.move.begin()+a_c, a.move.end());
                }else if (b_c < b.input.size()){
                    outputs.insert(outputs.end(), b.output.begin()+b_c, b.output.end());
                    outputs_index.insert(outputs_index.end(), b.output_index.begin()+b_c, b.output_index.end());
                    move_index.insert(move_index.end(), b.move.begin()+b_c, b.move.end());
                }


                new_t.def_move = a.def_move;
                new_t.state = a.state;
                new_t.to_state = a.to_state;
                new_t.input = inputs;
                new_t.input_index = inputs_index;
                new_t.output = outputs;
                new_t.output_index = outputs_index;
                new_t.move = move_index;
                new_t.control_increase = a.control_increase;

                int multi = 1;
                if (!a.control_increase.empty() && a.increase_amount[0] < 0){
                    multi = -1;
                }

                new_t.increase_amount = {(int) std::count(inputs.begin(), inputs.end(), 'X')*multi};




                if (temp_out2.find(new_t) == temp_out2.end()){
                    temp_out2.insert(new_t);
                }
            }
        }

        out.insert(temp_out.begin(), temp_out.end());
        temp_out = std::move(temp_out2);
        temp_out2.clear();

    }
    out.insert(temp_out.begin(), temp_out.end());
    return out;
}

Transition TuringTools::make_transition(IncompleteTransition &incomp, int tapes) {
    Transition transition;
    transition.state = incomp.state;
    vector<char> inputs;
    vector<char> outputs;
    vector<int> moves;

    int def_move = incomp.def_move;

    bool input_empty = incomp.input_index.empty();
    bool output_empty = incomp.output_index.empty();

    for (int i = 0; i< tapes; i++){
        if (!input_empty && incomp.input_index.front() == i){
            inputs.push_back(incomp.input.front());
            incomp.input.erase(incomp.input.begin());
            incomp.input_index.erase(incomp.input_index.begin());
        }else{
            inputs.push_back('\u0001');
        }

        if (!output_empty && incomp.output_index.front() == i){
            outputs.push_back(incomp.output.front());
            moves.push_back(incomp.move.front());
            incomp.output.erase(incomp.output.begin());
            incomp.move.erase(incomp.move.begin());
            incomp.output_index.erase(incomp.output_index.begin());
        }else{
            outputs.push_back('\u0001');
            moves.push_back(def_move);
        }
    }

    transition.input = inputs;
    Production p;
    p.new_state = incomp.to_state;
    p.replace_val = outputs;
    p.movement = moves;
    p.control_increase = incomp.control_increase;
    p.increase_amount = incomp.increase_amount;
    transition.production = p;
    return transition;
}

void TuringTools::check_split_nesting(IncompleteSet &a) {
    //requires after done find traverse
    //now check if outside split nesting, if inside split nesting ignore
    //find must be != \u0000
    //already on a find position
    //first put keep in place

    IncompleteSet check_split_nesting{"check_split_nesting_"+ to_string(counter), "check_split_nesting_"+ to_string(counter)};
    counter++;
    move(check_split_nesting, {(int) stack_tape}, 1);
    link_put(check_split_nesting, {'!'}, {(int) stack_tape});

    //stack setup

    move(check_split_nesting, {0,1}, 1);
    link_put(check_split_nesting, {heap_sep}, {1});
    move(check_split_nesting, {0,1}, 1);

    //give 1 extra nesting margin for parameters
    IncompleteSet check_param_loop{"check_param_"+ to_string(counter), "check_param_"+ to_string(counter)};
    counter++;
    go_to(check_param_loop, {'}', '{', stack_symbol, heap_sep}, stack_tape, 1, {(int) stack_tape});

    string reached_end = branch_on(check_param_loop, {'}'}, {(int) stack_tape});

    string not_oke_1 = branch_on(check_param_loop, {'{'}, {(int) stack_tape});
    string not_oke_2 = branch_on(check_param_loop, {stack_symbol}, {(int) stack_tape});

    link_put(check_param_loop, {'0'}, {1});
    move(check_param_loop, {0,1}, 1);
    move(check_param_loop, {(int) stack_tape}, 1);
    make_loop(check_param_loop);
    check_param_loop.to_state = reached_end;

    link(check_split_nesting, check_param_loop);

    //above we pushed the amount of params passed including self on working tape
    //now we will pop for every D we pass
    go_to(check_split_nesting, {heap_sep}, stack_tape, 1, {(int) stack_tape});
    go_to(check_split_nesting, {'#'}, stack_tape, -1, {(int) stack_tape});
    move(check_split_nesting, {0,1}, -1);
    move(check_split_nesting, {(int) stack_tape}, 1);

    IncompleteSet check_param_loop2{"check_param2_"+ to_string(counter), "check_param2_"+ to_string(counter)+"_never_reached"};
    counter++;

    IncompleteTransition looper;
    looper.state = check_param_loop2.state;
    looper.to_state = check_param_loop2.state;
    looper.def_move = 0;

    looper.input = {'0', 'D'};
    looper.input_index = {1, (int) stack_tape};

    looper.output = {'\u0001', '\u0000', '\u0001'};
    looper.output_index = {0, 1, (int) stack_tape};
    looper.move = {-1, -1, 1};

    check_param_loop2.transitions.push_back(looper);

    string suc6_state = "suc6_"+ to_string(counter);
    counter++;

    string failure_state = "failure_"+ to_string(counter);
    counter++;

    IncompleteTransition suc6;
    suc6.state = check_param_loop2.state;
    suc6.to_state = suc6_state;
    suc6.def_move = 0;

    suc6.input = {heap_sep};
    suc6.input_index = {1};
    check_param_loop2.transitions.push_back(suc6);


    IncompleteTransition failure;
    failure.state = check_param_loop2.state;
    failure.to_state = failure_state;
    failure.def_move = 0;

    failure.input = {'0'};
    failure.input_index = {1};
    check_param_loop2.transitions.push_back(failure);

    check_param_loop2.to_state = suc6_state;
    link_put(check_param_loop2, {'\u0000'}, {1});
    move(check_param_loop2, {0,1}, -1);
    suc6_state = check_param_loop2.to_state;


    for (auto s: {failure_state, not_oke_1, not_oke_2}){
        check_param_loop2.to_state = s;
        go_to_clear(check_param_loop2, {heap_sep}, 1, -1, {0,1}, {1});
        link_put(check_param_loop2, {'\u0000'}, {1});
        move(check_param_loop2, {0,1}, -1);
        go_to(check_param_loop2, {'!'}, stack_tape, -1, {(int) stack_tape});

        IncompleteTransition toSuc6;
        toSuc6.state = check_param_loop2.to_state;
        toSuc6.to_state = suc6_state;
        toSuc6.def_move = 0;
        check_param_loop2.transitions.push_back(toSuc6);
    }
    check_param_loop2.to_state = suc6_state;

    link(check_split_nesting, check_param_loop2);

    IncompleteSet br{"b", "br"};
    //link(check_split_nesting, br);


    //IncompleteSet br{"b", "br"};
    //link(check_split_nesting, br);

    std::vector<string> branches;

    for (int i=0; i<split_nesting+2; i++){
        go_to(check_split_nesting, {'}', '{', stack_symbol}, stack_tape, 1, {(int) stack_tape});

        IncompleteSet skip_nesting_set{"skip_nesting_"+ to_string(counter), "skip_nesting_"+ to_string(counter)};
        skip_nesting(skip_nesting_set, 1, 1, stack_tape, 1, {(int) stack_tape});

        link_on(check_split_nesting, skip_nesting_set, {'{'}, {(int) stack_tape});

        string branch_fine = branch_on(check_split_nesting, {stack_symbol}, {(int) stack_tape});
        branches.push_back(branch_fine);
        move(check_split_nesting, {(int) stack_tape}, 1);

    }

    go_to(check_split_nesting, {'!'}, stack_tape, -1, {(int) stack_tape});
    link_put(check_split_nesting, {'#'}, {(int) stack_tape});
    go_to(check_split_nesting, {'\u0000'}, stack_tape, -1, {(int) stack_tape});

    IncompleteSet cleanup{"check_split_cleanup_"+ to_string(counter), "check_split_cleanup_"+ to_string(counter)};
    counter++;
    go_to(cleanup, {'!'}, stack_tape, -1, {(int) stack_tape});
    link_put(cleanup, {'#'}, {(int) stack_tape});
    move(cleanup, {(int) stack_tape}, -1);

    for (auto b: branches){
        IncompleteTransition branch_to_sub;
        branch_to_sub.state = b;
        branch_to_sub.to_state = cleanup.state;
        branch_to_sub.def_move = 0;

        check_split_nesting.transitions.push_back(branch_to_sub);
    }

    //link cleanup
    IncompleteTransition cleanupBack;
    cleanupBack.state = cleanup.to_state;
    cleanupBack.to_state = check_split_nesting.to_state;
    cleanupBack.def_move = 0;

    check_split_nesting.transitions.push_back(cleanupBack);
    check_split_nesting.transitions.insert(check_split_nesting.transitions.end(), cleanup.transitions.begin(), cleanup.transitions.end());

    link(a, check_split_nesting);

}

string TuringTools::check_stack_double(IncompleteSet& a) {

    IncompleteSet find_var{"find_var_"+ to_string(counter), "find_var_"+ to_string(counter)};

    go_to(find_var, {'{'}, 1, -1, {0,1});
    move(find_var, {0,1}, -1);
    go_to(find_var, {'{'}, 1, -1, {0,1});
    link_put(find_var, {'B'}, {0});

    IncompleteSet check_on_stack{"check_on_stack_"+ to_string(counter), "check_on_stack_"+ to_string(counter)};
    counter++;

    go_to(check_on_stack, {'-'}, stack_tape, -1, {(int) stack_tape});
    go_to(check_on_stack, {stack_sep}, stack_tape, 1, {(int) stack_tape});
    IncompleteSet check_stack_loop{"check_on_stack_loop_"+ to_string(counter), "check_on_stack_loop_"+ to_string(counter)};
    counter++;
    move(check_stack_loop, {0, 1, (int) stack_tape}, 1);

    //assume startpos is S and endpos is E on working tape
    IncompleteSet compareStringLoop{"compareStringLoop_"+ to_string(counter), "compareStringLoop_"+ to_string(counter)+"_unreached"};
    counter++;
    for (int j =31; j<128; j++){
        char c = (char) j;
        if (j == 127){
            c = '\u0000';
        }
        if (j == 31){
            c = '\n';
        }

        IncompleteTransition move_next;
        move_next.state = compareStringLoop.state;
        move_next.to_state = compareStringLoop.state+"_sub"+c;
        move_next.def_move = 0;
        move_next.input = {c};
        move_next.input_index = {1};
        compareStringLoop.transitions.push_back(move_next);

        IncompleteTransition move_next2;
        move_next2.state = compareStringLoop.state+"_sub"+c;
        move_next2.to_state = compareStringLoop.state;
        move_next2.def_move = 0;
        move_next2.input = {c};
        move_next2.input_index = {(int) stack_tape};

        move_next2.output = {'\u0001', '\u0001', '\u0001'};
        move_next2.output_index = {0, 1, (int) stack_tape};
        move_next2.move = {1,1, 1};

        compareStringLoop.transitions.push_back(move_next2);

        IncompleteTransition move_break;
        move_break.state = compareStringLoop.state+"_sub"+c;
        move_break.to_state = to_string(counter);
        move_break.def_move = 0;

        compareStringLoop.transitions.push_back(move_break);

    }


    compareStringLoop.to_state = to_string(counter);
    counter++;

    IncompleteSet onFullRead{"onFullRead_"+ to_string(counter), "onFullRead_"+ to_string(counter)};
    counter++;
    string equals = branch_on(onFullRead, {'{'}, {1});

    link_on(compareStringLoop, onFullRead, {heap_sep}, {(int) stack_tape});

    string different = branch_on(compareStringLoop, {'\u0000'}, {(int) stack_tape});

    go_to(compareStringLoop, {'B'}, 0, -1, {0,1});
    go_to(compareStringLoop, {stack_sep, '\u0000'}, stack_tape, 1, {(int) stack_tape});

    string different2 = branch_on(compareStringLoop, {'\u0000'}, {(int) stack_tape});

    link(check_stack_loop, compareStringLoop);

    make_loop(check_stack_loop);
    check_stack_loop.to_state = "never_reached";

    link(check_on_stack, check_stack_loop);

    link(find_var, check_on_stack);

    IncompleteSet on_different{"on_different_"+ to_string(counter), "on_different_"+ to_string(counter)};
    counter++;
    go_to(on_different, {'B'}, 0, -1, {0,1});
    link_put(on_different, {'\u0000'}, {0});
    go_to(on_different, {'S'}, 0, 1, {0,1});

    for (auto& d: {different, different2}){
        IncompleteTransition toDifferent;
        toDifferent.state = d;
        toDifferent.to_state = on_different.state;
        toDifferent.def_move = 0;

        find_var.transitions.push_back(toDifferent);
    }

    find_var.transitions.insert(find_var.transitions.begin(), on_different.transitions.begin(), on_different.transitions.end());

    IncompleteTransition fromDifferent;
    fromDifferent.state = on_different.to_state;
    fromDifferent.to_state = find_var.to_state;
    fromDifferent.def_move = 0;

    find_var.transitions.push_back(fromDifferent);

    IncompleteSet on_equals{"on_equals_"+ to_string(counter), "on_equals_"+ to_string(counter)};
    go_to(on_equals, {'B'}, 0, -1, {0,1});
    link_put(on_equals, {'\u0000'}, {0});
    go_to(on_equals, {'S'}, 0, 1, {0,1});

    IncompleteTransition toEqual;
    toEqual.state = equals;
    toEqual.to_state = on_equals.state;
    toEqual.def_move = 0;

    find_var.transitions.push_back(toEqual);
    find_var.transitions.insert(find_var.transitions.begin(), on_equals.transitions.begin(), on_equals.transitions.end());


    link(a, find_var);

    return on_equals.to_state;
}

void TuringTools::store_param_count(IncompleteSet &a, const vector<int>&tuple_indexes) {
    //stores amount of param between () before { on working tape
    //used when making path for  loops to identify the difference between for loop param and just defined param

    IncompleteSet store_param_count{"store_param_count_"+ to_string(counter), "store_param_count_"+ to_string(counter)};
    counter++;

    link_put(store_param_count, {'B'}, {tuple_indexes[0]});
    move(store_param_count, tuple_indexes, -1);

    //on without parameters
    IncompleteSet on_param_less{"on_param_less_"+ to_string(counter), "on_param_less_"+ to_string(counter)};
    counter++;
    link_put(on_param_less, {'N'}, {1});
    move(on_param_less, {0,1}, 1);

    link_on_not(store_param_count, on_param_less, {')'}, {tuple_indexes[1]});

    IncompleteSet on_param_full{"on_param_full_"+ to_string(counter), "on_param_full_"+ to_string(counter)};
    counter++;
    skip_nesting(on_param_full, stack_tape, 1, tuple_indexes[1], -1, tuple_indexes, '(', ')');
    link_put(on_param_full, {'N'}, {1});
    move(on_param_full, {0,1}, 1);

    IncompleteSet go_to_B_loop{"go_to_B_loop_"+ to_string(counter), "go_to_B_loop_"+ to_string(counter)};
    counter++;
    go_to_multiple(go_to_B_loop, {{'B'}, {'D'}}, {tuple_indexes[0], tuple_indexes[1]}, 1, tuple_indexes);

    string end_loop = branch_on(go_to_B_loop, {'B'}, {tuple_indexes[0]});

    link_put(go_to_B_loop, {'D'}, {1});
    move(go_to_B_loop, {0,1}, 1);

    move(go_to_B_loop, tuple_indexes, 1);
    make_loop(go_to_B_loop);
    go_to_B_loop.to_state = end_loop;


    link(on_param_full, go_to_B_loop);

    link_on(store_param_count, on_param_full, {')'}, {tuple_indexes[1]});

    go_to(store_param_count, {'B'}, tuple_indexes[0], 1, tuple_indexes);
    link_put(store_param_count, {'\u0000'}, {tuple_indexes[0]});

    link(a, store_param_count);


}

