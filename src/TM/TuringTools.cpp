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


IncompleteSet::IncompleteSet(const string& state, const string& to_state): state{state}, to_state{to_state} {

}

TuringTools::TuringTools(unsigned int stack_tape) {
    goto_counter = 0;
    counter = 0;
    branch_counter = 0;
    this->stack_tape = stack_tape;
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

    arrived.input = {'*'};
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

void TuringTools::copy_to_working(IncompleteSet &a, const vector<int> &tuple_indexes) {
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

        link_on_not(copier_do_sub, copy_to_working_check, {'\u0000'}, {tuple_indexes[i]});

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


void TuringTools::heap_push_definer(IncompleteSet& a, const vector<int>&tuple_indexes, bool function) {
    //requires that string that is marked is seperated by at least 1 space
    IncompleteSet push_heap_action{"push_heap_"+ to_string(counter), "push_heap_"+ to_string(counter)};
    counter++;

    copy_to_working(push_heap_action, tuple_indexes);

    //from here on we will copy data from working tape and put it on the heap
    go_to(push_heap_action, {' '}, 1, -1, {0,1});
    link_put(push_heap_action, {'P'}, {0});

    //manipulate hierarchy in case of object type
    //make sure class::func equals class{func}
    IncompleteSet object_hierarchy{"object_hierarchy_"+ to_string(counter), "object_hierarchy_"+ to_string(counter)};
    counter++;
    //copy class object to stack
    move(object_hierarchy, {0,1}, 1);
    go_to_copy(object_hierarchy, {':'}, 1, 1, {0, 1}, stack_tape, 1, {(int) stack_tape});
    push(object_hierarchy, '{');

    //copy return type on stack
    go_to(object_hierarchy, {'A', 'S'}, 0, -1, {0,1});
    go_to_copy(object_hierarchy, {' '}, 1, 1, {0, 1}, stack_tape, 1, {(int) stack_tape});
    push(object_hierarchy, ' ');
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

    go_to(object_hierarchy, {'*'}, stack_tape, -1, {(int) stack_tape});
    move(object_hierarchy, {(int) stack_tape}, 1);

    //but old markings
    go_to(object_hierarchy, {' '}, 1, -1, {0,1});
    link_put(object_hierarchy, {'P'}, {0});
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
    go_to(move_heap, {'P'}, 0, -1, {0,1});

    link_on(push_heap_action, move_heap, {'S'}, {0});

    go_to(push_heap_action, {'P'}, 0, 1, {0,1});
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

    link_put(push_heap_action, {':'}, {(int) stack_tape});
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
        go_to_copy(push_heap_action, {':'}, stack_tape, -1, {(int) stack_tape}, 1, 1, {0, 1});
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

void TuringTools::find_match_heap(IncompleteSet &a, char start_marker, char end_marker, int marker_tape, int data_tape) {
    //precondition: in heap mode
    if (!heap_mode){
        throw "not in heap mode";
    }
    IncompleteSet searcher{"heap_search_find"+ to_string(counter), "heap_search_find"+ to_string(counter)};
    counter++;

    go_to(searcher, {'#'}, stack_tape, -1, {(int) stack_tape});
    move(searcher, {(int) stack_tape}, -1);

    IncompleteSet check_match{"check_match_"+ to_string(counter), "check_match_"+ to_string(counter+1)};


    for (int j =32; j<127; j++){
        IncompleteTransition check_equal;
        check_equal.state = "check_match_"+ to_string(counter);
        check_equal.to_state = "check_match_"+ to_string(counter);
        check_equal.def_move = 0;

        check_equal.input = {(char) j, (char) j};
        check_equal.input_index = {data_tape, (int) stack_tape};
        check_equal.output = {'\u0001', '\u0001', '\u0001'};
        check_equal.output_index = {marker_tape, data_tape, (int) stack_tape};
        check_equal.move = {1, 1, -1};

        check_match.transitions.push_back(check_equal);

        IncompleteTransition end_check;
        end_check.state = "check_match_"+ to_string(counter);
        end_check.to_state = "check_match_"+ to_string(counter+1);
        end_check.def_move = 0;

        end_check.input = {(char) j, '\u0001'};
        end_check.input_index = {data_tape, (int) stack_tape};

        check_match.transitions.push_back(end_check);
    }


    counter+=2;

    link(searcher, check_match);
    move(searcher, {marker_tape, data_tape}, 1);


    string branch = branch_on(searcher, {end_marker}, {marker_tape});

    //on not found
    string end_tape = branch_on(searcher, {'\u0000'}, {(int) stack_tape});
    string end_tape2 = branch_on(searcher, {'{'}, {(int) stack_tape});

    IncompleteSet go_to_end{"find_match_to_end_"+ to_string(counter), "find_match_to_end_"+ to_string(counter)};
    counter++;
    for (auto& b: {end_tape, end_tape2}){
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
    to_branch.to_state = branch;
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
    searcher.to_state = branch;
    go_to(searcher, {'#'}, stack_tape, -1, {(int) stack_tape});
    move(searcher, {(int) stack_tape}, -1);

    //clear 'C' characters on marker tape
    move(searcher, {marker_tape, data_tape}, -1);
    go_to_clear(searcher, {start_marker}, marker_tape, -1, {marker_tape, data_tape}, {marker_tape});
    go_to(searcher, {end_marker}, marker_tape, 1, {marker_tape, data_tape});
    link(a, searcher);

}

void TuringTools::find_match_heap_traverse(IncompleteSet &a, char start_marker, char end_marker, int marker_tape,
                                           int data_tape) {
    //requires heap mode
    //also check for variables before and after nesting
    //after should not matter for variables, just for functions
    //for functions we want to guarantee uniqueness
    find_match_heap(a, start_marker, end_marker, marker_tape, data_tape);

    IncompleteSet do_traverse_check{"do_traverse_check_"+ to_string(counter), "do_traverse_check_"+ to_string(counter)};
    counter++;

    go_to_not(do_traverse_check, {'\u0000'}, stack_tape, 1, {(int) stack_tape});

    go_to(do_traverse_check, {'A'}, marker_tape, -1, {marker_tape, data_tape});

    set_heap_mode(do_traverse_check, false);
    push(do_traverse_check, '.');
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
    go_to_move(traverse_loop, {'{', '.'}, stack_tape, -1, {(int) stack_tape}, 1, 1, {marker_tape, data_tape});
    link_put(traverse_loop, {'{', '\u0000'}, {data_tape, (int) stack_tape});

    move(traverse_loop, {marker_tape, data_tape}, 1);
    link_put(traverse_loop, {'S'}, {marker_tape});

    go_to(traverse_loop, {'A'}, 0, -1, {marker_tape, data_tape});
    set_heap_mode(traverse_loop, true);
    find_match_heap(traverse_loop, start_marker, end_marker, marker_tape, data_tape);
    //still need to move last key

    go_to(traverse_loop, {'A'}, marker_tape, -1, {marker_tape, data_tape});
    go_to(traverse_loop, {'{'}, data_tape, 1, {marker_tape, data_tape});
    move(traverse_loop, {0, 1}, 1);
    string not_found_branch = branch_on(traverse_loop, {'S', '\u0000'}, {marker_tape, (int) stack_tape});
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

    //store old nesting on working
    go_to_clear(do_traverse_check, {'A'}, marker_tape, -1, {marker_tape, data_tape}, {marker_tape, data_tape});
    go_to(do_traverse_check, {'.'}, stack_tape, -1, {(int) stack_tape});
    move(do_traverse_check, {(int) stack_tape}, 1);
    go_to_move(do_traverse_check, {'\u0000'}, stack_tape, 1, {(int) stack_tape}, data_tape, 1, {marker_tape, data_tape});
    go_to(do_traverse_check, {'.'}, stack_tape, -1, {(int) stack_tape});
    link_put(do_traverse_check, {'S', '\u0000'}, {marker_tape, (int) stack_tape});

    //go back to old heap pos
    set_heap_mode(do_traverse_check, true);
    go_to(do_traverse_check, {'!'}, stack_tape, -1, {(int) stack_tape});
    link_put(do_traverse_check, {'#'}, {(int) stack_tape});
    move(do_traverse_check, {(int) stack_tape}, -1);


    link_on(a, do_traverse_check, {'\u0000'}, {(int) stack_tape});


}

void TuringTools::skip_nesting(IncompleteSet &a, int new_stack_tape, int
stack_direction, int skip_tape, int skip_direction, const vector<int>& affected) {
    /*
     * case 1: found '{' has '0' on stack -> pop 0
     * case 2: found '{' has not '0' on stack -> push 1
     * case 3: found '}' has '1' on stack -> pop 1
     * case 4: found '}' has not '1' on stack -> push 0
     * */
    move(a, {new_stack_tape}, stack_direction);
    link_put(a, {'#'}, {new_stack_tape});

    IncompleteSet result{"skip_nesting_"+ to_string(counter), "skip_nesting_"+ to_string(counter)};
    counter++;

    go_to(result, {'{', '}'}, skip_tape, skip_direction, affected);

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

    link_on(result, case_handler_1, {'{'}, {skip_tape});
    link_on(result, case_handler_2, {'}'}, {skip_tape});

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
        go_to(to_heap_mode, {'*'}, stack_tape, -1, {(int) stack_tape});
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
        if (i <= split_nesting){
            move(forward_action, tuple_indexes, -1);
            mark_definer(forward_action, tuple_indexes);

            IncompleteSet store_definer{"store_definer_"+ to_string(counter), "store_definer_"+ to_string(counter)};
            counter++;
            make_working_nesting(store_definer, tuple_indexes);

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
    go_to(result, {'N'}, tuple_indexes[0], -1, tuple_indexes);
    skip_nesting(result, stack_tape, 1, tuple_indexes[1], 1, tuple_indexes);

    string final = result.to_state;

    for (auto& e: end_loops){
        e.to_state = final;
        result.transitions.push_back(e);

    }

    link_put(result, {'U'}, {tuple_indexes[0]});
    go_to(result, {'N'}, tuple_indexes[0], -1, tuple_indexes);

    link(a, result);

}

void TuringTools::make_working_nesting(IncompleteSet &a, const vector<int> &tuple_indexes) {

    IncompleteSet working_nesting{"make_working_nesting_"+ to_string(counter), "make_working_nesting_"+ to_string(counter)};
    counter++;

    copy_to_working(working_nesting, tuple_indexes);
    move(working_nesting, {0, 1}, -1);
    go_to_copy(working_nesting, {' ', ':'}, 1, -1, {0,1}, stack_tape, 1, {(int) stack_tape});

    IncompleteSet copy_object{"make_working_nesting_"+ to_string(counter), "make_working_nesting_"+ to_string(counter)};

    push(copy_object, ':');
    go_to_not(copy_object, {':'}, 1, -1, {0, 1});
    go_to_copy(copy_object, {' '}, 1, -1, {0, 1}, stack_tape, 1, {(int) stack_tape});
    counter++;

    link_on(working_nesting, copy_object, {':'}, {1});

    go_to(working_nesting, {'E'}, 0, 1, {0,1});
    go_to_clear(working_nesting, {'S', 'A'}, 0, -1, {0,1}, {0,1});

    link_put(working_nesting, {'\u0000'}, {1});

    move(working_nesting, {(int) stack_tape}, -1);
    go_to_move(working_nesting, {'*', '{', ':'}, stack_tape, -1, {(int) stack_tape}, 1, 1, {0,1});
    IncompleteSet replace_double_dot{"replace_double_dot_"+ to_string(counter), "replace_double_dot_"+ to_string(counter)};
    counter++;
    //replaces ':' on stack by '{' on working tape
    link_put(replace_double_dot, {'{'}, {1});
    move(replace_double_dot, {0, 1}, 1);
    link_put(replace_double_dot, {'\u0000'}, {(int) stack_tape});
    move(replace_double_dot, {(int) stack_tape}, -1);

    go_to_move(replace_double_dot, {'*', '{'}, stack_tape, -1, {(int) stack_tape}, 1, 1, {0,1});

    link_on(working_nesting, replace_double_dot, {':'}, {(int) stack_tape});

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
    go_to(working_nesting, {'O','C','U','F','I'}, tuple_indexes[1], -1, tuple_indexes);

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
    go_to(create_key, {'*', '{', 'O'}, stack_tape, -1, {(int) stack_tape});
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
    go_to_clear(pop, {'{', 'O', '*'}, stack_tape, -1, {(int) stack_tape}, {(int) stack_tape});
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
    for (int j =32; j<128; j++){
        char c = (char) j;
        if (j == 127){
            c = '\u0000';
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
    find_match_heap_traverse(write_function_header, 'A', 'S', 0, 1);

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

    push(write_function_header, '.');
    go_to_move(write_function_header, {'\u0000'}, 1, 1, {0,1}, stack_tape, 1, {(int) stack_tape});
    write_on(write_function_header, {'E'}, {0}, {'\u0000'}, {0});
    go_to(write_function_header, {'S'}, 0, -1, {0,1});

    //add 'void ' on working tape
    string void_word = "void ";
    for (char v: void_word){
        link_put(write_function_header, {v}, {1});
        move(write_function_header, {0,1}, 1);
    }

    //put stack back on working
    go_to(write_function_header, {'.'}, stack_tape, -1, {(int) stack_tape});
    link_put(write_function_header, {'\u0000'}, {(int) stack_tape});
    move(write_function_header, {(int) stack_tape}, 1);
    go_to_move(write_function_header, {'\u0000'}, stack_tape, 1, {(int) stack_tape}, 1, 1, {0,1});
    go_to_not(write_function_header, {'\u0000'}, stack_tape, -1, {(int) stack_tape});
    move(write_function_header, {(int) stack_tape}, 1);

    write_on(write_function_header, {'\u0000'}, {0}, {'E'}, {0});
    go_to(write_function_header, {'S'}, 0, -1, {0,1});


    //make function caller token
    make_token(write_function_header, tuple_indexes, 'U');

    //clear first working tape
    go_to_clear(write_function_header, {'S'}, 0, -1, {0,1}, {0,1});
    link_put(write_function_header, {'\u0000'}, {1});
    push(write_function_header, '.');

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

    go_to(copy_to_working, {'.'}, stack_tape, -1, {(int) stack_tape});
    move(copy_to_working, {(int) stack_tape}, -1);

    go_to(copy_to_working, {'.', '*'}, stack_tape, -1, {(int) stack_tape});
    string end_branch = branch_on(copy_to_working, {'*'}, {(int) stack_tape});
    IncompleteSet copy_data{"copy_data_"+ to_string(counter), "copy_data_"+ to_string(counter)};
    counter++;

    go_to(copy_data, {':'}, stack_tape, 1, {(int) stack_tape});
    move(copy_data, {(int) stack_tape}, 1);
    go_to_copy(copy_data, {'.'}, stack_tape, 1, {(int) stack_tape}, 1, 1, {0,1});
    move(copy_data, {(int) stack_tape}, -1);
    go_to(copy_data, {'.'}, stack_tape, -1, {(int) stack_tape});
    move(copy_data, {(int) stack_tape}, 1);

    //add by refrence and space on working tape
    link_put(copy_data, {'&'}, {1});
    move(copy_data, {0,1}, 1);

    link_put(copy_data, {' '}, {1});
    move(copy_data, {0,1}, 1);

    go_to_copy(copy_data, {':'}, stack_tape, 1, {(int) stack_tape}, 1, 1, {0,1});

    link_put(copy_data, {','}, {1});
    move(copy_data, {0,1}, 1);

    link_put(copy_data, {' '}, {1});
    move(copy_data, {0,1}, 1);

    //check if we don't copy a space
    IncompleteSet copy_check{"copy_check_"+ to_string(counter), "copy_check_"+ to_string(counter)};
    counter++;
    move(copy_check, {(int) stack_tape}, 1);
    link_on_not(copy_check, copy_data, {' '}, {(int) stack_tape});

    link_on(copy_to_working, copy_check, {'.'}, {(int) stack_tape});

    make_loop(copy_to_working);
    copy_to_working.to_state = end_branch;

    //remove last ',' and ' '
    move(copy_to_working, {0,1}, -1);
    link_put(copy_to_working, {'\u0000'}, {1});
    move(copy_to_working, {0,1}, -1);
    link_put(copy_to_working, {'\u0000'}, {1});

    link_put(copy_to_working, {'E'}, {0});
    link(write_function_header, copy_to_working);

    //clears all stack overhead
    go_to(write_function_header, {'\u0000'}, stack_tape, 1, {(int) stack_tape});
    go_to_clear(write_function_header, {'{'}, stack_tape, -1, {(int) stack_tape}, {(int) stack_tape});
    move(write_function_header, {(int) stack_tape}, 1);

    go_to(write_function_header, {'S'}, 0, -1, {0,1});
    make_token_splitter(write_function_header, tuple_indexes, 'D', ',');

    go_to(write_function_header, {'S'}, 0, -1, {0,1});
    //store every value that is a parameter on stack for later
    IncompleteSet stack_loop{"stack_loop_"+ to_string(counter), "stack_loop_"+ to_string(counter)};
    counter++;

    push(stack_loop, '.');
    go_to(stack_loop, {' '}, 1, 1, {0,1});
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

void TuringTools::make_token(IncompleteSet &a, const vector<int> &tuple_indexes, char def_token) {
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
    link_put(a, {'\u0000'}, {tuple_indexes[0]});

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
    go_to(remove_useless, {'.'}, stack_tape, -1, {(int) stack_tape});
    move(remove_useless, {(int) stack_tape}, 1);
    go_to_move(remove_useless, {'\u0000'}, (int) stack_tape, 1, {(int) stack_tape}, 1, 1, {0,1});
    go_to(remove_useless, {'.'}, stack_tape, -1, {(int) stack_tape});
    move(remove_useless, {(int) stack_tape}, 1);

    write_on(remove_useless, {'\u0000'}, {0}, {'E'}, {0});
    go_to(remove_useless, {'S'}, 0, -1, {0,1});

    check_var_char_working(remove_useless);

    string remove_useless_quit = branch_on(remove_useless, {'S', '\u0000'}, {0, 1});
    make_loop_on(remove_useless, 'S', 0);

    link_on(check_var_loop, remove_useless, {'S'}, {0});

    go_to(check_var_loop, {'S'}, 0, -1, {0,1});

    //move part from 'P' temporarly on stack
    push(check_var_loop, '.');
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
    go_to(check_var_loop, {'A'}, 0, -1, {0,1});
    set_heap_mode(check_var_loop, true);

    find_match_heap_traverse(check_var_loop, 'A', 'S', 0, 1);

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
    go_to(check_var_loop, {'.'}, stack_tape, -1, {(int) stack_tape});
    move(check_var_loop, {(int) stack_tape}, 1);
    go_to_move(check_var_loop, {'\u0000'}, stack_tape, 1, {(int) stack_tape}, 1, 1, {0,1});
    go_to(check_var_loop, {'.'}, stack_tape, -1, {(int) stack_tape});
    link_put(check_var_loop, {'\u0000'}, {(int) stack_tape});

    //overrides P if their are no values on stack
    link_put(check_var_loop, {'E'}, {0});

    //store heap data on stack
    //go to S in case P doesnt exist
    go_to(check_var_loop, {'P', 'S'}, 0, -1, {0,1});

    write_on(check_var_loop, {'P'}, {0}, {'\u0000'}, {1});

    go_to(check_var_loop, {'S'}, 0, -1, {0,1});
    go_to_move(check_var_loop, {'\u0000'}, 1, 1, {0, 1},  stack_tape, 1, {(int) stack_tape});
    push(check_var_loop, '.');

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
    go_to(check_var_loop, {'.'}, stack_tape, -1, {(int) stack_tape});
    move(check_var_loop, {(int) stack_tape}, 1);
    go_to_move(check_var_loop, {'\u0000'}, stack_tape, 1, {(int) stack_tape}, 1, 1, {0,1});
    go_to(check_var_loop, {'.'}, stack_tape, -1, {(int) stack_tape});
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











