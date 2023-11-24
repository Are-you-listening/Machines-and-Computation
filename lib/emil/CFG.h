#ifndef CFG_H
#define CFG_H

#include "json.hpp"
#include <algorithm>
#include <bitset>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>
using namespace std;
using json = nlohmann::json;


string encaseincurly(const set<string> &input);
set<pair<string, string>> cartesianproduct(const set<string> &v1, const set<string> &v2);
void printTable(vector<vector<set<string>>> &table);

class CFG {
private:
    vector<string> _nonterminals;
    vector<string> _terminals;
    set<pair<string, vector<string>>> _productions;
    string _startSymbol;
    set<string> findruleforterminal(const char &terminal) const;
    set<string> findruleforvars(const pair<string, string> &vars) const;
    map<string, set<string>> findunitpairs(int &unitproduction_count);
    set<string> findnullables();
    set<string> findreachable();
    set<string> findgenerating();

    void removeepsilons(const set<string> &nullables);
    void removeunitpairs(const map<string, set<string>> &unitpairs);
    void removeuseless(set<string> &generating, set<string> &reachable);

    set<string> replaceterminals();
    int breakrules();

    string convertunitpairstostring(map<string, set<string>>& unitpairs);
public:
    CFG(const string &filename);

    void print();
    bool accepts(const string &input) const;
    void toCNF();
    const vector<string> &getNonterminals() const;
    const vector<string> &getTerminals() const;
    const set<pair<string, vector<string>>> &getProductions() const;
    const string &getStartSymbol() const;

    /**
     * collect the terminals that start the _productions with head == input
     * we assume the cfg is in greibach normal form --> the terminals we want are always at the start of the rule body
     * @param input
     * @return
     */
    set<string> First(const string& input);
};

#endif// CFG_H
