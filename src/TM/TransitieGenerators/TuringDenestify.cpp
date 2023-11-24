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
    createNewFunction(result);

    return result;
}

void TuringDenestify::createNewFunction(IncompleteSet &a) {
    IncompleteSet create_function{"create_function", "create_function"};
    tools->go_to(create_function, {'\u0000'}, get_tuple_index()[1], 1, get_tuple_index());
    tools->link_put(create_function, {'H'}, {get_tuple_index()[0]});
    tools->go_to(create_function, {'N'}, get_tuple_index()[0], -1, get_tuple_index());

    tools->write_function_header(create_function, get_tuple_index());
    /*
    //from here the data is moved to new function
    auto temp = get_tuple_index();
    temp.push_back(0);
    temp.push_back(1);

    //store on working
    tools->copy_till(create_function, {'U'}, get_tuple_index()[0], get_tuple_index()[1], 1, 1, temp);
    tools->copy(create_function, get_tuple_index()[1], 1);
    tools->move(create_function, {0,1}, 1);
    tools->link_put(create_function, {'E'}, {0});

    //set head tapes on right positions
    tools->go_to(create_function, {'H'}, get_tuple_index()[0], 1, get_tuple_index());
    tools->go_to(create_function, {'S'}, 0, -1, {0, 1});

    //store on tuples
    tools->copy_till(create_function, {'E'}, 0, 1, get_tuple_index()[1], 1, temp);
    */
    tools->link(a, create_function);
}