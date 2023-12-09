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
#include <tuple>
#include <set>

#include "Config.h"
#include "src/CFG.h"

typedef set<tuple<string, vector<string>, set<string>>> augmentedrules; //Use typedef to simplify usage

class LALR;

class State {
public:
    int _stateName;
    augmentedrules _productions;
    vector<pair<string, State*>> _connections; // outgoing _connections

    void createConnections(LALR &lalr);

    /**
     * Destructor to free used memory
     */
    ~State();

    /**
     * <rule number in cfg, lookahead symbols>
     * when an ending rule is found (dot at the end of body) add it to this set
     */
    set<pair<int, set<string>>> endings;
};

class ParseTree {
public:
    vector<ParseTree*> children;
    string symbol;

    /**
     * Destructor to free used memory
     */
    ~ParseTree();

    /**
     * Simple Constructor
     */
    ParseTree() = default;

    /**
     * Full Constructor
     * @param children
     * @param symbol
     */
    ParseTree(const vector<ParseTree *> &children, string symbol);

    /**
     * Traverse the parse tree & cleanup
     * @param T
     * @param _root
     * @param V_root
     */
    void traverse(const std::vector<std::string> &T , ParseTree* _root, bool &V_root);

    void findBracket(bool left,std::tuple<ParseTree *, ParseTree *, unsigned long,bool> &data); // { _root, bracket  , depth }
};

/**
 * For the whole LALR process we assume the given cfg is in Greibach normal form
 **/
class LALR {
    State* I0;
    void createStates();   // creates I0, I1, ...
    set<State *> findSimilar(const set<tuple<string, vector<string>, set<string>>> &rules);
    void mergeSimilar();

public:
    unordered_map<int, map<string, string>> parseTable;
    CFG _cfg;
    int state_counter = 0;
    ParseTree* _root;

    void printTable();  // this function is mainly for debugging and is not needed for LALR parsing

    augmentedrules createAugmented(const tuple<string, vector<string>, set<string>> &inputrule);

    /**
     * Constructor
     * @param cfg , in GNF
     */
    explicit LALR(const CFG &cfg);

    /**
     * Setup the parse table
     */
    void createTable();

    State* findState(const augmentedrules& rules);

    void printStates();

    void parse(std::vector<std::tuple<std::string, std::string, std::set<std::string>>> &input);

    void parse(std::vector<std::pair<std::string, std::string>> &input);

    /**
     * Clean up the parse tree so it has a usable format
     */
    void cleanUp() const;

    void move();
};

#endif//CFG_LALR_H
