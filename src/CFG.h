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
#include "lib/json.hpp"
#include <iostream>
#include <set>

#include "lib/kars/CFGKars.h"

using json = nlohmann::json;

class CFG {
private:
    std::vector<std::string> V;
    std::vector<std::string> T;
    std::vector<std::pair<std::string,std::vector<std::string>>> P;
    std::string S;
    bool CNF = false;
    bool GNF = false;
    
public:
    CFG();
    explicit CFG(const std::string & c);
    void print();
    CFG(const std::vector<std::string> &v, const std::vector<std::string> &t,
        const std::vector<std::pair<std::string, std::vector<std::string>>> &p, const std::string &s);
    void toCNF();
    void toGNF();

    [[nodiscard]] bool accepts(const std::string &w) const;

};


#endif //TOG_CFG_H
