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
    //copy from marker 'A' or 'S' till marker 'E'
    IncompleteSet copier{"copy_to_working_main_"+ to_string(counter), "copy_to_working_main_"+ to_string(counter)};
    counter++;

    IncompleteSet copier_do{"copier_do_"+ to_string(counter), "copier_do_"+ to_string(counter)};
    counter++;

    IncompleteSet copier_do_sub{"copier_do_sub_"+ to_string(counter), "copier_do_sub_"+ to_string(counter)};
    counter++;


    for (int i = 0; i< tuple_indexes.size(); i++){
        if (i < 2){
            continue;
        }

        IncompleteSet copy_to_working("copy_to_working_" + to_string(counter), "copy_to_working_" + to_string(counter));
        counter++;
        //copy to start_tapes
        copy(copy_to_working, tuple_indexes[i], 1);
        move(copy_to_working, {0, 1}, 1);

        link_on_not(copier_do_sub, copy_to_working, {'\u0000'}, {tuple_indexes[i]});

    }

    move(copier_do_sub, tuple_indexes, 1);

    link_on_multiple(copier_do, copier_do_sub, {{'A'}, {'S'}, {'\u0000'}}, {tuple_indexes[0]});
    string end_loop = branch_on(copier_do, {'E'}, {tuple_indexes[0]});
    make_loop(copier_do);
    copier_do.to_state = end_loop;
    link(copier, copier_do);
    go_to(copier, {'S', 'A'}, tuple_indexes[0], -1, tuple_indexes);
    link_put(copier, {'E'}, {0});
    link(a,copier);

}


void TuringTools::heap_push_definer(IncompleteSet& a, const vector<int>&tuple_indexes) {
    //requires that string that is marked is seperated by at least 1 space
    IncompleteSet push_heap_action{"push_heap_"+ to_string(counter), "push_heap_"+ to_string(counter)};
    counter++;

    //go to heap mode
    go_to(push_heap_action, {'*'}, stack_tape, -1, {(int) stack_tape});
    heap_mode = true;

    copy_to_working(push_heap_action, tuple_indexes);

    //from here on we will copy data from working tape and put it on the heap

    go_to(push_heap_action, {' '}, 1, -1, {0,1});
    link_put(push_heap_action, {'P'}, {0});

    //check if this starts with A or S
    go_to(push_heap_action, {'S', 'A'}, 0, -1, {0,1});

    //store part on heap after insert on working tape after new data
    IncompleteSet move_heap{"move_heap_"+ to_string(counter), "move_heap_"+ to_string(counter)};
    counter++;

    go_to(move_heap, {'A'}, 0, -1, {0,1});
    find_match_heap(move_heap, 'A', 'S', 0, 1);

    //go to end of nesting
    //TODO: still need to count nestings
    go_to(move_heap, {'{'}, stack_tape, -1, {(int) stack_tape});
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
    link_put(push_heap_action, {'}'}, {(int) stack_tape});
    move(push_heap_action, {(int) stack_tape}, -1);
    link_put(push_heap_action, {'#'}, {(int) stack_tape});
    move(push_heap_action, {(int) stack_tape}, -1);
    link_put(push_heap_action, {'{'}, {(int) stack_tape});
    move(push_heap_action, {(int) stack_tape}, -1);
    link_put(push_heap_action, {'#'}, {(int) stack_tape});
    move(push_heap_action, {(int) stack_tape}, -1);

    //check if part heap is still on working tape
    go_to(push_heap_action, {'E'}, 0, 1, {0,1});

    IncompleteSet fix_heap{"fix_heap_"+ to_string(counter), "fix_heap_"+ to_string(counter)};
    counter++;

    go_to_move(fix_heap, {'\u0000'}, 1, 1, {0,1}, stack_tape, -1, {(int) stack_tape});
    go_to_clear(fix_heap, {'E'}, 0, -1, {0,1}, {0,1});


    //string branch_break = branch_on(fix_heap, {'E'}, {0});
    link_on_not(push_heap_action, fix_heap, {'\u0000'}, {1});

    //clear working tapes
    go_to(push_heap_action, {'E'}, 0, 1, {0,1});
    go_to_clear(push_heap_action, {'A', 'S'}, 0, -1, {0,1}, {0,1});
    link_put(push_heap_action, {'\u0000'}, {1});

    //store definer again on working tape
    go_to(push_heap_action, {'!'}, (int) stack_tape, 1, {(int) stack_tape});
    go_to(push_heap_action, {'#'}, (int) stack_tape, 1, {(int) stack_tape});
    move(push_heap_action, {(int) stack_tape}, -1);

    //remove 'S' markers
    write_on(push_heap_action, {'S'}, {0}, {'\u0000'}, {0});

    //make nesting on working tape
    go_to_copy(push_heap_action, {':'}, stack_tape, -1, {(int) stack_tape}, 1, 1, {0, 1});
    link_put(push_heap_action, {'{'}, {1});
    move(push_heap_action, {0,1}, 1);
    link_put(push_heap_action, {'S'}, {0});

    //change '!' to '#'
    go_to(push_heap_action, {'!'}, (int) stack_tape, -1, {(int) stack_tape});
    link_put(push_heap_action, {'#'}, {(int) stack_tape});

    //go back to stack mode
    //move(push_heap_action, {(int) stack_tape}, 1);
    go_to(push_heap_action, {'\u0000'}, (int) stack_tape, 1, {(int) stack_tape});
    heap_mode = false;


    link_on_multiple(a, push_heap_action, {{'A'}, {'S'}}, {tuple_indexes[0]});
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

    counter++;

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
        //throw "not in heap mode";
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
    //go_to(searcher, {start_marker}, marker_tape, -1, {marker_tape, data_tape});

    //recreate search loop
    move(searcher, {marker_tape, data_tape}, -1);

    IncompleteSet change_stack_pos{"change_stack_pos_"+ to_string(counter), "change_stack_pos_"+ to_string(counter)};
    counter++;
    go_to(change_stack_pos, {'#'}, stack_tape, -1, {(int) stack_tape});
    //TODO: find '}' will need to be removed in future, so we can make defined, function multi-compatible
    go_to(change_stack_pos, {'}'}, stack_tape, -1, {(int) stack_tape});
    move(change_stack_pos, {marker_tape, data_tape}, 1);

    link_on(searcher, change_stack_pos, {'{'}, {data_tape});

    make_loop_on(searcher, '}', stack_tape);


    //TODO: still need to count nestings
    go_to(searcher, {'}'}, stack_tape, -1, {(int) stack_tape});
    go_to(searcher, {'{'}, stack_tape, -1, {(int) stack_tape});
    move(searcher, {(int) stack_tape}, -1);

    //loop somewhere here
    //TODO: make loop to search again

    //after loop on found
    searcher.to_state = branch;
    go_to(searcher, {'}'}, stack_tape, -1, {(int) stack_tape});

    link(a, searcher);

}
