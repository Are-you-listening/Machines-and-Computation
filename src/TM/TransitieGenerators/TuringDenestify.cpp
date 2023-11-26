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

    //tools->go_to(result, {'\u0000'}, get_tuple_index()[1], 1, get_tuple_index());
    //tools->link_put(result, {'E'}, {get_tuple_index()[0]});
    //tools->go_to(result, {'A'}, get_tuple_index()[0], -1, get_tuple_index());
    //tools->copy_to_working(result, get_tuple_index());

    return result;
}

void TuringDenestify::createNewFunction(IncompleteSet &a) {
    IncompleteSet create_function{"create_function", "create_function"};
    tools->go_to(create_function, {'\u0000'}, get_tuple_index()[1], 1, get_tuple_index());
    tools->link_put(create_function, {'H'}, {get_tuple_index()[0]});
    tools->go_to(create_function, {'N'}, get_tuple_index()[0], -1, get_tuple_index());

    tools->write_function_header(create_function, get_tuple_index());

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
    tools->go_to(create_function, {'*', '{'}, tapes-1, -1, {(int) tapes-1});
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
