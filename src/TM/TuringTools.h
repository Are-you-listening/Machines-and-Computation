//
// Created by tibov on 04/11/23.
//

#ifndef TOG_TURINGTOOLS_H
#define TOG_TURINGTOOLS_H

#include <iostream>
#include <vector>
#include "lib/json.hpp"
#include "TuringProduction.h"
#include <set>
using namespace std;
using json = nlohmann::json;

struct Transition{
public:
    Production production;
    string state;
    vector<char> input;
};

struct IncompleteTransition{
    IncompleteTransition(json &data);
    IncompleteTransition() = default;
    string state;
    string to_state;
    int def_move;
    vector<char> input;
    vector<int> input_index;
    vector<char> output;
    vector<int> output_index;
    vector<int> move;
    vector<int> control_increase = {};
    vector<int> increase_amount = {};
    bool operator==(const IncompleteTransition& other) const;
    bool operator<(const IncompleteTransition& other) const;
};


struct IncompleteSet{
    IncompleteSet(const string& state, const string& to_state);
    string state;
    string to_state;
    vector<IncompleteTransition> transitions;
};

class TuringTools {
public:
    static TuringTools* getInstance(unsigned int stack_tape);

    void go_to(IncompleteSet& a, const vector<char>& symbol, int tape_index, int direction);
    void go_to_multiple(IncompleteSet& a, const vector<vector<char>>& symbol, const vector<int>& tape_index, int direction, const vector<int>& affected);
    void go_to(IncompleteSet& a, const vector<char>& symbol, int tape_index, int direction, const vector<int>& affected);
    void go_to_not(IncompleteSet& a, const vector<char>& symbol, int tape_index, int direction, const vector<int>& affected);
    void go_to_clear(IncompleteSet& a, const vector<char>& symbol, int tape_index, int direction, const vector<int>& affected,
                     const vector<int>& cleared);
    void go_to_copy(IncompleteSet& a, const vector<char>& symbol, int tape_index, int direction, const vector<int>& affected,
                    int copy_to_tape, int copy_to_direction, const vector<int>& copy_affected);
    void go_to_copy_multiple(IncompleteSet& a, const vector<vector<char>>& symbol, const vector<int>& tape_index, int direction, const vector<int>& affected,
                             int copy_to_tape, int copy_to_direction, const vector<int>& copy_affected, int index);
    void go_to_move(IncompleteSet& a, const vector<char>& symbol, int tape_index, int direction, const vector<int>& affected,
                    int copy_to_tape, int copy_to_direction, const vector<int>& copy_affected);

    static void link(IncompleteSet& a, const IncompleteSet& b);
    static void link_put(IncompleteSet& a, const IncompleteSet& b,
                         const vector<char>& output, const vector<int>& output_index);

    void link_put(IncompleteSet& a, const vector<char>& output, const vector<int>& output_index);
    static void add(IncompleteSet& a, const IncompleteTransition& transition);

    void push(IncompleteTransition& transition, char symbol);
    void push(IncompleteSet& a, char symbol);
    void stack_replace(IncompleteSet& a, const vector<char>&input, const vector<char>& output);
    void move(IncompleteSet& a, const vector<int>& tape, int direction);
    void copy(IncompleteSet& a, unsigned int from_tape, unsigned int to_tape);
    void copy_any(IncompleteSet& a, unsigned int from_tape, unsigned int to_tape);
    void copy_till(IncompleteSet& a, const vector<char>&symbols, int check_tape, int from_tape, int to_tape, int direction, const vector<int>& affected);
    void link_on(IncompleteSet& a, const IncompleteSet& b, const vector<char>&input, const vector<int>& input_index);
    void link_on_not(IncompleteSet& a, const IncompleteSet& b, const vector<char>&input, const vector<int>& input_index);
    void link_on_sequence(IncompleteSet& a, const IncompleteSet& b, const vector<char>&input_sequence, int input_index);
    void link_on_multiple(IncompleteSet& a, const IncompleteSet& b, const vector<vector<char>>&input, const vector<int>& input_index);
    void clear_stack(IncompleteSet& a);
    void make_loop(IncompleteSet& a);
    void make_loop_on(IncompleteSet& a, char input, int input_index);
    void make_loop_on_sequence(IncompleteSet& a, const vector<char>& input_sequence, int input_index);
    string branch_on(IncompleteSet& a, const vector<char>&input, const vector<int>& input_index);
    void write_on(IncompleteSet& a, const vector<char>&input, const vector<int>& input_index,
                  const vector<char>&output, const vector<int>& output_index);

    void heap_push_definer(IncompleteSet& a, const vector<int>&tuple_indexes, bool function, bool has_bracket);
    void heap_push_working(IncompleteSet& a, bool function);

    void copy_to_working(IncompleteSet& a, const vector<int>&tuple_indexes, bool output= false);
    void clear_working(IncompleteSet& a);
    void check_split_nesting(IncompleteSet& a);

    static void reset();

    //this needs to become private in future
    void find_match_heap(IncompleteSet& a, char start_marker, char end_marker, int marker_tape, int data_tape, bool search_template=false);
    void find_match_heap_traverse(IncompleteSet& a, char start_marker, char end_marker, int marker_tape, int data_tape);

    //still needs to become private in future
    void skip_nesting(IncompleteSet& a, int new_stack_tape, int stack_direction, int skip_tape, int skip_direction, const vector<int>& affected, char i='{', char j='}');
    void makeAntiNesting(IncompleteSet& a, const vector<int>&tuple_indexes);
    void makeAntiNestingElse(IncompleteSet& a, const vector<int>&tuple_indexes);
    void makeAntiNestingIf(IncompleteSet& a, const vector<int>&tuple_indexes);
    void makeAntiNestingIfSolo(IncompleteSet& a, const vector<int>&tuple_indexes);
    void copyTupleRight(IncompleteSet& a, const vector<int>&tuple_indexes, char start_token, char end_token);
    void addToken(IncompleteSet& a, const vector<int>&tuple_indexes, const string& s, char ch, char replace='\u0000');

    void push_on_sequence(IncompleteSet& a, const vector<char>&input_sequence, int input_index, char push_char);
    void set_heap_mode(IncompleteSet& a, bool to_heap);

    void nesting_marker(IncompleteSet& a, const vector<int>&tuple_indexes, int split_nesting, int max_nesting);
    void make_working_nesting(IncompleteSet& a, const vector<int>&tuple_indexes);
    void mark_definer(IncompleteSet& a, const vector<int>&tuple_indexes);
    void add_nesting_working(IncompleteSet& a);
    void remove_nesting_working(IncompleteSet& a);
    void write_function_header(IncompleteSet& a, const vector<int>&tuple_indexes);
    void make_token(IncompleteSet& a, const vector<int>&tuple_indexes, char def_token, char replace='\u0000');
    void make_token_splitter(IncompleteSet& a, const vector<int>&tuple_indexes, char def_token, char splitter);
    void check_var_define_location(IncompleteSet& a, const vector<int>&tuple_indexes);
    void check_var_char_working(IncompleteSet& a);
    void clear_heap(IncompleteSet& a);
    void compare_single_tape(IncompleteSet& a, char s1, char e1, char s2, char e2, int marker_tape, int data_tape);
    char stack_symbol;
    vector<char> nesting_tokens;

    set<IncompleteTransition> mergeToSingle(const set<IncompleteTransition>& m);
    Transition make_transition(IncompleteTransition& incomp, int tapes);

    string check_stack_double(IncompleteSet& a);
    void store_param_count(IncompleteSet& a, const vector<int>&tuple_indexes);
private:

    TuringTools(unsigned int stack_tape);
    inline static unique_ptr<TuringTools> _instance;
    inline static bool _instance_flag;


    unsigned long goto_counter;
    unsigned long counter;
    unsigned int stack_tape;
    unsigned int branch_counter;

    bool heap_mode = false;

    char heap_sep = '@';
    char stack_sep = '?';
    int split_nesting = 2;


};



#endif //TOG_TURINGTOOLS_H