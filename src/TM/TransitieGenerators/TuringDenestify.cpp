//
// Created by tibov on 22/11/23.
//

#include "TuringDenestify.h"

TuringDenestify::TuringDenestify(int split_nesting, int max_nesting): TuringGenerator(4),
split_nesting{split_nesting}, max_nesting{max_nesting} {

}

IncompleteSet TuringDenestify::getTransitions() {
    return deNestify();
}

IncompleteSet TuringDenestify::deNestify() {
    IncompleteSet result{"denestify_start", "denestify_start"};

    tools->nesting_marker(result, get_tuple_index(), split_nesting, max_nesting);
    string end_denesting = tools->branch_on(result, {'\u0000'}, {get_tuple_index()[1]});


    tools->go_to(result, {'N', 'A'}, get_tuple_index()[0], -1, get_tuple_index());

    createNewFunction(result);

    //clear stack
    //clear working
    //go to start denestifier

    tools->clear_stack(result);
    tools->go_to_clear(result, {'A'}, 0, -1, {0,1}, {0,1});
    tools->link_put(result, {'\u0000'}, {1});

    //small verify
    string looping = result.to_state;
    result.to_state = end_denesting;

    tools->go_to_clear(result, {'A'}, 0, -1, {0,1}, {0,1});
    //tools->go_to_clear(result, {'A'}, 0, -1, {0,1}, {1});
    tools->link_put(result, {'\u0000'}, {1});

    tools->go_to(result, {'\u0000'}, get_tuple_index()[1], 1, get_tuple_index());
    tools->link_put(result, {'E'}, {get_tuple_index()[0]});
    tools->move(result, get_tuple_index(), -1);
    tools->go_to_clear(result, {'A'}, get_tuple_index()[0], -1, get_tuple_index(), {get_tuple_index()[0]});
    tools->copy_to_working(result, get_tuple_index());

    result.to_state = looping;

    return result;
}

void TuringDenestify::createNewFunction(IncompleteSet &a) {

    IncompleteSet create_function{"create_function", "create_function"};
    tools->go_to(create_function, {'\u0000'}, get_tuple_index()[1], 1, get_tuple_index());
    tools->link_put(create_function, {'H'}, {get_tuple_index()[0]});
    tools->go_to(create_function, {'N'}, get_tuple_index()[0], -1, get_tuple_index());

    tools->write_function_header(create_function, get_tuple_index());

    //checks if function is part of a class
    //if part of class go to H marker tuple tapes
    //call own store function

    IncompleteSet check_object{"check_if_object", "check_if_object"};
    tools->go_to( check_object, {'C'}, get_tuple_index()[0], 1, get_tuple_index());
    tools->go_to( check_object, {'('}, get_tuple_index()[1], 1, get_tuple_index());
    tools->link_put(check_object, {'E'}, {get_tuple_index()[0]});
    tools->go_to( check_object, {'C'}, get_tuple_index()[0], -1, get_tuple_index());
    tools->link_put(check_object, {'S'}, {get_tuple_index()[0]});
    tools->copy_to_working(check_object, get_tuple_index());

    //clear 'S' and 'E' marker on tuple tapes
    // 'S will be done bit later'
    tools->go_to( check_object, {'E'}, get_tuple_index()[0], 1, get_tuple_index());
    tools->write_on(check_object, {'E'}, {get_tuple_index()[0]}, {'\u0000'}, {get_tuple_index()[0]});

    tools->go_to_multiple(check_object, {{'A', 'S'}, {':'}}, {0, 1}, -1, {0,1});

    tools->go_to( check_object, {'S'}, get_tuple_index()[0], -1, get_tuple_index());

    //changes token 'U' to 'O'
    IncompleteSet change_marker{"change_token_marker_to_object", "change_token_marker_to_object"};
    tools->link_put(change_marker, {'O'}, {get_tuple_index()[1]});
    tools->link_on(check_object, change_marker, {':'}, {1});
    //clear S
    tools->write_on(check_object, {'S'}, {get_tuple_index()[0]}, {'\u0000'}, {get_tuple_index()[0]});

    IncompleteSet call_store_obj{"call_store_obj", "call_store_obj"};

    tools->go_to(call_store_obj, {'A', 'S'}, 0, -1, {0, 1});
    tools->go_to(call_store_obj, {'H'}, get_tuple_index()[0], 1, get_tuple_index());

    storeObjectDefinition(call_store_obj);

    tools->link_on(check_object, call_store_obj, {':'}, {1});

    //go back to N marker
    tools->go_to( check_object, {'N'}, get_tuple_index()[0], -1, get_tuple_index());

    tools->link(create_function, check_object);


    //go back to N
    //clear workingtape if not empty
    tools->go_to(create_function, {'N'}, get_tuple_index()[0], -1, get_tuple_index());
    tools->go_to_multiple(create_function, {{'E'}, {'\u0000'}}, {0, 1}, 1, {0,1});
    tools->go_to_clear(create_function, {'S'}, 0, -1, {0,1}, {0,1});
    tools->link_put(create_function, {'\u0000'}, {1});

    //IncompleteSet breaker{"breaker", "breaker2"};
    //tools->link(create_function, breaker);


    //from here the data is moved to new function
    auto temp = get_tuple_index();
    temp.push_back(0);
    temp.push_back(1);

    for (int i=1; i<get_tuple_index().size(); i++){
        int index = get_tuple_index()[i];

        IncompleteSet copy_tuple{"copy_tuple_create_function_"+to_string(i), "copy_tuple_create_function_"+to_string(i)};

        //store on working
        tools->copy_till(copy_tuple, {'U'}, get_tuple_index()[0], index, 1, 1, temp);
        tools->copy_any(copy_tuple, index, 1);
        tools->move(copy_tuple, {0,1}, 1);
        tools->link_put(copy_tuple, {'E'}, {0});

        //set head tapes on right positions
        tools->go_to(copy_tuple, {'H'}, get_tuple_index()[0], 1, get_tuple_index());
        tools->go_to(copy_tuple, {'S'}, 0, -1, {0, 1});

        //store on tuples
        tools->copy_till(copy_tuple, {'E'}, 0, 1, index, 1, temp);
        tools->go_to_clear(copy_tuple, {'S'}, 0, -1, {0,1}, {0,1});
        tools->link_put(copy_tuple, {'\u0000'}, {1});
        tools->go_to(copy_tuple, {'N'}, get_tuple_index()[0], -1, get_tuple_index());
        tools->link(create_function, copy_tuple);

    }

    tools->go_to(create_function, {'H'}, get_tuple_index()[0], 1, get_tuple_index());
    tools->link_put(create_function, {'\u0000'}, {get_tuple_index()[0]});
    tools->go_to(create_function, {'\u0000'}, get_tuple_index()[1], 1, get_tuple_index());
    tools->link_put(create_function, {'H'}, {get_tuple_index()[0]});

    //clear current function and replace it by function call

    //create function call at the end(because size can be bigger than original size)
    //keep H on same position

    //for every tuple
    //copy after I on working
    //copy U-I on working
    //clear entire tuple_line from N-U
    //copy working after N

    //find original function
    tools->move(create_function, get_tuple_index(), -1);
    tools->skip_nesting(create_function, tapes-1, 1, get_tuple_index()[1], -1, get_tuple_index());

    //go to start of function definition
    tools->go_to(create_function, {'O', 'U'}, get_tuple_index()[1], -1, get_tuple_index());
    tools->write_on(create_function, {'\u0000'}, {get_tuple_index()[0]}, {'S'}, {get_tuple_index()[0]});
    tools->go_to(create_function, {'('}, get_tuple_index()[1], 1, get_tuple_index());
    tools->write_on(create_function, {'\u0000'}, {get_tuple_index()[0]}, {'E'}, {get_tuple_index()[0]});
    tools->go_to(create_function, {'S'}, get_tuple_index()[0], -1, get_tuple_index());
    tools->copy_to_working(create_function, get_tuple_index());

    //set a P token on space
    tools->go_to(create_function, {' '}, 1, -1, {0,1});
    tools->link_put(create_function, {'P'}, {0});
    tools->go_to(create_function, {'E'}, 0, 1, {0,1});
    tools->link_put(create_function, {'\u0000'}, {0});

    //go to start of stack variables
    tools->go_to(create_function, {stack_symbol, '{'}, tapes-1, -1, {(int) tapes-1});
    tools->go_to(create_function, {'.'}, tapes-1, 1, {(int) tapes-1});

    //put start bracket
    tools->link_put(create_function, {'('}, {1});
    tools->move(create_function, {0,1}, 1);


    IncompleteSet copy_loop{"copy_loop_create_function", "copy_loop_create_function"};
    tools->write_on(copy_loop, {'.'}, {(int) tapes-1}, {'\u0000'}, {(int) tapes-1});
    tools->move(copy_loop, {(int) tapes-1}, 1);

    tools->go_to_move(copy_loop, {'.', '\u0000'}, tapes-1, 1, {(int) tapes-1}, 1, 1, {0,1});

    string no_vars = tools->branch_on(copy_loop, {'\u0000'}, {(int)tapes-1});

    // store ', ' between string
    tools->link_put(copy_loop, {','}, {1});
    tools->move(copy_loop, {0,1}, 1);
    tools->link_put(copy_loop, {' '}, {1});
    tools->move(copy_loop, {0,1}, 1);
    tools->make_loop(copy_loop);
    copy_loop.to_state = no_vars;
    //b\put end bracket
    tools->link_put(copy_loop, {')'}, {1});
    tools->move(copy_loop, {0,1}, 1);

    tools->link_put(copy_loop, {';'}, {1});
    tools->move(copy_loop, {0,1}, 1);

    tools->link_put(copy_loop, {'E'}, {0});

    tools->link(create_function, copy_loop);

    //clear S and E marker on tuple tape
    tools->write_on(create_function, {'S'}, {get_tuple_index()[0]}, {'\u0000'}, {get_tuple_index()[0]});
    tools->go_to(create_function, {'E'}, get_tuple_index()[0], 1, get_tuple_index());
    tools->write_on(create_function, {'E'}, {get_tuple_index()[0]}, {'\u0000'}, {get_tuple_index()[0]});
    tools->go_to(create_function, {'H'}, get_tuple_index()[0], 1, get_tuple_index());

    //make a token of the working tape call
    tools->go_to(create_function, {'P'}, 0, -1, {0,1});
    tools->make_token(create_function, get_tuple_index(), 'A', 'I');
    tools->go_to(create_function, {'I'}, get_tuple_index()[0], -1, get_tuple_index());

    //clear working tape
    tools->go_to_clear(create_function, {'S'}, 0, -1, {0,1}, {0,1});
    tools->link_put(create_function, {'\u0000'}, {1});

    for (int i=1; i<get_tuple_index().size(); i++){
        int index = get_tuple_index()[i];

        IncompleteSet copy_tuple{"copy_tuple_create_function_2_"+to_string(i), "copy_tuple_create_function_2_"+to_string(i)};
        //copy from I-H on working tape
        tools->copy_till(copy_tuple, {'H'}, get_tuple_index()[0], index, 1, 1, temp);
        tools->copy_any(copy_tuple, index, 1);
        tools->go_to(copy_tuple, {'U'}, get_tuple_index()[0], -1, get_tuple_index());
        tools->copy_till(copy_tuple, {'I'}, get_tuple_index()[0], index, 1, 1, temp);

        tools->link_put(copy_tuple, {'E'}, {0});
        tools->go_to(copy_tuple, {'S'}, 0, -1, {0,1});

        tools->go_to(copy_tuple, {'N'}, get_tuple_index()[0], -1, get_tuple_index());
        tools->move(copy_tuple, get_tuple_index(), 1);
        tools->go_to_clear(copy_tuple, {'H'}, get_tuple_index()[0], 1, get_tuple_index(), {index});

        tools->go_to(copy_tuple, {'N'}, get_tuple_index()[0], -1, get_tuple_index());
        tools->move(copy_tuple, get_tuple_index(), 1);
        tools->copy_till(copy_tuple, {'E'}, 0, 1, index, 1, temp);

        //clear working
        tools->go_to_clear(copy_tuple, {'S'}, 0, -1, {0,1}, {0,1});
        tools->link_put(copy_tuple, {'\u0000'}, {1});

        //go to I for loop
        //go first to H because we can't guarantee direction I
        tools->go_to(create_function, {'H'}, get_tuple_index()[0], 1, get_tuple_index());
        tools->go_to(create_function, {'I'}, get_tuple_index()[0], -1, get_tuple_index());

        tools->link(create_function, copy_tuple);
    }

    tools->go_to(create_function, {'H'}, get_tuple_index()[0], 1, get_tuple_index());
    tools->link_put(create_function, {'\u0000'}, {get_tuple_index()[0]});
    tools->go_to(create_function, {'I'}, get_tuple_index()[0], -1, get_tuple_index());
    tools->link_put(create_function, {'\u0000'}, {get_tuple_index()[0]});
    tools->go_to(create_function, {'U'}, get_tuple_index()[0], -1, get_tuple_index());
    tools->link_put(create_function, {'\u0000'}, {get_tuple_index()[0]});
    tools->go_to_clear(create_function, {'A'}, get_tuple_index()[0], -1, get_tuple_index(), {get_tuple_index()[0]});

    tools->link(a, create_function);

}

void TuringDenestify::storeObjectDefinition(IncompleteSet &a) {
    //starts with position on marker H on tuple tapes
    //starts with position on marker S
    IncompleteSet store_object{"store_object_def", "store_object_def"};

    //set markers for compare single tape
    tools->go_to(store_object, {':'}, 1, 1, {0,1});
    tools->link_put(store_object, {'C'}, {0});

    tools->push(store_object, '.');
    tools->move(store_object, {0,1}, -1);
    tools->go_to_copy(store_object, {' '}, 1, -1, {0,1}, tapes-1, 1, {(int) tapes-1});

    tools->link_put(store_object, {'B'}, {0});

    tools->go_to(store_object, {'E'}, 0, 1, {0,1});
    tools->go_to(store_object, {'A'}, get_tuple_index()[0], -1, get_tuple_index());

    IncompleteSet store_object_loop{"store_object_def_loop", "store_object_def_loop"};
    tools->go_to(store_object_loop, {'C'}, get_tuple_index()[1], 1, get_tuple_index());
    tools->write_on(store_object_loop, {'\u0000'}, {get_tuple_index()[0]}, {'S'}, {get_tuple_index()[0]});
    tools->go_to_not(store_object_loop, {'C', 'E', 'P'}, get_tuple_index()[1], 1, get_tuple_index());
    tools->write_on(store_object_loop, {'\u0000'}, {get_tuple_index()[0]}, {'E'}, {get_tuple_index()[0]});
    tools->go_to(store_object_loop, {'A', 'S'}, get_tuple_index()[0], -1, get_tuple_index());
    tools->copy_to_working(store_object_loop, get_tuple_index());
    tools->link_put(store_object_loop, {'F'}, {0});
    tools->go_to(store_object_loop, {' '}, 1, -1, {0,1});
    tools->link_put(store_object_loop, {'D'}, {0});
    tools->go_to(store_object_loop, {'B'}, 0, -1, {0,1});

    tools->compare_single_tape(store_object_loop, 'B', 'C', 'D', 'F', 0, 1);

    string found = tools->branch_on(store_object_loop, {'C'}, {0});

    //set everything ready for loop
    tools->go_to(store_object_loop, {'C'}, 0, -1, {0,1});
    tools->move(store_object_loop, {0,1}, -1);
    tools->go_to(store_object_loop, {'.'}, tapes-1, -1, {(int) tapes-1});
    tools->move(store_object_loop, {(int) tapes-1}, 1);
    tools->go_to_copy(store_object_loop, {'\u0000'}, tapes-1, 1, {(int) tapes-1}, 1, -1, {0, 1});
    tools->link_put(store_object_loop, {' '}, {1});

    //clear working tape 2nd part
    tools->go_to(store_object_loop, {'F'}, 0, 1, {0,1});
    tools->go_to_clear(store_object_loop, {'E'}, 0, -1, {0,1}, {0,1});
    tools->link_put(store_object_loop, {'\u0000'}, {1});

    //clear markers on tuple tape
    tools->go_to(store_object_loop, {'A','S'}, get_tuple_index()[0], -1, get_tuple_index());
    tools->write_on(store_object_loop, {'S'}, {get_tuple_index()[0]}, {'\u0000'}, {get_tuple_index()[0]});
    tools->go_to(store_object_loop, {'E'}, get_tuple_index()[0], 1, get_tuple_index());
    tools->write_on(store_object_loop, {'E'}, {get_tuple_index()[0]}, {'\u0000'}, {get_tuple_index()[0]});

    tools->make_loop(store_object_loop);

    store_object_loop.to_state = found;

    //not in loop anymore from here
    tools->link(store_object, store_object_loop);

    //remove class from stack
    tools->go_to_clear(store_object, {'.'}, tapes-1, -1, {(int) tapes-1}, {(int) tapes-1});
    tools->link_put(store_object, {'\u0000'}, {(int) tapes-1});

    //clear working tape
    tools->move(store_object, {0,1}, 2);
    tools->push(store_object, '.');
    tools->copy_till(store_object, {'E'}, 0, 1, tapes-1, 1, {0, 1, (int) tapes-1});
    tools->go_to(store_object, {'F'}, 0, 1, {0,1});
    tools->go_to_clear(store_object, {'B'}, 0, -1, {0,1}, {0,1});
    tools->link_put(store_object, {'\u0000',' '}, {0, 1});
    tools->move(store_object, {0,1}, 1);
    tools->go_to(store_object, {'.'}, tapes-1, -1, {(int) tapes-1});
    tools->move(store_object, {(int) tapes-1}, 1);

    tools->go_to_move(store_object, {'\u0000'}, tapes-1, 1, {(int) tapes-1}, 1, -1, {0, 1});
    tools->go_to(store_object, {'.'}, tapes-1, -1, {(int) tapes-1});
    tools->link_put(store_object, {'\u0000'}, {(int) tapes-1});

    //store marker 'C' as start of insert
    tools->write_on(store_object, {'S'}, {get_tuple_index()[0]}, {'\u0000'}, {get_tuple_index()[0]});
    tools->go_to(store_object, {'E'}, get_tuple_index()[0], 1, get_tuple_index());
    tools->write_on(store_object, {'E'}, {get_tuple_index()[0]}, {'C'}, {get_tuple_index()[0]});

    //go to generated function to steal parameters
    tools->move(store_object, {0,1}, 2);
    tools->go_to(store_object, {'H'}, get_tuple_index()[0], 1, get_tuple_index());
    tools->go_to(store_object, {')'}, get_tuple_index()[1], -1, get_tuple_index());
    tools->link_put(store_object, {'E'}, {get_tuple_index()[0]});
    tools->skip_nesting(store_object, tapes-1, 1, get_tuple_index()[1], -1, get_tuple_index(), '(', ')');
    tools->link_put(store_object, {'S'}, {get_tuple_index()[0]});
    tools->copy_to_working(store_object, get_tuple_index());

    //clear tuple copy markers
    tools->go_to(store_object, {'E'}, get_tuple_index()[0], 1, get_tuple_index());
    tools->link_put(store_object, {'\u0000'}, {get_tuple_index()[0]});
    tools->go_to(store_object, {'A', 'S'}, get_tuple_index()[0], -1, get_tuple_index());
    tools->write_on(store_object, {'S'}, {get_tuple_index()[0]}, {'\u0000'}, {get_tuple_index()[0]});

    //make new token
    tools->go_to(store_object, {'H'}, get_tuple_index()[0], 1, get_tuple_index());
    tools->link_put(store_object, {'\u0000', ')'}, {0, 1});
    tools->move(store_object, {0,1}, 1);
    tools->link_put(store_object, {'\u0000', ';'}, {0, 1});
    tools->move(store_object, {0,1}, 1);
    tools->link_put(store_object, {'E'}, {0});
    tools->go_to(store_object, {'S'}, 0, -1, {0,1});
    tools->make_token(store_object, get_tuple_index(), 'A', 'I');

    //clear working tape till S
    tools->go_to_clear(store_object, {'S'}, 0, -1, {0,1}, {0,1});
    tools->link_put(store_object, {'\u0000'}, {1});

    //do the big copy move shit
    tools->go_to(store_object, {'I'}, get_tuple_index()[0], -1, get_tuple_index());

    auto temp = get_tuple_index();
    temp.push_back(0);
    temp.push_back(1);

    for (int i=1; i<get_tuple_index().size(); i++){
        int index = get_tuple_index()[i];

        IncompleteSet copy_tuple{"copy_tuple_object_function_2_"+to_string(i), "copy_tuple_object_function_2_"+to_string(i)};
        //copy from I-H on working tape
        tools->copy_till(copy_tuple, {'H'}, get_tuple_index()[0], index, 1, 1, temp);
        tools->copy_any(copy_tuple, index, 1);
        tools->go_to(copy_tuple, {'C'}, get_tuple_index()[0], -1, get_tuple_index());
        tools->move(copy_tuple, get_tuple_index(), 1);
        tools->copy_till(copy_tuple, {'I'}, get_tuple_index()[0], index, 1, 1, temp);

        tools->link_put(copy_tuple, {'E'}, {0});
        tools->go_to(copy_tuple, {'S'}, 0, -1, {0,1});

        tools->go_to(copy_tuple, {'C'}, get_tuple_index()[0], -1, get_tuple_index());
        tools->move(copy_tuple, get_tuple_index(), 1);
        tools->go_to_clear(copy_tuple, {'H'}, get_tuple_index()[0], 1, get_tuple_index(), {index});

        tools->go_to(copy_tuple, {'C'}, get_tuple_index()[0], -1, get_tuple_index());
        tools->move(copy_tuple, get_tuple_index(), 1);
        tools->copy_till(copy_tuple, {'E'}, 0, 1, index, 1, temp);

        //clear working
        tools->go_to_clear(copy_tuple, {'S'}, 0, -1, {0,1}, {0,1});
        tools->link_put(copy_tuple, {'\u0000'}, {1});

        //go to I for loop
        //go first to H because we can't guarantee direction I
        tools->go_to(copy_tuple, {'H'}, get_tuple_index()[0], 1, get_tuple_index());
        tools->go_to(copy_tuple, {'I'}, get_tuple_index()[0], -1, get_tuple_index());

        tools->link(store_object, copy_tuple);
    }

    //still move markers
    tools->copy_till(store_object, {'H'}, get_tuple_index()[0], get_tuple_index()[0], 1, 1, temp);
    tools->go_to(store_object, {'C'}, get_tuple_index()[0], -1, get_tuple_index());
    tools->move(store_object, get_tuple_index(), 1);
    tools->copy_till(store_object, {'I'}, get_tuple_index()[0], get_tuple_index()[0], 1, 1, temp);
    tools->go_to(store_object, {'C'}, get_tuple_index()[0], -1, get_tuple_index());
    tools->move(store_object, get_tuple_index(), 1);
    tools->go_to(store_object, {'S'}, 0, -1, {0,1});
    tools->copy_till(store_object, {'H'}, get_tuple_index()[0], 1, get_tuple_index()[0], 1, temp);

    //final clear working
    tools->go_to_clear(store_object, {'S'}, 0, -1, {0,1}, {0,1});
    tools->link_put(store_object, {'\u0000'}, {1});

    tools->go_to(store_object, {'H'}, get_tuple_index()[0], 1, get_tuple_index());
    tools->go_to(store_object, {'I'}, get_tuple_index()[0], -1, get_tuple_index());
    tools->link_put(store_object, {'\u0000'}, {get_tuple_index()[0]});
    tools->go_to(store_object, {'H'}, get_tuple_index()[0], 1, get_tuple_index());

    tools->link(a, store_object);
}
