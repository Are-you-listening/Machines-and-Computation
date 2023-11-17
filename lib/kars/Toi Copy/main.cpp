#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_set>
#include <set>

#include "json.hpp"
#include "CFG.h"
#include "PDA.h"
#include "helper.h"

using namespace std;
using json = nlohmann::json;

int main() {
    CFG cfg("input-cyk1.json");

    pair<int,int> c{-1,0}; //Collum wise
    pair<int,int> d{0+1,1-1}; //Diagonally seen

    vector<string> A ={"B","C"};
    vector<string> B = {};

    set<string> k;
    k.insert("e");
    //k.insert("e");
    vector<string> t{"a"};

    //cout << SubsetCheck(t,k) << endl;


    cout << cfg.accepts("baaba") << endl; //moet true geven
    cout <<  cfg.accepts("abba")<< endl; //moet false geven
    return 0;
}
