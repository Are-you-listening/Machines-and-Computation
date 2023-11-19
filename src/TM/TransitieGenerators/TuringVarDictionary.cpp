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

    check_defined(result);
    //tools->heap_push_definer(result, get_tuple_index());



    tools->make_loop(result);
    //result.to_state = branch;
    return result;
}

void TuringVarDictionary::check_defined_old(IncompleteSet &a) {
    //precondition dont start on seperator
    //goes marking an part of code that we can store on heap
    IncompleteSet added("vardict_check_defined", "vardict_check_defined");
    vector<int> tuple_index = get_tuple_index();
    tools->write_on(added, {'\u0000'}, {tuple_index[0]}, {'S'}, {tuple_index[0]});
    tools->write_on(added, {'E'}, {tuple_index[0]}, {'S'}, {tuple_index[0]});

    //go to next so we don't check the start

    tools->move(added, tuple_index, 1);
    tools->go_to(added, {'S', '{', '\u0000'}, tuple_index[1], 1, tuple_index);

    tools->write_on(added, {'{'}, {tuple_index[1]}, {'E'}, {tuple_index[0]});
    tools->write_on(added, {'\u0000'}, {tuple_index[1]}, {'E'}, {tuple_index[0]});

    tools->make_loop_on(added, 'S', tuple_index[1]);

    tools->go_to(added, {'S', 'A'}, tuple_index[0], -1, tuple_index);

    tools->link(a, added);

}

void TuringVarDictionary::check_defined(IncompleteSet &a) {
    IncompleteSet added("vardict_check_defined", "vardict_check_defined");
    vector<int> tuple_index = get_tuple_index();
    tools->write_on(added, {'\u0000'}, {tuple_index[0]}, {'S'}, {tuple_index[0]});
    tools->write_on(added, {'E'}, {tuple_index[0]}, {'S'}, {tuple_index[0]});

    tools->move(added, tuple_index, 1);
    tools->go_to(added, {'S', '{', '\u0000'}, tuple_index[1], 1, tuple_index);

    tools->write_on(added, {'{'}, {tuple_index[1]}, {'E'}, {tuple_index[0]});
    tools->write_on(added, {'\u0000'}, {tuple_index[1]}, {'E'}, {tuple_index[0]});

    IncompleteSet store_on_heap("vardict_store_on_heap", "vardict_store_on_heap");
    tools->go_to(store_on_heap, {'S', 'A'}, tuple_index[0], -1, tuple_index);
    tools->heap_push_definer(store_on_heap, get_tuple_index());

    tools->link_on(added, store_on_heap, {'E'}, {tuple_index[0]});

    tools->link_on_multiple(a, added, {{'U'}, {'C'}, {'O'}}, {tuple_index[1]});
}
