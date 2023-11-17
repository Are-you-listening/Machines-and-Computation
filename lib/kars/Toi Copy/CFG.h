//
// Created by watson on 10/6/23.
//

#ifndef TOI_CFG_H
#define TOI_CFG_H

#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <fstream>
#include <iomanip>
#include <set>

#include "DesignByContract.h"
#include "json.hpp"
#include "helper.h"

using namespace std;
using json = nlohmann::json;

class CFG {
private:
    vector<string> V; //Variables
    vector<string> T; //Terminals
    map<string,vector<vector<string> > > P; //Productions:  { Variable , {RULES} } | RULE = {Terminals,Variables,..}
    string S; //Start Symbol

    /**
     * Find the head of all productions-body's containing c
     * @param c
     * @return
     */
    [[nodiscard]] set<string> ReverseFindP(const string &c) const;

    /**
     * Find the head of all productions-body's containing the elements of c
     * @param c
     * @return
     */
    [[nodiscard]] set<string> ReverseFindP(const vector<string> &c) const;

public:
    CFG();

    CFG(const string &file);

    CFG(const vector<string> &v, const vector<string> &t, const map<string, vector<vector<string>>> &p,
        const string &s);

    [[nodiscard]] bool accepts(const string &w) const;

    void print() const;
};


#endif //TOI_CFG_H
