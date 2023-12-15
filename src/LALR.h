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
#include <fstream>

#include "Config.h"
#include "src/CFG.h"
#include <exception>
#include <filesystem>

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
    string symbol; //Abstract symbol used in the Grammar
    tuple<string, string, set<string>> token; //Translation
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
     * Find the leftmost or rightmost bracket
     * @param left, toggle between leftmost or rightmost
     * @param data, { root, bracket-index, depth, found }
     * @param T , Terminals
     */
    void findBracket(bool left, std::tuple<ParseTree *, unsigned long, unsigned long,bool> &data, const std::vector<std::string> &Terminals);

    /**
     * Check for maxNesting violations
     * @param max, max allowed nesting
     * @param count, counter for the nr of nesting
     * @param index, Rviolator.children[index] == violator
     * @param Rviolator , the root of the violating nesting
     * @param T , Terminals
     */
    void findViolation(unsigned long &max, unsigned long &count, unsigned long &index,ParseTree* &Rviolator,const std::vector<std::string> &Terminals);

    /**
     * Find the root of a given child
     * @param child
     * @param T , Terminals
     * @return
     */
    ParseTree* findRoot(ParseTree* &child,const std::vector<std::string> &Terminals);

    /**
     * Format the parsetree so each pair of brackets matches itsself on the same node-level
     * @param T , all the used Terminals
     */
    void matchBrackets(const std::vector<std::string> &Terminals);

    void getYield(vector<tuple<string, string, set<string>>> &yield);

    void addTokens(vector<tuple<string, string, set<string>>>& tokens);

    /**
     * Recursively create the tokenSet of a certain Sub-Parsetree
     * @param tokenSet
     */
    void getTokenSet(std::set<std::set<std::string>> &tokenSet) const;
};

/**
 * For the whole LALR process we assume the given cfg is in Greibach normal form
 **/
class LALR {
    State* I0;
    void createStates();   // creates I0, I1, ...
    set<State *> findSimilar(const set<tuple<string, vector<string>, set<string>>> &rules);
    void mergeSimilar();

    /**
     * Helper function for generate(), creates a new function Call in place
     * @return , Parsetree* containing the new code
     */
    ParseTree* functionCall(const string& code);

    /**
     * Helper function for generate(), creates a new function in place
     * @return , Parsetree* containing the new code
     */
    string function(ParseTree *violator, std::set<std::set<std::string>> &tokenSet, const string functionName);

public:
    unordered_map<int, map<string, string>> parseTable;
    CFG _cfg;
    int state_counter = 0;
    ParseTree* _root;

    /**
     * Debug function to print out the created LALR Table
     * CAVEAT: Not needed for LALR parsing
     */
    void printTable();

    /**
     *
     * @param inputrule
     * @return
     */
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

    /**
     * Find a state
     * @param rules
     * @return
     */
    State* findState(const augmentedrules& rules);

    /**
     * Print function
     */
    void printStates();

    /**
     * Process a given tokenvector and create a parseTable
     * @param input , tokenvector
     */
    void parse(std::vector<std::tuple<std::string, std::string, std::set<std::string>>> &input);

    /**
     * Manipulates the parsetree to decrease the amount of nesting
     */
    void generate();

    /**
     * Saves the parseTable to a file named "parseTablefile.txt"
     */
    void saveTable();

    /**
     * Loads the parseTable from "parseTablefile.txt" if this file exists
     */
    void loadTable();
};

#endif//CFG_LALR_H
