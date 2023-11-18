//
// Created by tibov on 16/11/23.
//

#include "TuringVarDictionary.h"

TuringVarDictionary::TuringVarDictionary(): TuringGenerator(4) {

}

IncompleteSet TuringVarDictionary::getTransitions() {
    return storeVar();
}

IncompleteSet TuringVarDictionary::storeVar() {
    IncompleteSet result("store_var_start", "store_var_start");
    //tools->heap_push_function(result, get_tuple_index());
    check_defined(result);
    tools->heap_push_definer(result, get_tuple_index());
    return result;
}

void TuringVarDictionary::check_defined(IncompleteSet &a) {
    //precondition dont start on seperator
    IncompleteSet added("vardict_check_defined", "vardict_check_defined");
    vector<int> tuple_index = get_tuple_index();
    tools->write_on(added, {'\u0000'}, {tuple_index[0]}, {'S'}, {tuple_index[0]});
    tools->write_on(added, {'E'}, {tuple_index[0]}, {'S'}, {tuple_index[0]});

    //go to next so we don't check the start

    tools->move(added, tuple_index, 1);
    tools->go_to(added, {'S', '{'}, tuple_index[1], 1, tuple_index);

    tools->write_on(added, {'{'}, {tuple_index[1]}, {'E'}, {tuple_index[0]});

    tools->make_loop_on(added, 'S', tuple_index[1]);

    tools->go_to(added, {'S', 'A'}, tuple_index[0], -1, tuple_index);

    tools->link(a, added);

}
