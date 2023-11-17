//
// Created by watson on 10/26/23.
//

#ifndef TOI_PDA_H
#define TOI_PDA_H

#include <string>
#include <vector>
#include <stack>
#include <map>
#include <set>
#include <fstream>
#include <iomanip>
#include <memory>
#include <algorithm>

#include "helper.h"
#include "DesignByContract.h"
#include "json.hpp"
#include "State.h"
#include "CFG.h"

using namespace std;
using json = nlohmann::json;

class CFG;

class PDA {
private:
    shared_ptr<State> q0;
    string z0;

    set<shared_ptr<State>> Q;
    vector<string> Alphabet;
    vector<string> StackAlphabet;

    stack<string> Stack;
public:
    explicit PDA(const string &file);

    [[nodiscard]] CFG toCFG() const;
};


#endif //TOI_PDA_H
