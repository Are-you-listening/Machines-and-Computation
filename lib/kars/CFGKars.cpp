//
// Created by watson on 10/6/23.
//

#include "CFGKars.h"

CFGKars::CFGKars() {
    V = {"BINDIGIT", "S"};
    S = "S";
    T = {"0", "1", "a", "b"};
    P.insert({  "BINDIGIT", { {"0"}, {"1"} }  } );
    P.insert( { "S",  {  {""}, {"a","S","b","BINDIGIT"} }  });
}

void CFGKars::print() const {
    printVector("V", V);
    printVector("T", T);

    //Generate correct string for all the productions to print
    cout << "P = { \n";
    for (auto &it: P){
        for (auto rule: it.second){
            cout << "       "+it.first + " -> `";
            for (int vt = 0; vt<rule.size()-1; vt++) {
                cout << rule[vt] + " ";
            }
            cout << rule[rule.size()-1]+"`" << endl;
        }
    }
    cout << "}" << endl;

    //S
    cout << "S = " << S << endl;
}

CFGKars::CFGKars(const string &file) {
    REQUIRE(FileExists(file), "No file found to load");
    //Read File
    ifstream input(file);
    json j;
    input >> j;

    //Load V
    for(auto &item: j["Variables"]){
        V.push_back(item);
    }

    //Load T
    for(auto &item: j["Terminals"]){
        T.push_back(item);
    }

    //Load P
    for(auto rule: j["Productions"]){
        string head = rule["head"];
        vector<string> body = rule["body"];
        if(P.find(head)==P.end()){
            P[head]={};
        }
        if(body.empty()){
            body.emplace_back("");
        }
        P[head].push_back(body);
    }


    //Load S
    S = j["Start"];
}

CFGKars::CFGKars(const vector<string> &v, const vector<string> &t, const map<string, vector<vector<string>>> &p,
         const string &s) : V(v), T(t), P(p), S(s) {}

bool CFGKars::accepts(const string &w) const {
    vector<vector<set<string>>> table;

    //Set up first row
    vector<set<string>> temp;
    for(const char &c: w){
        temp.push_back(ReverseFindP(string(1,c))); //Add First Value's
    }
    table.push_back(temp);
    temp.clear();

    int i = 1;
    while(i<=w.length()){
        vector<set<string>> row;
        for(int j = 0; j<w.length()-i; ++j){
            set<string> cel; //{} or {C} or {B,C}
            pair<int,int> c{0,j}; //Column wise
            pair<int,int> d{i-1,j+1}; //Diagonally seen

            //Collect all options
            //
            set<vector<string>> options;
            while(d.first>-1 && d.second<=w.length() && c.first<i){
                auto temp2 = Cartesian(table[c.first][c.second], table[d.first][d.second]);
                Merge(options, temp2);


                ++d.second;
                --d.first;
                ++c.first;
            }

            //Lookup Variables
            for(auto &s: options){
                set<string> temporary = ReverseFindP(s);
                cel.insert(temporary.begin(), temporary.end());
            }

            //Place Result in row
            row.push_back(cel);
        }
        table.push_back(row); //Place row in table
        ++i;
    }
    printVVSS(table);

    for(auto &entry: table[w.length()-1][0]){
        if(entry==S){
            cout << "true" << endl;
            return true;
        }
    }
    cout << "false" << endl;
    return false;
}

set<string> CFGKars::ReverseFindP(const string &c) const {
    const vector<string> temp = {c};
    set<string> possible;

    for(auto &tup: P){
        for(auto &vec: tup.second ){
            if(temp==vec){
                possible.insert(tup.first);
            }
        }
    }
    return possible;
}

set<string> CFGKars::ReverseFindP(const vector<string> &tocheck) const {
    set<string> possible;

    for(auto &tup: P){
        for(auto &vec: tup.second ){
            if(vec==tocheck){
                possible.insert(tup.first);
            }
        }
    }
    return possible;
}
