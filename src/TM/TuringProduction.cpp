//
// Created by tibov on 02/11/23.
//

#include "TuringProduction.h"
#include "TuringTools.h"
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

Production TuringProduction::getProduction(queue<char>& symbols) {
    if (symbols.empty()){
        return production;
    }

    auto loc = ptr_vector.find(symbols.front());
    if (loc != ptr_vector.end()){
        TuringProduction* tp = loc->second;
        symbols.pop();
        return tp->getProduction(symbols);
    }

    loc = ptr_vector.find('\u0001');
    if (loc == ptr_vector.end()){
        return production;
    }
    TuringProduction* tp = loc->second;
    symbols.pop();
    return tp->getProduction(symbols);
}

TuringProduction::~TuringProduction() {
    for (auto [k, v]: ptr_vector){
        delete v;
    }
}

vector<Transition> TuringProduction::traverse() {
    if (ptr_vector.empty()){
        Transition t;
        t.production = production;
        return {t};
    }

    vector<Transition> out;
    for (auto [k, v]: ptr_vector){
        auto t = v->traverse();
        for (auto u: t){
            u.input.insert(u.input.begin(), k);
            out.push_back(u);
        }

    }
    return out;
}
