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
    tools->go_to(result, {'C', 'U', 'O', '\u0000'}, get_tuple_index()[1], 1, {get_tuple_index()});
    string branch = tools->branch_on(result, {'\u0000'}, {get_tuple_index()[1]});

    IncompleteSet store_call{"vardict_store_call",  "vardict_store_call"};
    check_defined(store_call);
    tools->link_on_multiple(result, store_call, {{'C'}, {'U'}}, {get_tuple_index()[1]});

    //temp just prevents inf loop if token is 'O' (as a placeholder)
    IncompleteSet temp{"vardict_temp",  "vardict_temp"};
    tools->move(temp, get_tuple_index(), 1);

    tools->link_on_multiple(result, temp, {{'O'}}, {get_tuple_index()[1]});


    tools->make_loop(result);
    //result.to_state = branch;
    return result;
}



void TuringVarDictionary::check_defined(IncompleteSet &a) {
    IncompleteSet added("vardict_check_defined", "vardict_check_defined");
    vector<int> tuple_index = get_tuple_index();
    tools->write_on(added, {'\u0000'}, {tuple_index[0]}, {'S'}, {tuple_index[0]});
    tools->write_on(added, {'E'}, {tuple_index[0]}, {'S'}, {tuple_index[0]});

    tools->move(added, tuple_index, 1);
    tools->go_to(added, {'S', '{', '\u0000'}, tuple_index[1], 1, tuple_index);

    tools->write_on(added, {'{'}, {tuple_index[1]}, {'E'}, {tuple_index[0]});

    IncompleteSet store_on_heap("vardict_store_on_heap", "vardict_store_on_heap");
    tools->go_to(store_on_heap, {'S', 'A'}, tuple_index[0], -1, tuple_index);
    tools->heap_push_definer(store_on_heap, get_tuple_index());

    //store call func/class on working tape
    tools->copy_to_working(store_on_heap, get_tuple_index());
    tools->move(store_on_heap, {0,1}, 1);
    tools->link_put(store_on_heap, {'S'}, {0});

    tools->go_to(store_on_heap, {'E'}, tuple_index[0], 1, tuple_index);

    tools->link_on(added, store_on_heap, {'E'}, {tuple_index[0]});

    tools->link_on_multiple(a, added, {{'U'}, {'C'}}, {tuple_index[1]});
}
