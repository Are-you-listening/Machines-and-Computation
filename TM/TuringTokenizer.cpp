//
// Created by tibov on 03/11/23.
//

#include "TuringTokenizer.h"
IncompleteTransition::IncompleteTransition(json data) {

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



TuringTokenizer::TuringTokenizer():tuple_size{4} {

}

void TuringTokenizer::tokenize() {
    json TM_data;
    vector<string> states = {"tokenize_spatie", "tokenize_mark_start", "tokenize_mark_end", "tokenize_go_to_start"};
    for (int i = 0; i<tuple_size; i++){
        states.push_back("tokenize_"+to_string(i));
    }
    TM_data["States"] = states;
    tapes = tuple_size+4;
    TM_data["Tapes"] = tapes; // 2 for resulting marker and token same for original

    TM_data["Start"] = "tokenize_mark_start";

    /**
     * productions list
     *
     * mark_start: ["", *, *, ...] -> ["S", *, *, ....], mark_end, R
     * mark_end: ["", ;, *, ...] -> ["E", *, *, ....], go_to_start, L
     * mark_end: ["", ',', *, ...] -> ["E", *, *, ....], go_to_start, L
     * mark_end: ["", =, *, ...] -> ["E", *, *, ....], go_to_start, L
     * mark_end: ["", *, *, ...] -> ["", *, *, ....], mark_end, L
     * */

    ifstream f("TestFiles/TMStaticTransitions.json");
    json data = json::parse(f);
    json sub = data[0];
    IncompleteTransition temp_0(sub);
    make_transition(temp_0);
    int i = 0;
}

Transition TuringTokenizer::make_transition(IncompleteTransition &incomp) {
    Transition transition;
    transition.state = incomp.state;
    vector<char> inputs;
    vector<char> outputs;
    vector<int> moves;

    int def_move = incomp.def_move;
    for (int i = 0; i< tapes; i++){
        if (incomp.input_index.front() == i){
            inputs.push_back(incomp.input.front());
            incomp.input.erase(incomp.input.begin());
            incomp.input_index.erase(incomp.input_index.begin());
        }else{
            inputs.push_back('\u0001');
        }

        if (incomp.output_index.front() == i){
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


