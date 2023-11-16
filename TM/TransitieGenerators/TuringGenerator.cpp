//
// Created by tibov on 16/11/23.
//

#include "TuringGenerator.h"

vector<int> TuringGenerator::get_tuple_index() {
    vector<int> tuple_set_indexes;
    for (int i= 0; i<tuple_size+2; i++){
        tuple_set_indexes.push_back(i+2);
    }
    return tuple_set_indexes;
}

TuringGenerator::TuringGenerator(int tuple_size): tuple_size{tuple_size} {
    tapes = tuple_size+5;
    tools = TuringTools::getInstance(tapes-1);
    seperators = {'=', ';', '{', '}', ')', '(', '\u0000'};
    special_sep = {'{', '}'};

}
