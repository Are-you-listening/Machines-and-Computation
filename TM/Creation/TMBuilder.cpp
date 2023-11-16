//
// Created by tibov on 10/11/23.
//

#include "TMBuilder.h"

TMBuilder::TMBuilder(unsigned int tuple_size): tapes{tuple_size+5} {
    tools = TuringTools::getInstance(tapes-1);
}

json TMBuilder::generateTM() {
    json TM_data;
    vector<string> states = {"still need to do"};
    TM_data["States"] = states;

    TM_data["Tapes"] = tapes; // 2 for resulting marker and token same for original

    TM_data["Start"] = "program_start";

    TM_data["Input"] = "";


    IncompleteSet program("program_start", "program_start");
    tools->push(program, '#');
    tools->push(program, '*');
    tools->link_put(program, {'A', 'A'}, {0, 2});

    TuringTokenizer tokenizer{};
    IncompleteSet tokenize_program = tokenizer.getTransitions();

    tools->link(program, tokenize_program);

    TuringVarDictionary vardict{};

    tools->link(program, vardict.getTransitions());

    for (auto incomp: program.transitions){
        Transition t = make_transition(incomp);
        json production = add_transition(t);
        TM_data["Productions"].push_back(production);
    }

    return TM_data;
}

Transition TMBuilder::make_transition(IncompleteTransition &incomp) {
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

json TMBuilder::add_transition(Transition &transition) {
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
