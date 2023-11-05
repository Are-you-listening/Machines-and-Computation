//
// Created by tibov on 03/11/23.
//

#include "TuringTokenizer.h"
#include "TuringTools.h"

IncompleteTransition::IncompleteTransition(json &data) {

    state = data["state"].get<string>();
    to_state = data["to_state"].get<string>();
    def_move = data["def_move"].get<int>();

    for (int i = 0; i<data["input"].size(); i ++){
        input.push_back(data["input"][i].get<string>()[0]);
        input_index.push_back(data["input_index"][i].get<int>());
    }

    for (int i = 0; i<data["output"].size(); i ++){
        output.push_back(data["output"][i].get<string>()[0]);
        output_index.push_back(data["output_index"][i].get<int>());
        move.push_back(data["move"][i].get<int>());
    }
}

void IncompleteTransition::push(char symbol, int tape_size) {
    output.push_back(symbol);
    output_index.push_back(tape_size-1);
    move.push_back(1);
}


TuringTokenizer::TuringTokenizer():tuple_size{4} {

}

json TuringTokenizer::tokenize() {
    json TM_data;
    vector<string> states = {"still need to do"};
    for (int i = 0; i<tuple_size; i++){
        states.push_back("tokenize_"+to_string(i));
    }
    TM_data["States"] = states;
    tapes = tuple_size+5;
    TM_data["Tapes"] = tapes; // 2 for resulting marker and token same for original

    TM_data["Start"] = "tokenize_mark_start";

    TuringTools* tools = new TuringTools;

    ifstream f("TestFiles/TMStaticTransitions.json");
    json data = json::parse(f);
    vector<IncompleteTransition> incomp_list;

    //set end marker
    vector<IncompleteTransition> find_seperator = tools->go_to(';', 1, 1);
    incomp_list.insert(incomp_list.end(), find_seperator.begin(), find_seperator.end());

    //go to the start
    vector<IncompleteTransition> go_to_start = tools->go_to('S', 0, -1);
    incomp_list.insert(incomp_list.end(), go_to_start.begin(), go_to_start.end());

    //link the mark end to start and mark end to go to start
    incomp_list.push_back(TuringTools::link_put("tokenize_mark_start", find_seperator[0].state, {'S'}, {0}));
    incomp_list.push_back(TuringTools::link(find_seperator[1].to_state, go_to_start[0].state));
    incomp_list.push_back(TuringTools::link(go_to_start[1].to_state, "tokenize_0"));



    for (int i = 0; i< data.size(); i++){
        json sub = data[i];
        IncompleteTransition incomp(sub);
        incomp_list.push_back(incomp);
    }

    TM_data["Input"] = "";

    auto v = tokenize_runner_productions();
    incomp_list.insert(incomp_list.end(),v.begin(), v.end());
    for (auto incomp: incomp_list){
        Transition t = make_transition(incomp);
        json production = add_transition(t);
        TM_data["Productions"].push_back(production);
    }

    return TM_data;
}

Transition TuringTokenizer::make_transition(IncompleteTransition &incomp) {
    Transition transition;
    transition.state = incomp.state;
    vector<char> inputs;
    vector<char> outputs;
    vector<int> moves;

    int def_move = incomp.def_move;

    bool input_empty = incomp.input_index.empty();
    bool output_empty = incomp.output_index.empty();

    for (int i = 0; i< tapes; i++){
        if (!input_empty && incomp.input_index.front() == i){
            inputs.push_back(incomp.input.front());
            incomp.input.erase(incomp.input.begin());
            incomp.input_index.erase(incomp.input_index.begin());
        }else{
            inputs.push_back('\u0001');
        }

        if (!output_empty && incomp.output_index.front() == i){
            outputs.push_back(incomp.output.front());
            moves.push_back(incomp.move.front());
            incomp.output.erase(incomp.output.begin());
            incomp.move.erase(incomp.move.begin());
            incomp.output_index.erase(incomp.output_index.begin());
        }else{
            outputs.push_back('\u0001');
            moves.push_back(def_move);
        }
    }



    transition.input = inputs;
    Production p;
    p.new_state = incomp.to_state;
    p.replace_val = outputs;
    p.movement = moves;
    transition.production = p;
    return transition;
}

json TuringTokenizer::add_transition(Transition &transition) {
    json production;
    json to;
    production["state"] = transition.state;
    vector<string> input;
    for (char c: transition.input){
        string temp;
        temp += c;
        input.push_back(temp);
    }
    production["symbols"] = input;

    to["state"] = transition.production.new_state;

    vector<string> output;
    for (char c: transition.production.replace_val){
        string temp;
        temp += c;
        output.push_back(temp);
    }
    to["symbols"] = output;
    to["moves"] = transition.production.movement;
    production["to"] = to;
    return production;
}

vector<IncompleteTransition> TuringTokenizer::tokenize_runner_productions() {
    vector<IncompleteTransition> output;
    for (int j =32; j<127; j++){
        bool is_spatie = j == 32;
        for (int i = 0; i<tuple_size+1; i++){
            IncompleteTransition trans_prod;
            trans_prod.state = "tokenize_"+to_string(i);

            if (i+1 < tuple_size){
                trans_prod.to_state = "tokenize_"+to_string(i+1);
            }else{
                trans_prod.to_state = "tokenize_extension_1";
            }

            char c = (char) j;
            trans_prod.input = {c};
            trans_prod.input_index = {1};
            trans_prod.output = {c, c};
            trans_prod.output_index = {1, i+4};
            trans_prod.move = {1, 0};
            trans_prod.def_move = 0;

            if (is_spatie){
                trans_prod.push('S', tapes);
            }

            output.push_back(trans_prod);

        }

    }

    return output;
}


