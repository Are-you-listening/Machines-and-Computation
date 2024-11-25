//
// Created by tibov on 02/11/23.
//

#include "TuringMachine.h"
#include <thread>
#include <mutex>
#include <chrono>
std::mutex mutex_x;
std::mutex mutex_counter;
std::mutex mutex_debug;

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

    this->states.clear();
    this->tapes.clear();
    this->production_trees.clear();

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

string TuringMachine::exportTapeData(unsigned int index, bool full) const {
    return tapes[index]->exportTape(full);
}

int TuringMachine::getTuringIndex(int index) {
    return tapes[index]->getTapeHeadIndex();
}

void TuringMachine::clear(bool full) {
    for (auto tape: tapes){
        if (full){
            delete tape;
        }else{
            tape->clear();
        }


    }
    if (full){
        tapes.clear();
    }

    current_state = start_state;
    halted = false;
}

TuringMachine* TuringMachine::toSingleTape() {
    TuringMachine* output_tm = new TuringMachine;

    output_tm->makeStorage(tapes.size()+1);
    int new_control = tapes.size()+1;
    int mark_track = tapes.size()+1 + tapes.size()*2;

    auto tools = TuringTools::getInstance(-1, -1);

    vector<int> storage_in_state_indexes;
    vector<int> all_moving;
    vector<int> none_moving;
    for (int i =0; i<new_control; i++){
        storage_in_state_indexes.push_back(i);
        all_moving.push_back(1);
        none_moving.push_back(0);
    }

    IncompleteSet new_transitions{"new_transitions", "new_transitions"};
    int counter = 0;
    auto trans_map = getProductions();

    IncompleteTransition start;
    start.state = "singletape_start_tape";
    start.to_state = start_state;
    start.def_move = -1;
    new_transitions.transitions.push_back(start);

    vector<vector<IncompleteTransition>> add_list;

    vector<thread> t_list;
    for (auto [k, v]: trans_map){


        if (v.size() > 4){
            if (t_list.size() == 10){
                auto d = std::move(t_list[0]);
                d.join();
                t_list.erase(t_list.begin());
            }

            //here stuff
            auto t = thread{[this, &add_list, k, v, &counter, &mark_track, &new_control, &storage_in_state_indexes, &none_moving] (){ singleTapeProd(add_list, k, v, counter, mark_track, new_control, storage_in_state_indexes, none_moving);}};
            t_list.push_back(std::move(t));
        }else{
            singleTapeProd(add_list, k, v, counter, mark_track, new_control, storage_in_state_indexes, none_moving);
        }




    }
    for (auto& t: t_list){
        t.join();
    }

    cout << "done" << endl;

    vector<Transition> real_transitions;
    real_transitions.reserve(new_transitions.transitions.size()+1+add_list.size());
    for (auto& incomp: new_transitions.transitions){
        Transition t = tools->make_transition(incomp, new_control+tapes.size()*2+1);

        real_transitions.push_back(t);

        //json production = add_transition(t);
        //TM_data["Productions"].push_back(production);
    }
    auto t0 = std::chrono::high_resolution_clock::now();

    for (auto& a:add_list){
        for (auto& incomp: a){
            Transition t = tools->make_transition(incomp, new_control+tapes.size()*2+1);

            real_transitions.push_back(t);

            //json production = add_transition(t);
            //TM_data["Productions"].push_back(production);
        }
    }
    auto t1 = std::chrono::high_resolution_clock::now();

    auto d = std::chrono::duration_cast<std::chrono::milliseconds>(t1-t0);
    std::cout << d.count() << "ms" << endl;

    output_tm->load({}, start.state, "", tapes.size()*2+1, real_transitions);

    for (int i =0; i<tapes.size(); i++){
        string head;
        for (int j =0; j<tapes[i]->getTapeHeadIndex(); j++){
            head += " ";
        }

        head += "X";

        output_tm->load_input(head,i*2);
        output_tm->load_input(tapes[i]->exportTape(),i*2+1);
    }

    output_tm->setSingleTape(true);
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

map<string, vector<Transition>> TuringMachine::getProductions() const{
    map<string, vector<Transition>> out;

    for (auto [k, v]: production_trees){
        auto t = v->traverse();

        out.insert({k, {}});
        for (auto u: t){
            u.state = k;
            out[k].push_back(u);
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

set<int> TuringMachine::getUsefullIndexes(const Transition &t) {
    set<int> out;

    for (int i =0; i<t.input.size(); i++){
        if (t.input[i] != '\u0001' || t.production.replace_val[i] != '\u0001' || t.production.movement[i]){
            out.insert(i);
        }
    }
    return out;
}

set<int> TuringMachine::getUsefullIndexesParent(const vector<Transition> &t_list) {
    set<int> out;
    for (const auto& t: t_list){
        for (int i =0; i<t.input.size(); i++){
            if (t.input[i] != '\u0001' || t.production.replace_val[i] != '\u0001' || t.production.movement[i]){
                out.insert(i);
            }
        }
    }

    return out;
}

void TuringMachine::setSingleTape(bool singleTape) {
    single_tape = singleTape;
}

bool TuringMachine::isSingleTape() const {
    return single_tape;
}

void TuringMachine::singleTapeProd(vector<vector<IncompleteTransition>>& new_transitions, const string& k, const vector<Transition>& v, int& counter, const int& mark_track,
                                   const int& new_control, const vector<int>& storage_in_state_indexes, const vector<int>& none_moving){
    vector<IncompleteTransition> trans_incomp;
    if (v.size() <= 3){
        trans_incomp.reserve(10);
    }else{
        trans_incomp.reserve(50);
    }


    //later skip store part for same state
    IncompleteTransition loop_state;
    loop_state.state = k;
    loop_state.to_state = k;
    loop_state.def_move = 1;

    loop_state.output = {'N'};
    loop_state.output_index = {mark_track};
    loop_state.move = {1};

    trans_incomp.push_back(std::move(loop_state));

    set<int> usefullUpper = getUsefullIndexesParent(v);

    //later skip store part for same state
    for (const auto& prod: v){
        mutex_counter.lock();
        cout << counter << '\n';
        counter++;
        int local_count = counter;
        mutex_counter.unlock();

        set<int> usefull = getUsefullIndexes(prod);
        if (usefull.empty() && v.size() == 1){
            IncompleteTransition epsilon;
            epsilon.state = k;
            epsilon.to_state = prod.production.new_state;
            epsilon.def_move = 0;
            trans_incomp.push_back(std::move(epsilon));

            continue;
        }

        set<IncompleteTransition> soon_merging;
        for (int i =0; i<tapes.size(); i++){
            if (usefullUpper.find(i) == usefullUpper.end()){
                continue;
            }

            IncompleteTransition store;
            store.state = k;
            store.to_state = k;
            store.def_move = 1;

            store.input = {'X', prod.input[i]};
            store.input_index = {i*2+new_control, i*2+new_control+1};

            char new_token = 'X';
            if (usefullUpper.find(i) != usefullUpper.end()){
                new_token = 'P';
            }

            store.output = {prod.input[i], new_token, 'N'};
            store.output_index = {i+1, i*2+new_control, mark_track};
            store.move = {1, 1, 1};

            store.control_increase = {0};
            store.increase_amount = {1};
            soon_merging.insert(std::move(store));

        }

        auto all_store_options = TuringTools::mergeToSingle(soon_merging);
        trans_incomp.insert(trans_incomp.begin(), make_move_iterator(all_store_options.begin()), make_move_iterator(all_store_options.end()));

        string write_state = k+"_write"+ to_string(local_count);
        IncompleteTransition toWriteMode;
        toWriteMode.state = k;

        toWriteMode.input = {(char) ('\u0002'+(int) usefullUpper.size())};
        toWriteMode.input.insert(toWriteMode.input.end(), prod.input.begin(), prod.input.end());
        toWriteMode.input_index = storage_in_state_indexes;

        toWriteMode.output = {(char) ('\u0002'+(int) usefullUpper.size())};
        for (auto p: prod.production.replace_val){
            if (p == '\u0001'){
                p = '\u0005';
            }
            toWriteMode.output.push_back(p);
        }
        //toWriteMode.output.insert(toWriteMode.output.end(), prod.production.replace_val.begin(), prod.production.replace_val.end());
        toWriteMode.output.push_back('N');
        toWriteMode.output_index = storage_in_state_indexes;
        toWriteMode.output_index.push_back(mark_track);
        toWriteMode.move = none_moving;
        toWriteMode.move.push_back(0);

        toWriteMode.to_state = write_state;
        toWriteMode.def_move = 0;

        trans_incomp.push_back(std::move(toWriteMode));

        IncompleteTransition loop_state3;
        loop_state3.state = write_state;
        loop_state3.to_state = write_state;
        loop_state3.def_move = -1;

        loop_state3.output = {'\u0000'};
        loop_state3.output_index = {mark_track};
        loop_state3.move = {-1};

        trans_incomp.push_back(std::move(loop_state3));


        //soon_merging = {};

        for (int i =0; i<tapes.size(); i++){
            auto self = usefull.find(i);
            if (self == usefull.end()){
                if (usefullUpper.find(i) != usefullUpper.end()){
                    IncompleteTransition cleanup;
                    cleanup.state = write_state;
                    cleanup.to_state = write_state;
                    cleanup.input = {'P'};
                    cleanup.input_index = {i*2+new_control};

                    cleanup.def_move = 0;
                    cleanup.output = {'X'};
                    cleanup.output_index = {i*2+new_control};
                    cleanup.move = {0};
                    trans_incomp.push_back(std::move(cleanup));
                }
                continue;
            }
            auto next = self;
            next++;

            string from;
            if (self == usefull.begin()){
                from = write_state+"_checkModeOverhead";
            }else{

                from = write_state+"doCheckOn_"+to_string((*self));
            }

            IncompleteTransition write;
            string to;
            if (next == usefull.end()){
                to = write_state;
                write.def_move = -1;
                write.move = {-1, -1};
            }else{
                to = write_state+"doCheckOn_"+to_string((*next));

            }


            write.state = from;
            write.to_state = k+"_mark"+ to_string(local_count)+"_"+ to_string(i);


            char c = prod.production.replace_val[i];
            if (c == '\u0001'){
                c = '\u0005';
            }

            write.input = {c, 'P'};
            write.input_index = {i+1, i*2+new_control};

            write.output = {'\u0004', '\u0000', prod.production.replace_val[i]};
            write.output_index = {i+1, i*2+new_control, i*2+1+new_control};

            write.def_move = prod.production.movement[i];
            write.move = {prod.production.movement[i], prod.production.movement[i], prod.production.movement[i]};


            write.control_increase = {0};
            write.increase_amount = {-1};
            //soon_merging.insert(write);
            trans_incomp.push_back(std::move(write));

            IncompleteTransition toCheckMode;
            toCheckMode.state = write_state;
            toCheckMode.to_state = write_state+"_checkModeOverhead";
            toCheckMode.def_move = 0;
            toCheckMode.input = {'P'};
            toCheckMode.input_index = {i*2+new_control};
            trans_incomp.push_back(std::move(toCheckMode));

            if (from != write_state){
                IncompleteTransition write_skip;
                write_skip.state = from;
                write_skip.to_state = to;
                write_skip.def_move = 0;
                trans_incomp.push_back(write_skip);
            }

            IncompleteTransition write_skip2;
            write_skip2.state = from;
            write_skip2.to_state = to;
            write_skip2.def_move = 0;
            write_skip2.input = {c};
            write_skip2.input_index = {i+1};
            trans_incomp.push_back(write_skip2);

            IncompleteTransition loop_state2;
            loop_state2.state = write_state;
            loop_state2.to_state = write_state;

            loop_state2.input = {c};
            loop_state2.input_index = {storage_in_state_indexes[i+1]};
            //loop_state2.input_index.erase(loop_state2.input_index.begin());

            loop_state2.output = {'\u0000'};
            loop_state2.output_index = {mark_track};
            loop_state2.move = {-1};

            loop_state2.def_move = -1;
            //new_transitions.transitions.push_back(loop_state2);


            if (prod.production.movement[i] < 0){


                for (int w=0; w<-1*prod.production.movement[i]; w++){
                    IncompleteTransition move_marker_back;
                    string temp_from = "";
                    if (w > 0){
                        temp_from += "_"+ to_string(w);
                    }
                    move_marker_back.state = k+"_mark"+ to_string(local_count)+"_"+ to_string(i)+temp_from;
                    string temp_to = to;
                    if (w+1 < -1*prod.production.movement[i]){
                        temp_to = k+"_mark"+ to_string(local_count)+"_"+ to_string(i)+"_"+to_string(w+1);
                    }
                    move_marker_back.to_state = temp_to;
                    move_marker_back.def_move = 1;

                    if (w == 0){
                        move_marker_back.output = {'\u0004', 'X', 'N'};
                        move_marker_back.output_index = {i+1, i*2+new_control, mark_track};
                        move_marker_back.move = {1, 1, 1};
                    } else{
                        move_marker_back.output = {'N'};
                        move_marker_back.output_index = {mark_track};
                        move_marker_back.move = {1};
                    }


                    trans_incomp.push_back(std::move(move_marker_back));
                }
            }else{
                IncompleteTransition move_marker_back;
                move_marker_back.state = k+"_mark"+ to_string(local_count)+"_"+ to_string(i);
                move_marker_back.to_state = to;
                move_marker_back.def_move = -1*prod.production.movement[i];

                move_marker_back.output = {'\u0004', 'X'};
                move_marker_back.output_index = {i+1, i*2+new_control};
                move_marker_back.move = {-1*prod.production.movement[i], -1*prod.production.movement[i]};

                trans_incomp.push_back(move_marker_back);
            }



        }



        //all_store_options = tools->mergeToSingle(soon_merging);
        //new_transitions.transitions.insert(new_transitions.transitions.begin(), all_store_options.begin(), all_store_options.end());

        IncompleteTransition toNextMode;
        toNextMode.state = write_state;

        for (int  i =new_control-1; i<new_control; i++){
            //toNextMode.input.push_back('\u0002');
            //toNextMode.input_index.push_back(i);

        }
        toNextMode.input = {'\u0000'};
        toNextMode.input_index = {mark_track};

        for (int c = 0; c <new_control; c++){
            toNextMode.output.push_back('\u0002');
            toNextMode.output_index.push_back(c);
            toNextMode.move.push_back(1);
        }

        toNextMode.to_state = prod.production.new_state;
        toNextMode.def_move = 1;

        trans_incomp.push_back(toNextMode);
    }

    mutex_x.lock();
    new_transitions.push_back(std::move(trans_incomp));
    mutex_x.unlock();
}
