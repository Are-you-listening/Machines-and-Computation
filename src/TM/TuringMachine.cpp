//
// Created by tibov on 02/11/23.
//

#include "TuringMachine.h"

TuringMachine::TuringMachine(const string &path) {
    load(path);
}

void TuringMachine::load(json &data) {
    halted = false;
    for (const auto & i : data["States"]){
        states.insert(i.get<std::string>());
    }

    start_state = data["Start"].get<std::string>();
    current_state = start_state;

    unsigned int tape_amount = data["Tapes"].get<int>();
    for (int i=0; i<tape_amount; i++){
        tapes.push_back(new Tape{64});
    }

    auto input = data["Input"].get<std::string>();
    load_input(input, 0);

    for (int i = 0; i<data["Productions"].size(); i++){
        json production = data["Productions"][i];
        string state = production["state"];
        queue<char> symbols;
        for (const auto & j : production["symbols"]){
            symbols.push(j.get<string>()[0]);
        }

        json to = production["to"];
        Production p;
        p.new_state = to["state"];

        for (const auto & j : to["symbols"]){
            p.replace_val.push_back(j.get<string>()[0]);
        }

        for (const auto & j : to["moves"]){
            p.movement.push_back(j.get<int>());
        }

        auto loc = production_trees.find(state);

        TuringProduction* tp;
        if (loc != production_trees.end()){
            tp = loc->second;
        }else{
            tp = new TuringProduction{};
            production_trees.insert({state, tp});
        }

        tp->addRoute(symbols, std::move(p));
    }
}

void TuringMachine::load(const vector<string> &states, const string &start_state, const string &input, int tape_size,
                         const vector<Transition> &productions) {
    halted = false;
    for (const string& i : states){
        this->states.insert(i);
    }

    this->start_state = start_state;
    current_state = start_state;

    unsigned int tape_amount = tape_size;
    for (int i=0; i<tape_amount; i++){
        tapes.push_back(new Tape{64});
    }

    load_input(input, 0);

    for (int i = 0; i<productions.size(); i++){
        Transition production = productions[i];
        string state = production.state;
        queue<char> symbols;
        for (const auto & j : production.input){
            symbols.push(j);
        }

        //json to = production["to"];
        Production p = production.production;
        /*
        p.new_state = to["state"];

        for (const auto & j : to["symbols"]){
            p.replace_val.push_back(j.get<string>()[0]);
        }

        for (const auto & j : to["moves"]){
            p.movement.push_back(j.get<int>());
        }*/

        auto loc = production_trees.find(state);

        TuringProduction* tp;
        if (loc != production_trees.end()){
            tp = loc->second;
        }else{
            tp = new TuringProduction{};
            production_trees.insert({state, tp});
        }

        tp->addRoute(symbols, std::move(p));
    }

}



void TuringMachine::load(const string &path) {
    ifstream f(path);
    json data = json::parse(f);

    load(data);
}

string TuringMachine::getTapeData(unsigned int index) const {
    return tapes[index]->getTapeData();
}

void TuringMachine::move(){
    queue<char> symbols;
    for (Tape* t: tapes){
        symbols.push(t->getSymbol());
    }
    auto loc = production_trees.find(current_state);
    if (loc == production_trees.end()){
        halted = true;
        return;
    }

    Production p = loc->second->getProduction(symbols);

    if (p.new_state.empty()){
        halted = true;
        return;
    }

    current_state = p.new_state;
    for (int i=0; i<tapes.size(); i++){
        Tape* t = tapes[i];
        t->write(p.replace_val[i]);
        t->moveHead(p.movement[i]);
    }
}

bool TuringMachine::isHalted() const {
    return halted;
}

unsigned int TuringMachine::getTapeAmount() const {
    return tapes.size();
}

TuringMachine::~TuringMachine() {
    for (auto tape: tapes){
        delete tape;
    }

    for (auto [k, tree]: production_trees){
        delete tree;
    }
}

void TuringMachine::load_input(const string &input, int index) {
    tapes[index]->load(input);
}

const string &TuringMachine::getCurrentState() const {
    return current_state;
}

