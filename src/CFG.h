//
// Created by anass on 2-11-2023.
//

#ifndef TOG_CFG_H
#define TOG_CFG_H

#include <vector>
#include <cstring>
#include <tuple>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>

#include "lib/json.hpp"
#include "lib/kars/CFGKars.h"

using json = nlohmann::json;

class CFG {
private:
    bool table_made = false;
    std::map<string, std::set<string>> first_table;
    std::vector<std::string> V; //Variables
    std::vector<std::string> T;//Terminals
    std::vector<std::pair<std::string,std::vector<std::string>>> P; //Productions
    std::string S; //StartSymbol

    bool CNF = false;//Decides if it's in CNF
    bool GNF = false; //Decides if it's in GNF

    [[nodiscard]] CFGKars convert() const; //Convert to Kars' CFG
    
public:
    /**
     * Simple Default grammar, also see CFG.cpp
     */
    CFG();

    /**
     * Construct from json file
     * @param c , json file location
     */
    explicit CFG(const std::string & c);

    /**
     * Output the grammar data to console
     */
    void print();

    /**
     * Simple Constructor
     * @param v , variables
     * @param t , terminals
     * @param p , productions
     * @param s , startSymbol
     */
    CFG(const std::vector<std::string> &v, const std::vector<std::string> &t,
        const std::vector<std::pair<std::string, std::vector<std::string>>> &p, const std::string &s);

    /**
     * Convert the Grammar to Chomsky Normal Form
     */
    void toCNF();

    /**
     * Convert the current Grammar to Greibach Normal Form (GNF)
     */
    void toGNF();

    /**
     * Check for a given string if it's in the language of the grammar (CYK)
     * @param w
     * @return
     */
    [[nodiscard]] bool accepts(const std::string &w) const;

    /**
     * collect the terminals that start the _productions with head == input
     * we assume the cfg is in greibach normal form --> the terminals we want are always at the start of the rule body
     * @param input
     * @return
     */
    set<string> First(const string& input);

    const vector<std::string> &getV() const;

    const vector<std::string> &getT() const;

    void setT(const vector<std::string> &t);

    const vector<std::pair<std::string, std::vector<std::string>>> &getP() const;

    const string &getS() const;
    
    void setCnf(bool cnf);
};


#endif //TOG_CFG_H
