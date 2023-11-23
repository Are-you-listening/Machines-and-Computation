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

    //tools->mark_definer(result, get_tuple_index());
    //tools->make_working_nesting(result, get_tuple_index());
    //tools->add_nesting_working(result);

    return result;
}
