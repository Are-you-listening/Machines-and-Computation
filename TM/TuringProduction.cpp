//
// Created by tibov on 02/11/23.
//

#include "TuringProduction.h"

TuringProduction::TuringProduction() {

}

void TuringProduction::addRoute(queue<char>& symbols, Production&& p) {
    if (symbols.empty()){
        production = p;
        return;
    }

    auto loc = ptr_vector.find(symbols.front());
    TuringProduction* next_node;
    if (loc == ptr_vector.end()){
        next_node = new TuringProduction{};
        ptr_vector.insert({symbols.front(), next_node});

    }else{
        next_node = loc->second;
    }

    symbols.pop();
    next_node->addRoute(symbols, std::move(p));

}

Production TuringProduction::getProduction(const string_view &symbols) {
    if (symbols.empty()){
        return production;
    }

    auto loc = ptr_vector.find(symbols[0]);
    if (loc != ptr_vector.end()){
        TuringProduction* tp = loc->second;
        return tp->getProduction(symbols.substr(1, symbols.size() - 1));
    }

    loc = ptr_vector.find('\0');
    if (loc == ptr_vector.end()){
        throw invalid_argument("no paths");
    }
    TuringProduction* tp = loc->second;
    return tp->getProduction(symbols.substr(1, symbols.size() - 1));
}
