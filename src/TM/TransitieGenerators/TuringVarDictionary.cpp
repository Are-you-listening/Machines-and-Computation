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
    tools->go_to(result, {'C', 'U', 'O', '\u0000', '}', '{'}, get_tuple_index()[1], 1, {get_tuple_index()});
    string branch = tools->branch_on(result, {'\u0000'}, {get_tuple_index()[1]});

    IncompleteSet store_call{"vardict_store_call",  "vardict_store_call"};
    check_defined(store_call);
    tools->link_on_multiple(result, store_call, {{'C'}, {'U'}}, {get_tuple_index()[1]});

    //temp just prevents inf loop if token is 'O' (as a placeholder)
    IncompleteSet temp{"vardict_temp",  "vardict_temp"};
    tools->move(temp, get_tuple_index(), 1);

    tools->link_on_multiple(result, temp, {{'O'}}, {get_tuple_index()[1]});

    IncompleteSet remove_nest{"vardict_remove_nesting", "vardict_remove_nesting"};
    remove_nesting(remove_nest);
    tools->move(remove_nest, get_tuple_index(), 1);
    tools->link_on(result, remove_nest, {'}'}, {get_tuple_index()[1]});

    IncompleteSet add_nest{"vardict_add_nesting", "vardict_add_nesting"};
    tools->link_put(add_nest, {'{'}, {1});
    tools->write_on(add_nest, {'S'}, {0}, {'\u0000'}, {0});
    tools->move(add_nest, {0,1}, 1);
    tools->link_put(add_nest, {'S'}, {0});

    tools->move(add_nest, get_tuple_index(), 1);
    tools->link_on(result, add_nest, {'{'}, {get_tuple_index()[1]});

    tools->make_loop(result);
    result.to_state = branch;

    //remove_nesting(result);

    return result;
}

void TuringVarDictionary::check_defined(IncompleteSet &a) {
    IncompleteSet added("vardict_check_defined", "vardict_check_defined");
    vector<int> tuple_index = get_tuple_index();

    //but S at the start of tuple tapes
    tools->write_on(added, {'\u0000'}, {tuple_index[0]}, {'S'}, {tuple_index[0]});
    tools->write_on(added, {'E'}, {tuple_index[0]}, {'S'}, {tuple_index[0]});

    //go to seperator or '{'
    tools->move(added, tuple_index, 1);
    tools->go_to(added, {'S', '{', '\u0000'}, tuple_index[1], 1, tuple_index);

    //set end marker
    tools->write_on(added, {'{'}, {tuple_index[1]}, {'E'}, {tuple_index[0]});

    IncompleteSet store_on_heap("vardict_store_on_heap", "vardict_store_on_heap");
    tools->go_to(store_on_heap, {'S', 'A'}, tuple_index[0], -1, tuple_index);
    tools->heap_push_definer(store_on_heap, get_tuple_index());


    tools->go_to(store_on_heap, {'E'}, tuple_index[0], 1, tuple_index);

    tools->link_on(added, store_on_heap, {'E'}, {tuple_index[0]});

    tools->link_on_multiple(a, added, {{'U'}, {'C'}}, {tuple_index[1]});
}

void TuringVarDictionary::remove_nesting(IncompleteSet &a) {
    tools->go_to_clear(a, {'{'}, 1, -1, {0,1}, {0,1});
    tools->link_put(a, {'\u0000'}, {1});

    tools->go_to_multiple(a, {{'A'}, {'{'}}, {0,1}, -1, {0,1});

    tools->move(a, {0,1}, 1);
    tools->go_to_clear(a, {'\u0000'}, 1, 1, {0,1}, {0,1});
    tools->go_to_multiple(a, {{'A'}, {'{'}}, {0,1}, -1, {0,1});

    IncompleteSet remove_first_char{"remove_first_char", "remove_first_char"};
    tools->link_put(remove_first_char, {'\u0000'}, {1});

    tools->link_on(a, remove_first_char, {'A'}, {0});

    IncompleteSet move_right{"remove_nesting_move_right", "remove_nesting_move_right"};
    tools->move(move_right, {0,1}, 1);

    tools->link_on(a, move_right, {'{'}, {1});

    tools->write_on(a, {'\u0000'}, {0}, {'S'}, {0});
}
