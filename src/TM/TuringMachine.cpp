//
// Created by tibov on 02/11/23.
//

#include "TuringMachine.h"

TuringMachine::TuringMachine(const string &path) {
    storage_in_state_size = 0;
    storage_in_state = (char*) calloc(0, 1);
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

    for (int i=0; i< storage_in_state_size; i++){
        symbols.push(storage_in_state[i]);
    }

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

    for (int i=0; i<storage_in_state_size; i++){
        storage_in_state[i] = p.replace_val[i];
    }

    for (int i=storage_in_state_size; i<tapes.size(); i++){
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

    free(storage_in_state);
}

void TuringMachine::load_input(const string &input, int index) {
    tapes[index]->load(input);
}

const string &TuringMachine::getCurrentState() const {
    return current_state;
}

string TuringMachine::exportTapeData(unsigned int index) const {
    return tapes[index]->exportTape();
}

int TuringMachine::getTuringIndex(int index) {
    return tapes[index]->getTapeHeadIndex();
}

void TuringMachine::clear() {
    for (auto tape: tapes){
        tape->clear();
    }
    current_state = start_state;
    halted = false;
}

TuringMachine TuringMachine::toSingleTape() {
    TuringMachine output_tm;

    output_tm.makeStorage(tapes.size()+1);




    vector<char> kleene_starts;
    vector<int> moving_list;
    for (int i =0; i<tapes.size(); i++){
        kleene_starts.push_back('\u0001');
    }

    for (int i =0; i<tapes.size()*3+1; i++){
        moving_list.push_back(1);
    }

    vector<Transition> new_productions;

    auto prod = getProductions();
    for (auto p: prod){
        for (int i =0; i<tapes.size(); i++){
            Transition new_t;
            new_t.input.push_back((char) (i+48));
            new_t.input.insert(new_t.input.end(), kleene_starts.begin(), kleene_starts.end());
            for (auto u: p.input){
                new_t.input.push_back('\u0001');
                new_t.input.push_back('\u0001');
            }
            new_t.state = p.state;

            Production moving;
            moving.new_state = p.state;
            moving.movement = moving_list;
            moving.replace_val = new_t.input;

            new_t.production = moving;
            new_productions.push_back(new_t);

            new_t.production.replace_val[0] = (char) (i+1+48);
            for (int k =0; k<tapes.size(); k++){
                int index = k*2 + tapes.size()+1;
                new_t.input[index] = 'X';
                for (int j =31; j<128; j++){
                    char c = (char) j;
                    if (j == 127){
                        c = '\u0000';
                    }
                    if (j == 31){
                        c = '\n';
                    }

                    new_t.input[index+1] = c;
                    new_t.production.replace_val[k+1] = c;
                    new_productions.push_back(new_t);
                }
                new_t.input[index] = '\u0001';
                new_t.input[index+1] = '\u0001';
                new_t.production.replace_val[k+1] = '\u0001';

            }
        }
    }
    std::cout <<new_productions.size() << std::endl;

    //start Transition
    Transition start_t;
    Production start_p;
    start_t.state = start_state;
    for (int i =0; i<tapes.size()*3+1; i++){
        start_t.input.push_back('\u0001');
        if (i == 0){
            start_p.replace_val.push_back('0');
        }else{
            start_p.replace_val.push_back('\u0001');
        }
        start_p.movement.push_back(-1);

    }
    start_t.production = start_p;
    new_productions.push_back(start_t);
    output_tm.load({}, start_state, "", tapes.size()*2, new_productions);
    for (int i =0; i<tapes.size(); i++){
        string head;
        for (int j =0; j<tapes[i]->getTapeHeadIndex(); j++){
            head += " ";
        }

        head += "X";

        output_tm.load_input(head,i*2);
        output_tm.load_input(tapes[i]->exportTape(),i*2+1);
    }
    return output_tm;
}

void TuringMachine::makeStorage(int size) {
    free(storage_in_state);
    storage_in_state_size = size;
    storage_in_state = (char*) calloc(size, 1);

}

TuringMachine::TuringMachine() {
    storage_in_state_size = 0;
    storage_in_state = (char*) calloc(0, 1);

}

vector<Transition> TuringMachine::getProductions() {
    vector<Transition> out;
    for (auto [k, v]: production_trees){
        auto t = v->traverse();
        for (auto u: t){
            u.state = k;
            out.push_back(u);
        }
    }
    return out;
}

