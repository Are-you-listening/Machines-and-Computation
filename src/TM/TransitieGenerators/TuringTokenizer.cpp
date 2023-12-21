//
// Created by tibov on 03/11/23.
//

#include "TuringTokenizer.h"

TuringTokenizer::TuringTokenizer(int tuple_size): TuringGenerator(tuple_size) {
}



IncompleteSet TuringTokenizer::tokenize() {



    IncompleteSet result("tokenize_mark_start", "tokenize_mark_start");
    // int stack start symbol

    //tools->link_put(result, {'S'}, {0});

    tools->write_on(result, {'\u0000'}, {0}, {'S'}, {0});
    tools->write_on(result, {'E'}, {0}, {'S'}, {0});

    tools->go_to_not(result, {' ', '\n'}, 1, 1, {0,1});
    //branch on include
    string on_include = tools->branch_on(result, {'#'}, {1});

    IncompleteSet checkSecondBracket{"check_second_bracket_tokenazation", "check_second_bracket_tokenazation"};
    tools->move(checkSecondBracket, {0, 1}, 1);
    string on_comment = tools->branch_on(checkSecondBracket, {'/'}, {1});
    tools->move(checkSecondBracket, {0, 1}, -1);

    tools->link_on(result, checkSecondBracket, {'/'}, {1});

    tools->go_to(result, {'S', 'A'}, 0, -1, {0,1});

    string end_tokenization_state = tools->branch_on(result, {'\u0000'}, {1});

    IncompleteSet tokenize_seperator("tokenize_seperator", "tokenize_seperator");
    tools->move(tokenize_seperator, {0}, 1);
    tools->link_put(tokenize_seperator, {'E'}, {0});
    tools->move(tokenize_seperator, {0}, -1);

    for (char s: seperators){
        tools->link_on(result, tokenize_seperator, {s}, {1});
    }

    IncompleteSet onString{"on_string_tokenazation", "on_string_tokenazation"};
    tools->move(onString, {0,1}, 1);
    tools->go_to(onString, {'\"'}, 1, 1, {0, 1});
    tools->move(onString, {0,1}, 1);
    tools->link_put(onString, {'E'}, {0});
    tools->move(onString, {0,1}, -1);
    tools->go_to_clear(onString, {'S', 'A'}, 0, -1, {0,1}, {0});

    tools->link_on(result, onString, {'\"'}, {1});

    string skip_find_seperator_1 = result.to_state;

    tools->move(result, {0, 1}, 1);


    IncompleteSet find_seperator("tokenize_find_seperator", "tokenize_find_seperator");
    //need to loop this, and repeat if ::
    tools->go_to(find_seperator, seperators, 1, 1, {0, 1});

    IncompleteSet onAmpercent{"tokenize_onAmpercent", "tokenize_onAmpercent"};
    tools->move(onAmpercent, {0,1}, 1);

    IncompleteSet onAmpercentNot{"tokenize_onAmpercentNot", "tokenize_onAmpercentNot"};
    tools->go_to(onAmpercentNot, seperators, 1, 1, {0, 1});
    tools->move(onAmpercentNot, {0,1}, 1);
    tools->link_on_not(onAmpercent, onAmpercentNot, {'&'}, {1});
    tools->move(onAmpercent, {0,1}, -1);

    tools->link_on(find_seperator, onAmpercent, {'&'}, {1});

    //first check if > has a corresponding < before
    IncompleteSet onLess{"on_less_tokenazation", "on_less_tokenazation"};
    tools->go_to_multiple(onLess, {{'S'}, {'<'}}, {0, 1}, -1, {0,1});
    IncompleteSet onNotS{"on_not_s_tokenazation", "on_not_s_tokenazation"};
    tools->go_to(onNotS, {'>'}, 1, 1, {0, 1});
    tools->move(onNotS, {0,1}, 1);
    tools->go_to(onNotS, seperators, 1, 1, {0,1});
    tools->make_loop_on(onNotS, '>', 1);

    IncompleteSet onTemplate{"on_template_tokenazation", "on_template_tokenazation"};
    tools->go_to(onTemplate, {'>'}, 1, 1, {0, 1});
    string temp = onTemplate.to_state;
    onTemplate.to_state = "on_template_tokenazation_unreached";

    tools->link_on_sequence(onLess, onTemplate, {'t', 'e', 'm', 'p', 'l', 'a', 't', 'e'}, {1});

    string on_not_s = tools->branch_on(onLess, {'<'}, {1});

    tools->go_to(onLess, {'>'}, 1, 1, {0, 1});


    IncompleteTransition toOnNotS;
    toOnNotS.state = on_not_s;
    toOnNotS.to_state = onNotS.state;
    toOnNotS.def_move = 0;

    IncompleteTransition fromOnNotS;
    fromOnNotS.state = onNotS.to_state;
    fromOnNotS.to_state = onLess.to_state;
    fromOnNotS.def_move = 0;

    onLess.transitions.push_back(toOnNotS);
    onLess.transitions.push_back(fromOnNotS);
    onLess.transitions.insert(onLess.transitions.end(), onNotS.transitions.begin(), onNotS.transitions.end());

    tools->link_on(find_seperator, onLess, {'>'}, {1});

    IncompleteTransition tohere;
    tohere.state = temp;
    tohere.to_state = find_seperator.to_state;
    tohere.def_move = 0;

    find_seperator.transitions.push_back(tohere);



    tools->move(find_seperator, {0,1}, 2);
    tools->make_loop_on_sequence(find_seperator, {':', ':'}, 1);
    tools->move(find_seperator, {0,1}, -2);

    tools->link(result, find_seperator);

    tools->link_put(result, {'E'}, {0});

    IncompleteSet onInclude{"onInclude", "onInclude"};
    tools->go_to(onInclude, {'\n'}, 1, 1, {0, 1});
    tools->push(onInclude, 'I');
    tools->link_put(onInclude, {'E'}, {0});

    for (auto s: {on_include, on_comment}){
        IncompleteTransition toOnInclude;
        toOnInclude.state = s;
        toOnInclude.to_state = onInclude.state;
        toOnInclude.def_move = 0;
        result.transitions.push_back(toOnInclude);
    }

    IncompleteTransition fromOnInclude;
    fromOnInclude.state = onInclude.to_state;
    fromOnInclude.to_state = result.to_state;
    fromOnInclude.def_move = 0;


    result.transitions.push_back(fromOnInclude);
    result.transitions.insert(result.transitions.end(), onInclude.transitions.begin(), onInclude.transitions.end());
    tools->move(result, {0,1}, -1);

    for (char c: ignore_sep){
        IncompleteTransition skip_find_sep;
        skip_find_sep.state = skip_find_seperator_1;
        skip_find_sep.to_state = result.to_state;
        skip_find_sep.def_move = 0;
        skip_find_sep.input = {c};
        skip_find_sep.input_index = {1};
        result.transitions.push_back(skip_find_sep);
    }

    tools->go_to_clear(result, {'S', 'A'}, 0, -1, {0, 1}, {0});



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

    //check for special tokens with regards to if/else

    tools->move(result, {0,1}, -1);
    tools->go_to_not(result, {' '}, 1, -1, {0,1});
    tools->move(result, {0,1}, 1);
    tools->push_on_sequence(result, {'i', 'f'}, 1, '0');
    tools->push_on_sequence(result, {'e', 'l', 's', 'e'}, 1, '1');
    tools->push_on_sequence(result, {'e', 'l', 's', 'e', ' ', 'i', 'f'}, 1, '2');
    tools->push_on_sequence(result, {'r','e','t', 'u', 'r', 'n'}, 1, 'R');
    tools->push_on_sequence(result, {'c','o','n', 't', 'i', 'n', 'u', 'e'}, 1, '3');
    tools->push_on_sequence(result, {'b','r','e', 'a', 'k'}, 1, '3');
    tools->go_to_not(result, {' '}, 1, 1, {0,1});

    //before here, tokenize of 1 token without classier symbol

    tools->stack_replace(result, {'.', '('}, {'A'});
    tools->stack_replace(result, {'A','P','A'}, {'D'});
    tools->stack_replace(result, {'A','P','A', 'P'}, {'D'});
    tools->stack_replace(result, {':',':','P','A'}, {'D'});
    tools->stack_replace(result, {':',':','P','A', 'S', 'A'}, {'D'});
    tools->stack_replace(result, {':',':','P','A', 'S', 'A', 'P'}, {'D'});

    tools->stack_replace(result, {'A', 'P'}, {'A'});

    tools->stack_replace(result, {'A','P','A', '('}, {'U'});
    tools->stack_replace(result, {'A','P','A', 'P', '('}, {'U'});
    tools->stack_replace(result, {'A',':',':','P','A', '('}, {'U'});
    tools->stack_replace(result, {'A', '('}, {'F'});
    tools->stack_replace(result, {'A', 'P' ,'('}, {'F'});

    tools->stack_replace(result, {':', ':', '('}, {'O'});
    tools->stack_replace(result, {'A', ':'}, {'I'});
    tools->stack_replace(result, {'.'}, {'A'});


    //make sure we still have useful tokens
    //tools->stack_replace(result, {'('}, {'A'});
    tools->stack_replace(result, {':'}, {'S'});
    tools->stack_replace(result, {'S', 'A'}, {'S'});
    tools->stack_replace(result, {'X'}, {'A'});
    tools->stack_replace(result, {'\n'}, {'I'});
    tools->stack_replace(result, {':',':','P'}, {'A'});
    tools->stack_replace(result, {stack_symbol, 'P','('}, {'P'});

    IncompleteSet putString{"put_string_tokenazation", "put_string_tokenazation"};
    tools->push(putString, 'A');
    tools->link_on_sequence(result, putString, {'\"'}, 1);



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
        for (int j =31; j<127; j++){

            bool is_spatie = j == 32;

            char c = (char) j;
            if (j == 31){
                c = '\n';
            }

            bool is_seperator = (find(seperators.begin(), seperators.end(), c) != seperators.end());
            bool is_less_symbol = j == 60;
            bool dot = j == 46;

            IncompleteTransition trans_prod;
            trans_prod.state = from;

            trans_prod.to_state = to;



            trans_prod.input = {c};
            trans_prod.input_index = {1};
            trans_prod.output = {'\u0001', c, c};
            trans_prod.output_index = {0, 1, i+4};
            trans_prod.move = {1, 1, 0};
            trans_prod.def_move = 0;

            if (is_spatie || c == '\n'){
                IncompleteSet spatie_pusher("tokenize_spatie"+ to_string(i), "tokenize_spatie"+ to_string(i));

                tools->push_on_sequence(spatie_pusher, {'c','l','a','s','s', ' '}, 1, 'C');
                tools->push_on_sequence(spatie_pusher, {'n','e','w', ' '}, 1, 'X');
                tools->push_on_sequence(spatie_pusher, {'d','e','l', 'e', 't', 'e', ' '}, 1, 'X');
                tools->push_on_sequence(spatie_pusher, {'r','e','t', 'u', 'r', 'n', ' '}, 1, 'R');
                tools->push_on_sequence(spatie_pusher, {'n','a','m', 'e', 's', 'p', 'a', 'c', 'e', ' '}, 1, 'C');

                //sets S on stack if no other S is before
                tools->stack_replace(spatie_pusher, {stack_symbol}, {'P'});
                tools->stack_replace(spatie_pusher, {'A'}, {'P'});
                tools->stack_replace(spatie_pusher, {':'}, {'P'});

                trans_prod.to_state = spatie_pusher.state;

                IncompleteTransition go_back;
                go_back.state = spatie_pusher.to_state;
                go_back.to_state = to.substr(0, to.size()-8);
                go_back.def_move = 0;
                tokenize_set.transitions.push_back(go_back);
                tokenize_set.transitions.insert(tokenize_set.transitions.end(), spatie_pusher.transitions.begin(), spatie_pusher.transitions.end());
            }

            if (is_less_symbol){
                IncompleteSet less_symbol_pusher("tokenize_less_symbol"+ to_string(i), "tokenize_less_symbol"+ to_string(i));
                tools->push_on_sequence(less_symbol_pusher, {'t','e','m','p','l', 'a', 't', 'e', '<'}, 1, 'T');

                trans_prod.to_state = less_symbol_pusher.state;

                IncompleteTransition go_back;
                go_back.state = less_symbol_pusher.to_state;
                go_back.to_state = to.substr(0, to.size()-8);
                go_back.def_move = 0;
                tokenize_set.transitions.push_back(go_back);
                tokenize_set.transitions.insert(tokenize_set.transitions.end(), less_symbol_pusher.transitions.begin(), less_symbol_pusher.transitions.end());
            }

            if (is_seperator){

                bool is_special = (find(special_sep.begin(), special_sep.end(), c) != special_sep.end());
                if (is_special){
                    tools->push(trans_prod, c);
                }else{
                    tools->push(trans_prod, 'S');
                }

            }

            if (dot){
                tools->push(trans_prod, '.');
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
        tools->stack_replace(tokenize_set, {stack_symbol}, {'A'});
        tools->stack_replace(tokenize_set, {'P'}, {'A'});
        tools->stack_replace(tokenize_set, {'S'}, {'A'});


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

