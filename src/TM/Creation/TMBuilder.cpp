//
// Created by tibov on 10/11/23.
//

#include "TMBuilder.h"

TMBuilder::TMBuilder(unsigned int tuple_size, bool if_else_antinesting, int split_nesting, int max_nesting): tapes{tuple_size+5} {
    TuringTools::reset();
    tools = TuringTools::getInstance(tapes-1, split_nesting);

    TMBuilder::split_nesting = split_nesting;
    TMBuilder::max_nesting = max_nesting;

    if_else_anti = if_else_antinesting;
}


TMBuilder_output TMBuilder::generateTM() {
    tools->resetCounter();
    //json TM_data;
    vector<string> states = {"still need to do"};

    TMBuilder_output TM_data{};
    //TM_data["States"] = states;
    TM_data.states = states;

    //TM_data["Tapes"] = tapes; // 2 for resulting marker and token same for original
    TM_data.tape_size = tapes;

    //TM_data["Start"] = "program_start";

    TM_data.start_state = "program_start";

    //TM_data["Input"] = "";
    TM_data.input = "";


    IncompleteSet program("program_start", "program_start");
    tools->push(program, '#');
    tools->push(program, '-');
    tools->link_put(program, {'A', 'A'}, {0, 2});
    //tools->move(program, {0,1}, -1);
    //tools->move(program, {0,1}, 1);

    TuringTokenizer tokenizer{(int) tapes-5};
    IncompleteSet tokenize_program = tokenizer.getTransitions();

    tools->link(program, tokenize_program);

    IncompleteSet breaker{"breaker", "breaker2"};
    //tools->link(program, breaker);


    if (if_else_anti){
        TuringIfElseAntiNesting ifElse{(int) tapes-5, split_nesting, max_nesting};
        IncompleteSet if_else_program = ifElse.getTransitions();
        tools->link(program, if_else_program);
    }

    TuringVarDictionary vardict{(int) tapes-5};

    IncompleteSet vardict_set = vardict.getTransitions();
    tools->link(program, vardict_set);


    TuringDenestify denest{(int) tapes-5, split_nesting, max_nesting};

    tools->link(program, denest.getTransitions());

    tools->clear_heap(program);

    IncompleteTransition re_heap;
    re_heap.state = program.to_state;
    re_heap.to_state = vardict_set.state;
    re_heap.def_move = 0;

    program.transitions.push_back(re_heap);

    for (auto incomp: program.transitions){
        Transition t = tools->make_transition(incomp, tapes);

        TM_data.productions.push_back(t);

        //json production = add_transition(t);
        //TM_data["Productions"].push_back(production);
    }

    return TM_data;
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