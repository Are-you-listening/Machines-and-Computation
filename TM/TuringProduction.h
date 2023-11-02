//
// Created by tibov on 02/11/23.
//

#ifndef TOG_TURINGPRODUCTION_H
#define TOG_TURINGPRODUCTION_H
#include <vector>
#include <map>
#include <iostream>
#include <queue>
using namespace std;

struct Production{
    vector<char> replace_val;
    vector<int> movement;
    string new_state;
};


class TuringProduction {
public:
    TuringProduction();
    void addRoute(queue<char>& symbols, Production&& p);
    Production getProduction(queue<char>& symbols);

private:
    map<char, TuringProduction*> ptr_vector;
    Production production;
};


#endif //TOG_TURINGPRODUCTION_H
