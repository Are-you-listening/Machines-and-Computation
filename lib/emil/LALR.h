//
// Created by emill on 20/11/2023.
//

/*
 * LALR parser steps
 * 1) create augmented grammar
 * 2) find LR(0) collection of items
 * 3) create CLR parsing table
 * 4) merge similar states in parsing table (states that can only differ in the look-ahead)
 * 5) remove unwanted rows in parsing table
 */

#ifndef CFG_LALR_H
#define CFG_LALR_H

#include "CFG.h"
#include <queue>
#include <stack>

#define augmentedrules set<tuple<string, vector<string>, set<string>>>

class LALR;

class state {
public:
    int _stateName;
    augmentedrules _productions;
    vector<pair<string, state*>> _connections; // outgoing _connections
    void createConnections(LALR &lalr);
    ~state();
    bool isendingstate = false;
    int cfgrulenumber = NULL;
};

class parseTree {
public:
    vector<parseTree*> children;
    string symbol;
    ~parseTree();
};

/*
 * For the whole LALR process we assume the given cfg is in Greibach normal form
 */
class LALR {
    state* I0;
    void createStates();   // creates I0, I1, ...
    set<state *> findSimilar(const set<tuple<string, vector<string>, set<string>>> &rules);
    void mergeSimilar();
public:
    unordered_map<int, map<string, string>> parseTable;
    CFG _cfg;
    int state_counter = 0;
    parseTree*_root;
    augmentedrules createAugmented(const tuple<string, vector<string>, set<string>> &inputrule);
    LALR(const CFG &cfg);
    void createTable();
    state* findstate(const augmentedrules& rules);
    void printstates();
    void parse(string inputstring);
};

#endif//CFG_LALR_H
