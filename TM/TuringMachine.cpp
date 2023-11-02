//
// Created by tibov on 02/11/23.
//

#include "TuringMachine.h"

TuringMachine::TuringMachine(const string &path) {
    load(path);

}

void TuringMachine::load(const string &path) {
    ifstream f(path);
    json data = json::parse(f);

    for (int i = 0; i<data["States"].size(); i++){
        states.insert(data["States"][i].get<std::string>());
    }

    start_state = data["Start"].get<std::string>();
    current_state = start_state;


    unsigned int tape_amount = data["Tapes"].get<int>();
    for (int i=0; i<tape_amount; i++){
        tapes.push_back(new Tape{64});
    }

    string input = data["Input"].get<std::string>();
    tapes[0]->load(input);

    for (int i = 0; i<data["Productions"].size(); i++){
        json production = data["Productions"][i];
        string state = production["state"];
        queue<char> symbols;
        for (int j = 0; j<production["symbols"].size(); j++){
            symbols.push(production["symbols"][j].get<string>()[0]);
        }

        json to = production["to"];
        Production p;
        p.new_state = to["state"];

        for (int j = 0; j<to["symbols"].size(); j++){
            p.replace_val.push_back(to["symbols"][j].get<string>()[0]);
        }

        for (int j = 0; j<to["moves"].size(); j++){
            p.movement.push_back(to["moves"][j].get<int>());
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
        //cout << "halted" << endl;
        return;
    }

    Production p = loc->second->getProduction(symbols);

    if (p.new_state == ""){
        //cout << "halted" << endl;
        return;
    }

    current_state = p.new_state;
    for (int i=0; i<tapes.size(); i++){
        Tape* t = tapes[i];
        t->write(p.replace_val[i]);
        t->moveHead(p.movement[i]);
    }
}
