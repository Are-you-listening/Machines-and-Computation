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
    tools->go_to(create_function, {'U'}, get_tuple_index()[0], -1, get_tuple_index());

    tools->link(a, create_function);
}
