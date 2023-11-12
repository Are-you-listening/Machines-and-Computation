//
// Created by tibov on 03/11/23.
//

#include "TuringTokenizer.h"
#include "TuringTools.h"



TuringTokenizer::TuringTokenizer():tuple_size{4} {
    tapes = tuple_size+5;
    tools = new TuringTools(tapes-1);
    seperators = {'=', ';', '{', '}', '\u0000'};
    special_sep = {'{', '}'};
}

json TuringTokenizer::tokenize() {
    json TM_data;
    vector<string> states = {"still need to do"};
    for (int i = 0; i<tuple_size; i++){
        states.push_back("tokenize_"+to_string(i));
    }
    TM_data["States"] = states;

    TM_data["Tapes"] = tapes; // 2 for resulting marker and token same for original

    TM_data["Start"] = "program_start";



    ifstream f("TestFiles/TMStaticTransitions.json");
    json data = json::parse(f);

    IncompleteSet program("program_start", "program_start");
    tools->push(program, '*');

    IncompleteSet result("tokenize_mark_start", "tokenize_mark_start");
    // int stack start symbol


    tools->link_put(result, {'S'}, {0});



    IncompleteSet tokenize_seperator("tokenize_seperator", "tokenize_seperator");
    tools->move(tokenize_seperator, 0, 1);
    tools->link_put(tokenize_seperator, {'E'}, {0});
    tools->move(tokenize_seperator, 0, -1);

    for (char s: seperators){
        tools->link_on(result, tokenize_seperator, {s}, {1});
    }

    tools->move(result, 0, 1);
    tools->move(result, 1, 1);
    tools->go_to(result, seperators, 1, 1, {0, 1});
    IncompleteSet temp("tokenize_mark_end", "tokenize_mark_end");
    tools->link_put(result, temp, {'E'}, {0});
    tools->go_to(result, {'S'}, 0, -1, {0, 1});

    tools->link_put(result, {'S'}, {2});
    IncompleteSet tokenization("tokenize_link","tokenize_link");
    for (int i = 0; i< data.size(); i++){
        json sub = data[i];
        IncompleteTransition incomp(sub);

        tools->add(tokenization, incomp);
    }
    tokenization.to_state = "tokenize_link";

    auto v = tokenize_runner_productions();
    tools->link(tokenization, v);

    tools->link(result, tokenization);

    //before here, tokenize of 1 token without classier symbol

    tools->stack_replace(result, {'A','P','A'}, {'D'});
    tools->stack_replace(result, {'A','P','A', 'P'}, {'D'});
    tools->stack_replace(result, {'P'}, {'A'});

    //guarantees right token on top

    //here we do define the define on other place token function
    IncompleteSet set_token_else("set_token_else", "set_token_else");
    vector<int> tuple_set_indexes;
    for (int i= 0; i<tuple_size+2; i++){
        tuple_set_indexes.push_back(i+2);
    }

    tools->link_put(set_token_else, {'E'}, {2});
    tools->go_to(set_token_else, {'S'}, 2, -1, tuple_set_indexes);
    tools->move(set_token_else, tapes - 1, -1);
    tools->copy(set_token_else, tapes - 1, 3);
    tools->move(set_token_else, tapes - 1, 1);
    tools->go_to(set_token_else, {'E'}, 2, 1, tuple_set_indexes);

    tools->link_on(result, set_token_else, {'E'}, {3});
    //end link on

    IncompleteSet set_token("set_token", "set_token");
    tools->move(set_token, tapes - 1, -1);
    tools->copy(set_token, tapes - 1, 3);
    tools->move(set_token, tapes - 1, 1);

    tools->link_on(result, set_token, {'\u0000'}, {3});

    for (int m: tuple_set_indexes){
        tools->move(result, m, 1);
    }

    tools->clear_stack(result);

    tools->make_loop(result);


    tools->link(program, result);

    TM_data["Input"] = "";




    for (auto incomp: program.transitions){
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

IncompleteSet TuringTokenizer::tokenize_runner_productions() {
    IncompleteSet final_tokenize_set("tokenize_pre_setup", "tokenize_pre_setup");
    for (int i = 0; i<tuple_size; i++){
        string from = "tokenize_"+to_string(i);
        string to;

        if (i+1 < tuple_size){
            to = "tokenize_"+to_string(i+1)+"_replace";
        }else{
           to = "tokenize_extension_1_replace";
        }

        IncompleteSet tokenize_set(from,to);
        for (int j =32; j<127; j++){
            bool is_spatie = j == 32;
            bool is_seperator = (find(seperators.begin(), seperators.end(), (char) j) != seperators.end());

            IncompleteTransition trans_prod;
            trans_prod.state = from;

            trans_prod.to_state = to;

            char c = (char) j;
            trans_prod.input = {c};
            trans_prod.input_index = {1};
            trans_prod.output = {'\u0001', c, c};
            trans_prod.output_index = {0, 1, i+4};
            trans_prod.move = {1, 1, 0};
            trans_prod.def_move = 0;

            if (is_spatie){
                IncompleteSet spatie_pusher("tokenize_spatie"+ to_string(i), "tokenize_spatie"+ to_string(i));

                //sets S on stack if no other S is before
                tools->stack_replace(spatie_pusher, {'*'}, {'P'});
                tools->stack_replace(spatie_pusher, {'A'}, {'P'});

                trans_prod.to_state = spatie_pusher.state;

                IncompleteTransition go_back;
                go_back.state = spatie_pusher.to_state;
                go_back.to_state = to.substr(0, to.size()-8);
                go_back.def_move = 0;
                tokenize_set.transitions.push_back(go_back);
                tokenize_set.transitions.insert(tokenize_set.transitions.end(), spatie_pusher.transitions.begin(), spatie_pusher.transitions.end());
            }

            if (is_seperator){
                bool is_special = (find(special_sep.begin(), special_sep.end(), (char) j) != special_sep.end());
                if (is_special){
                    tools->push(trans_prod, (char) j);
                }else{
                    tools->push(trans_prod, 'S');
                }

            }

            tokenize_set.transitions.push_back(trans_prod);

        }

        IncompleteTransition end_marker;
        end_marker.state = from;
        end_marker.to_state = "tokenize_sub_finished_token";
        end_marker.input = {'E'};
        end_marker.input_index = {0};

        tokenize_set.transitions.push_back(end_marker);
        //sets A to say here is a token
        tools->stack_replace(tokenize_set, {'*'}, {'A'});
        tools->stack_replace(tokenize_set, {'P'}, {'A'});

        tools->link(final_tokenize_set, tokenize_set);


    }

    IncompleteSet end_marking("tokenize_extension_1","tokenize_extension_1");
    tools->link(final_tokenize_set, end_marking);

    final_tokenize_set.to_state = "tokenize_sub_finished_token";
    return final_tokenize_set;
}

