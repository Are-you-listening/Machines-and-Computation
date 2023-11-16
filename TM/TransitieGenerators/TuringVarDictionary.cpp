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
    tools->heap_push_function(result, get_tuple_index());

    return result;
}
