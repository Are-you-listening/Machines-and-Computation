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
    //TODO: functions with paramerters supporter in heap storage
    IncompleteSet result("store_var_start", "store_var_start");
    //tools->heap_push_function(result, get_tuple_index());
    tools->go_to(result, {'C', 'U', 'O', '\u0000', '}', '{', 'O', 'D', ')', '('}, get_tuple_index()[1], 1, {get_tuple_index()});
    string branch = tools->branch_on(result, {'\u0000'}, {get_tuple_index()[1]});

    IncompleteSet store_call{"vardict_store_call",  "vardict_store_call"};
    check_defined(store_call);
    tools->link_on_multiple(result, store_call, {{'C'}, {'U'}, {'O'}}, {get_tuple_index()[1]});

    IncompleteSet store_call_2{"vardict_store_call_2",  "vardict_store_call_2"};
    store_defined(store_call_2);
    tools->link_on_multiple(result, store_call_2, {{'D'}}, {get_tuple_index()[1]});

    IncompleteSet remove_nest{"vardict_remove_nesting", "vardict_remove_nesting"};
    tools->remove_nesting_working(remove_nest);
    tools->move(remove_nest, get_tuple_index(), 1);
    tools->link_on(result, remove_nest, {'}'}, {get_tuple_index()[1]});

    IncompleteSet add_nest{"vardict_add_nesting", "vardict_add_nesting"};

    //make sure 2 following if statements have their own key
    IncompleteSet create_key{"vardict_create_key", "vardict_create_key"};
    tools->move(create_key, {0,1}, 1);

    tools->push(create_key, 'N');
    tools->push(create_key, '{');

    tools->move(create_key, {(int) tapes-1}, -2);
    tools->go_to(create_key, {stack_symbol, '{', 'O'}, tapes-1, -1, {(int) tapes-1});
    tools->move(create_key, {(int) tapes-1}, 1);
    tools->go_to_copy(create_key, {'{'}, tapes-1, 1, {(int) tapes-1}, 1, 1, {0, 1});
    tools->link_put(create_key, {'P', ' '}, {0, 1});
    tools->move(create_key, {0,1}, 1);

    tools->move(create_key, {(int) tapes-1}, -2);
    tools->go_to(create_key, {stack_symbol, '{', 'O'}, tapes-1, -1, {(int) tapes-1});
    tools->move(create_key, {(int) tapes-1}, 1);
    tools->go_to_copy(create_key, {'{'}, tapes-1, 1, {(int) tapes-1}, 1, 1, {0, 1});

    tools->link_put(create_key, {'E'}, {0});


    tools->go_to(create_key, {'A', 'S'}, 0, -1, {0,1});
    tools->set_heap_mode(create_key, true);
    tools->heap_push_working(create_key, true);
    tools->move(create_key, {0,1}, -1);
    //string breaker = tools->branch_on(create_key, {'\u0000'}, {(int) tapes-1});

    //check if already curly bracket present
    tools->move(add_nest, {0,1}, -1);
    tools->link_on(add_nest, create_key, {'{'}, {1});
    tools->move(add_nest, {0,1}, 1);

    tools->link_put(add_nest, {'{'}, {1});
    tools->write_on(add_nest, {'S'}, {0}, {'\u0000'}, {0});
    tools->move(add_nest, {0,1}, 1);
    tools->link_put(add_nest, {'S'}, {0});

    //check for bracket stuff on tuple tapes
    //all between () before this nesting needs to be defined in this nesting
    tools->move(add_nest, get_tuple_index(), -1);
    tools->go_to_not(add_nest, {'P', 'E'}, get_tuple_index()[1], -1, get_tuple_index());

    //'continue in bracket' will go never go directly back, but will loop to the start
    IncompleteSet continue_in_bracket{"continue_in_bracket", "continue_in_bracket"};
    tools->skip_nesting(continue_in_bracket, tapes-1, 1, get_tuple_index()[1], -1, get_tuple_index(), '(', ')');
    tools->move(continue_in_bracket, get_tuple_index(), 1);
    tools->link(continue_in_bracket, result);

    //IncompleteSet breaker{"breaker", "breaker2"};
    //tools->link(continue_in_bracket, breaker);

    tools->link_on(add_nest, continue_in_bracket, {')'}, {get_tuple_index()[1]});

    tools->move(add_nest, get_tuple_index(), 1);
    tools->go_to_not(add_nest, {'P', 'E'}, get_tuple_index()[1], 1, get_tuple_index());
    tools->move(add_nest, get_tuple_index(), 1);

    tools->link_on(result, add_nest, {'{'}, {get_tuple_index()[1]});

    IncompleteSet skip_bracket{"skip_bracket", "skip_bracket"};
    tools->skip_nesting(skip_bracket, tapes-1, 1, get_tuple_index()[1], 1, get_tuple_index(), '(', ')');
    tools->move(skip_bracket, get_tuple_index(), 1);
    tools->link_on(result, skip_bracket, {'('}, {get_tuple_index()[1]});

    IncompleteSet skip_nesting{"skip_bracket_nesting", "skip_bracket_nesting"};
    tools->go_to(skip_nesting, {'{'}, get_tuple_index()[1], 1, get_tuple_index());
    tools->move(skip_nesting, get_tuple_index(), 1);
    tools->link_on(result, skip_nesting, {')'}, {get_tuple_index()[1]});

    tools->make_loop(result);
    result.to_state = branch;

    //cleanup
    tools->go_to_clear(result, {'A'}, get_tuple_index()[0], -1, get_tuple_index(), {get_tuple_index()[0]});
    tools->clear_stack(result);

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

    IncompleteSet store_bracket{"vardict_store_on_heap_bracket", "vardict_store_on_heap_bracket"};
    tools->heap_push_definer(store_bracket, get_tuple_index(), true, true);

    tools->link_on_multiple(store_on_heap, store_bracket, {{'U'}, {'O'}}, {tuple_index[1]});

    IncompleteSet store_no_bracket{"vardict_store_on_heap_no_bracket", "vardict_store_on_heap_no_bracket"};
    tools->heap_push_definer(store_no_bracket, get_tuple_index(), true, false);

    tools->link_on_multiple(store_on_heap, store_no_bracket, {{'C'}}, {tuple_index[1]});


    tools->go_to(store_on_heap, {'E'}, tuple_index[0], 1, tuple_index);

    tools->link_on(added, store_on_heap, {'E'}, {tuple_index[0]});

    tools->link_on_multiple(a, added, {{'U'}, {'C'}, {'O'}}, {tuple_index[1]});
}

void TuringVarDictionary::store_defined(IncompleteSet &a) {
    IncompleteSet added("vardict_store_defined", "vardict_store_defined");
    vector<int> tuple_index = get_tuple_index();

    tools->write_on(added, {'\u0000'}, {tuple_index[0]}, {'S'}, {tuple_index[0]});
    tools->write_on(added, {'E'}, {tuple_index[0]}, {'S'}, {tuple_index[0]});

    tools->move(added, tuple_index, 1);
    tools->go_to(added, {'S', '\u0000', ')', '('}, tuple_index[1], 1, tuple_index);

    tools->write_on(added, {'S'}, {tuple_index[1]}, {'E'}, {tuple_index[0]});
    tools->write_on(added, {'('}, {tuple_index[1]}, {'E'}, {tuple_index[0]});
    tools->write_on(added, {')'}, {tuple_index[1]}, {'E'}, {tuple_index[0]});

    IncompleteSet store_on_heap("vardict_store_data_on_heap", "vardict_store_data_on_heap");
    tools->go_to(store_on_heap, {'S', 'A'}, tuple_index[0], -1, tuple_index);
    tools->heap_push_definer(store_on_heap, get_tuple_index(), false, false);


    tools->go_to(store_on_heap, {'E'}, tuple_index[0], 1, tuple_index);

    tools->link_on(added, store_on_heap, {'E'}, {tuple_index[0]});

    tools->link_on(added, store_on_heap, {'E'}, {tuple_index[0]});

    tools->link_on_multiple(a, added, {{'D'}}, {tuple_index[1]});

}



