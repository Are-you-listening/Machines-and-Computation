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
        char c = p.replace_val[i];
        if (c == '\u0001'){
            continue;
        }
        storage_in_state[i] = c;
    }

    for (int u=0; u<p.control_increase.size(); u++){
        storage_in_state[p.control_increase[u]] += p.increase_amount[u];
    }


    for (int i=0; i<tapes.size(); i++){
        Tape* t = tapes[i];
        t->write(p.replace_val[i+storage_in_state_size]);
        t->moveHead(p.movement[i+storage_in_state_size]);
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
    int new_control = tapes.size()+1;

    auto tools = TuringTools::getInstance(-1);

    vector<int> storage_in_state_indexes;
    vector<int> all_moving;
    for (int i =0; i<new_control; i++){
        storage_in_state_indexes.push_back(i);
        all_moving.push_back(1);
    }


    IncompleteSet new_transitions{"new_transitions", "new_transitions"};
    int counter = 0;
    set<string> start_states;
    for (auto prod: getProductions()){
        if (start_states.find(prod.state) == start_states.end()){
            start_states.insert(prod.state);

            cout << counter << endl;
            //later skip store part for same state
            IncompleteTransition loop_state;
            loop_state.state = prod.state;
            loop_state.to_state = prod.state;
            loop_state.def_move = 1;
            new_transitions.transitions.push_back(loop_state);
        }

        set<IncompleteTransition> soon_merging;
        for (int i =0; i<tapes.size(); i++){
            IncompleteTransition store;
            store.state = prod.state;
            store.to_state = prod.state;
            store.def_move = 1;

            store.input = {'X', prod.input[i]};
            store.input_index = {i*2+new_control, i*2+new_control+1};

            store.output = {prod.input[i]};
            store.output_index = {i+1};
            store.move = {1};

            store.control_increase = {0};
            store.increase_amount = {1};
            soon_merging.insert(store);

        }

        auto all_store_options = tools->mergeToSingle(soon_merging);
        new_transitions.transitions.insert(new_transitions.transitions.begin(), all_store_options.begin(), all_store_options.end());


        IncompleteTransition toWriteMode;
        toWriteMode.state = prod.state;

        toWriteMode.input = {(char) ('\u0002'+(int) tapes.size())};
        toWriteMode.input.insert(toWriteMode.input.end(), prod.input.begin(), prod.input.end());
        toWriteMode.input_index = storage_in_state_indexes;

        toWriteMode.output = {(char) ('\u0002'+(int) tapes.size())};
        toWriteMode.output.insert(toWriteMode.output.end(), prod.production.replace_val.begin(), prod.production.replace_val.end());
        toWriteMode.output_index = storage_in_state_indexes;
        toWriteMode.move = all_moving;

        toWriteMode.to_state = prod.state+"_write"+ to_string(counter);
        toWriteMode.def_move = 1;

        new_transitions.transitions.push_back(toWriteMode);


        IncompleteTransition loop_state2;
        loop_state2.state = prod.state+"_write"+ to_string(counter);
        loop_state2.to_state = prod.state+"_write"+ to_string(counter);

        loop_state2.input = prod.production.replace_val;
        loop_state2.input_index = storage_in_state_indexes;
        loop_state2.input_index.erase(loop_state2.input_index.begin());

        loop_state2.def_move = -1;
        new_transitions.transitions.push_back(loop_state2);


        IncompleteTransition loop_state3;
        loop_state3.state = prod.state+"_write"+ to_string(counter);
        loop_state3.to_state = prod.state+"_write"+ to_string(counter);
        loop_state3.def_move = -1;
        new_transitions.transitions.push_back(loop_state3);


        soon_merging = {};
        for (int i =0; i<tapes.size(); i++){
            IncompleteTransition write;
            write.state = prod.state+"_write"+ to_string(counter);
            write.to_state = prod.state+"_write"+ to_string(counter);
            write.def_move = 0;

            write.input = {prod.production.replace_val[i], 'X'};
            write.input_index = {i+1, i*2+new_control};

            write.output = {'\u0003', prod.production.replace_val[i]};
            write.output_index = {i+1, i*2+1+new_control};
            write.move = {0, 0};

            write.control_increase = {0};
            write.increase_amount = {-1};
            soon_merging.insert(write);

            IncompleteTransition move_marker;
            move_marker.state = prod.state+"_write"+ to_string(counter);
            move_marker.to_state = prod.state+"_mark"+ to_string(counter);
            move_marker.def_move = prod.production.movement[i];

            move_marker.input = {'\u0003'};
            move_marker.input_index = {i+1};

            move_marker.output = {'\u0000'};
            move_marker.output_index = {i*2+new_control};
            move_marker.move = {prod.production.movement[i]};

            new_transitions.transitions.push_back(move_marker);

            IncompleteTransition move_marker_back;
            move_marker_back.state = prod.state+"_mark"+ to_string(counter);
            move_marker_back.to_state = prod.state+"_write"+ to_string(counter);
            move_marker_back.def_move = -1*prod.production.movement[i];

            move_marker_back.input = {'\u0003'};
            move_marker_back.input_index = {i+1};

            move_marker_back.output = {'\u0002', 'X'};
            move_marker_back.output_index = {i+1, i*2+new_control};
            move_marker_back.move = {-1*prod.production.movement[i], -1*prod.production.movement[i]};

            new_transitions.transitions.push_back(move_marker_back);

        }

        all_store_options = tools->mergeToSingle(soon_merging);
        new_transitions.transitions.insert(new_transitions.transitions.begin(), all_store_options.begin(), all_store_options.end());

        IncompleteTransition toNextMode;
        toNextMode.state = prod.state+"_write"+ to_string(counter);

        for (int  i =new_control-1; i<new_control; i++){
            toNextMode.input.push_back('\u0002');
            toNextMode.input_index.push_back(i);
        }

        toNextMode.to_state = prod.production.new_state;
        toNextMode.def_move = -1;

        new_transitions.transitions.push_back(toNextMode);
        counter++;
    }


    vector<Transition> real_transitions;
    for (auto incomp: new_transitions.transitions){
        Transition t = tools->make_transition(incomp, new_control+tapes.size()*2);

        real_transitions.push_back(t);

        //json production = add_transition(t);
        //TM_data["Productions"].push_back(production);
    }

    output_tm.load({}, start_state, "", tapes.size()*2, real_transitions);

    for (int i =0; i<tapes.size(); i++){
        string head;
        for (int j =0; j<tapes[i]->getTapeHeadIndex(); j++){
            head += " ";
        }

        head += "X";

        output_tm.load_input(head,i*2);
        output_tm.load_input(tapes[i]->exportTape(),i*2+1);
    }

    /*
    vector<char> kleene_starts;
    vector<int> moving_list;
    vector<int> moving_list_2;
    for (int i =0; i<tapes.size(); i++){
        kleene_starts.push_back('\u0001');
    }

    for (int i =0; i<tapes.size()*3+1; i++){
        moving_list.push_back(1);
        moving_list_2.push_back(0);
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
            new_t.production.movement = moving_list_2;
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
                    new_t.input[k+1] = '\u0002';
                    new_t.production.replace_val[k+1] = c;
                    new_productions.push_back(new_t);
                }
                new_t.input[index] = '\u0001';
                new_t.input[index+1] = '\u0001';
                new_t.production.replace_val[k+1] = '\u0001';
                new_t.input[k+1] = '\u0001';

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
    start_p.new_state = start_state;
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
     */
    return output_tm;
}

void TuringMachine::makeStorage(int size) {
    free(storage_in_state);
    storage_in_state_size = size;
    storage_in_state = (char*) calloc(size, 1);
    for (int i=0; i<storage_in_state_size; i++){
        storage_in_state[i] = '\u0002';
    }

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

string TuringMachine::getControlStorage() {
    string s;
    for (int i =0; i<storage_in_state_size; i++){
        s += storage_in_state[i];
    }
    return s;
}

