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

#include <queue>
#include <stack>
#include <algorithm>

#include "src/CFG.h"

#define augmentedrules set<tuple<string, vector<string>, set<string>>>

class LALR;

class state {
public:
    int _stateName;
    augmentedrules _productions;
    vector<pair<string, state*>> _connections; // outgoing _connections
    void createConnections(LALR &lalr);
    ~state();

    /**
     * <rule number in cfg, lookahead symbols>
     * when an ending rule is found (dot at the end of body) add it to this set
     */
    set<pair<int, set<string>>> endings;
};

class parseTree {
public:
    vector<parseTree*> children;
    string symbol;
    ~parseTree();

    parseTree();
    parseTree( vector<parseTree *> children,  string symbol);

    void traverse(const std::vector<std::string> &T , parseTree* _root, bool &V_root);
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
    parseTree* _root;

    void printTable();  // this function is mainly for debugging and is not needed for LALR parsing
    
    augmentedrules createAugmented(const tuple<string, vector<string>, set<string>> &inputrule);
    
    /**
     * Constructor
     * @param cfg , in GNF
     */
    LALR(const CFG &cfg);
    
    void createTable();
    
    state* findstate(const augmentedrules& rules);
    
    void printstates();
    
    void parse(std::vector<std::pair<std::string, std::string>> &input);

    /**
     * Clean up the parsetree so it has a usable format
     */
    void cleanUp();
};

#endif//CFG_LALR_H
