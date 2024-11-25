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

#include "Config.h"
#include "src/CFG.h"

#include <queue>
#include <stack>
#include <algorithm>
#include <tuple>
#include <set>
#include <fstream>
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
     * Constructor
     * @param children
     * @param symbol
     * @param token
     */
    ParseTree(const vector<ParseTree *> &children, const string &symbol,
              const tuple<string, string, set<string>> &token);

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
    void findViolation(const unsigned long &max, const unsigned long &split, unsigned long &count, unsigned long &index,ParseTree* &Rviolator,const std::vector<std::string> &Terminals,bool &found,ParseTree* &function);

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

    /**
     * Get the Yield of the ParseTree
     * @param yield
     */
    void getYield(vector<tuple<string, string, set<string>>> &yield);

    void addTokens(vector<tuple<string, string, set<string>>>& tokens);

    /**
     * Recursively create the tokenSet of a certain Sub-Parsetree
     * @param tokenSet
     */
    void getTokenSet(set<std::string> &vSet, std::set<std::string> &dSet) const;

    /**
     * Helper function to collect Includes & Typedefs and add them in front
     * @param newKids
     */
    void cleanIncludeTypedefs(std::vector<ParseTree*> &newKids);
    
    /**
     * Checks if there is no break, continue or return
     */
    void checkBRC(pair<bool,int> &fDepth , pair<bool,int> &cbrDepth);

    /**
     * describes this node and its children (recursively) in the dot format
     */
    void generateDot(std::ostream& out);
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
    static ParseTree* functionCall(const string& code);

    /**
     * Helper function for generate(), creates a new function in place
     * @return , Parsetree* containing the new code
     */
    string function(ParseTree *violator, std::set<std::string> &tokenSet, const string &functionName,ParseTree* root) const;

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
     * Process a given tokenvector using the parse Table and create a parse Tree
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
    bool loadTable();

    void saveYield();
    string getYield();

    /**
     * creates an image of the parse tree using dot
     */
    void generateParseTreeImage(const string filename);
};

#endif//CFG_LALR_H
