//
// Created by tibov on 02/11/23.
//

#ifndef TOG_TURINGPRODUCTION_H
#define TOG_TURINGPRODUCTION_H

#include <vector>
#include <map>
#include <iostream>
#include <queue>
#include <memory>

using namespace std;

struct Production{
    vector<char> replace_val;
    vector<int> movement;
    string new_state;
    vector<int> control_increase = {};
    vector<int> increase_amount = {};
};
struct Transition;
class TuringProduction {
public:
    TuringProduction() = default;
    ~TuringProduction();
    void addRoute(queue<char>& symbols, Production&& p);
    Production getProduction(queue<char>& symbols);
    vector<Transition> traverse();

private:
    map<char, TuringProduction*> ptr_vector;
    Production production;
};

#endif //TOG_TURINGPRODUCTION_H
