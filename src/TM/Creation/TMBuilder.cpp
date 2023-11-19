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

    //IncompleteSet test("test1", "test1");
    //tools->push(test, 'B');
    //tools->make_loop_on_sequence(test, {'#','*'}, (int) tapes-1);
    //tools->push(test, 'C');

    //tools->link(program, test);

    for (auto inComp: program.transitions){
        Transition t = make_transition(inComp);
        json production = add_transition(t);
        TM_data["Productions"].push_back(production);
    }

    return TM_data;
}

Transition TMBuilder::make_transition(IncompleteTransition &inComp) const {
    Transition transition;
    transition.state = inComp.state;
    vector<char> inputs;
    vector<char> outputs;
    vector<int> moves;

    int def_move = inComp.def_move;

    bool input_empty = inComp.input_index.empty();
    bool output_empty = inComp.output_index.empty();

    for (int i = 0; i< tapes; i++){
        if (!input_empty && inComp.input_index.front() == i){
            inputs.push_back(inComp.input.front());
            inComp.input.erase(inComp.input.begin());
            inComp.input_index.erase(inComp.input_index.begin());
        }else{
            inputs.push_back('\u0001');
        }

        if (!output_empty && inComp.output_index.front() == i){
            outputs.push_back(inComp.output.front());
            moves.push_back(inComp.move.front());
            inComp.output.erase(inComp.output.begin());
            inComp.move.erase(inComp.move.begin());
            inComp.output_index.erase(inComp.output_index.begin());
        }else{
            outputs.push_back('\u0001');
            moves.push_back(def_move);
        }
    }

    transition.input = inputs;
    Production p;
    p.new_state = inComp.to_state;
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
