//
// Created by tibov on 03/11/23.
//

#include "TuringTokenizer.h"
#include "src/TM/TuringTools.h"



TuringTokenizer::TuringTokenizer(): TuringGenerator(4) {
}



IncompleteSet TuringTokenizer::tokenize() {



    IncompleteSet result("tokenize_mark_start", "tokenize_mark_start");
    // int stack start symbol

    //tools->link_put(result, {'S'}, {0});

    tools->write_on(result, {'\u0000'}, {0}, {'S'}, {0});
    tools->write_on(result, {'E'}, {0}, {'S'}, {0});

    string end_tokenization_state = tools->branch_on(result, {'\u0000'}, {1});

    IncompleteSet tokenize_seperator("tokenize_seperator", "tokenize_seperator");
    tools->move(tokenize_seperator, {0}, 1);
    tools->link_put(tokenize_seperator, {'E'}, {0});
    tools->move(tokenize_seperator, {0}, -1);

    for (char s: seperators){
        tools->link_on(result, tokenize_seperator, {s}, {1});
    }

    tools->move(result, {0, 1}, 1);

    IncompleteSet find_seperator("tokenize_find_seperator", "tokenize_find_seperator");
    //need to loop this, and repeat if ::
    tools->go_to(find_seperator, seperators, 1, 1, {0, 1});
    tools->move(find_seperator, {0,1}, 2);
    tools->make_loop_on_sequence(find_seperator, {':', ':'}, 1);
    tools->move(find_seperator, {0,1}, -2);



    tools->link(result, find_seperator);

    tools->link_put(result, {'E'}, {0});

    tools->go_to(result, {'S', 'A'}, 0, -1, {0, 1});



    tools->write_on(result, {'\u0000'}, {2}, {'S'}, {2});
    tools->write_on(result, {'E'}, {2}, {'S'}, {2});

    IncompleteSet tokenization = tokenize_runner_productions();

    IncompleteSet check_if ("check_if","check_if");
    tools->push(check_if, '(');

    tools->link_on(tokenization, check_if, {'('}, {1});

    IncompleteSet check_if2 ("check_if2","check_if2");
    tools->push(check_if2, ':');

    tools->link_on(tokenization, check_if2, {':'}, {1});

    tools->link(result, tokenization);

    //before here, tokenize of 1 token without classier symbol

    tools->stack_replace(result, {'A','P','A'}, {'D'});
    tools->stack_replace(result, {'A','P','A', 'P'}, {'D'});
    tools->stack_replace(result, {'A', 'P'}, {'A'});

    tools->stack_replace(result, {'A','P','A', '('}, {'U'});
    tools->stack_replace(result, {'A','P','A', 'P', '('}, {'U'});
    tools->stack_replace(result, {'A', '('}, {'F'});

    tools->stack_replace(result, {':', ':', '('}, {'O'});
    tools->stack_replace(result, {'A', ':'}, {'I'});

    //make sure we still have usefull tokens
    //tools->stack_replace(result, {'('}, {'A'});
    tools->stack_replace(result, {':'}, {'S'});



    //guarantees right token on top

    //here we do define the define on other place token function
    IncompleteSet set_token_else("set_token_else", "set_token_else");
    vector<int> tuple_set_indexes = get_tuple_index();

    tools->link_put(set_token_else, {'E'}, {2});
    tools->go_to(set_token_else, {'S', 'A'}, 2, -1, tuple_set_indexes);
    tools->move(set_token_else, {(int) tapes - 1}, -1);
    tools->copy(set_token_else, tapes - 1, 3);
    tools->move(set_token_else, {(int) tapes - 1}, 1);
    tools->go_to(set_token_else, {'E'}, 2, 1, tuple_set_indexes);

    //in case we have an empty extension tuple
    IncompleteSet clear_extension("clear_extension", "clear_extension");
    tools->link_put(clear_extension, {'\u0000'}, {3});
    tools->move(clear_extension, tuple_set_indexes, -1);
    tools->link_on(set_token_else, clear_extension, {'\u0000'}, {4});
    //end of case

    tools->link_on(result, set_token_else, {'E'}, {3});
    //end link on

    IncompleteSet set_token("set_token", "set_token");
    tools->move(set_token, {(int) tapes - 1}, -1);
    tools->copy(set_token, tapes - 1, 3);
    tools->move(set_token, {(int) tapes - 1}, 1);

    tools->link_on(result, set_token, {'\u0000'}, {3});

    tools->move(result, tuple_set_indexes, 1);

    tools->clear_stack(result);

    tools->make_loop(result);
    result.to_state = end_tokenization_state;

    //do cleanup
    tools->go_to_clear(result, {'A'}, 0, -1, {0, 1}, {0,1});
    tools->link_put(result, {'\u0000'}, {1});
    tools->go_to_clear(result, {'A'}, 2, -1, get_tuple_index(), {2});

    return result;
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
            bool is_double_dot = j == 58;
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

                IncompleteSet class_checker("tokenize_class_checker"+ to_string(i), "tokenize_class_checker"+ to_string(i));
                tools->push(class_checker, 'C');
                tools->link_on_sequence(spatie_pusher, class_checker, {'c','l','a','s','s', ' '}, 1);

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
    tools->move(final_tokenize_set, get_tuple_index(), 1);
    tools->link_put(final_tokenize_set, {'E'}, {3});
    tools->make_loop(final_tokenize_set);

    final_tokenize_set.to_state = "tokenize_sub_finished_token";
    return final_tokenize_set;
}

IncompleteSet TuringTokenizer::getTransitions() {
    return tokenize();
}


