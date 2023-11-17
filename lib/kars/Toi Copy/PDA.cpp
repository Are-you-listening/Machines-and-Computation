//
// Created by watson on 10/26/23.
//

#include "PDA.h"

PDA::PDA(const string &file) {
    REQUIRE(FileExists(file), "No file found to load");
    //Read File
    ifstream input(file);
    json j;
    input >> j;

    string q = j["StartState"];
    for(auto &item: j["States"]){
        Q.insert(std::make_shared<State> (item));
        if(item==q){
            q0 = *(--Q.end());
        }
    }

    for(auto &item: j["Alphabet"]){
        Alphabet.push_back(item);
    }

    for(auto &item: j["StackAlphabet"]){
        StackAlphabet.push_back(item);
    }

    for(auto &rule: j["Transitions"]){
        string from = rule["from"];
        string inputsymbol = rule["input"];
        string stacktop = rule["stacktop"];
        string to = rule["to"];
        vector<string> replacement = rule["replacement"];

        if(replacement.empty()){
            replacement.push_back("");
        }

        shared_ptr<State> fState;
        weak_ptr<State> tState;

        for(auto &State: Q){ //Can use optimisation
            if(State->getName()==from){
                fState=State;
            }
            if(State->getName()==to){
                tState=State;
            }
        }
        fState->AddTransition(inputsymbol,stacktop,tState,replacement);
    }

    z0 = j["StartStack"];
    Stack.push(z0);
}

CFG PDA::toCFG() const {
    vector<string> QString;
    vector<string> V{"S"}; //Variables + StartSymbol
    vector<vector<string>> var;
    map<string,vector<vector<string> > > P; //Productions:  { Variable , {RULES} } | RULE = {Terminals,Variables,..}

    //Generate Variables
    for(const string &t: StackAlphabet){
        for(auto &State: Q){
            for(auto &State2: Q){
                V.push_back("["+State2->getName()+","+t+","+State->getName()+"]");
                //var.push_back( {State2->getName(),t,State->getName()} ); //Keep track of each single variable for Productions
            }
        }
    }

    //Create Productions

    //Add S rules
    for(auto &p: Q){
        QString.push_back(p->getName()); //Collect StateNames
        P["S"].push_back({"["+q0->getName()+","+z0+","+p->getName()+"]"}); // S -> [q0Z0p]
    }

    for(auto &State: Q){
        auto delta = State->getDelta();
        for(auto &ruleset: delta){
            string stacktop = ruleset.first.second;
            string input = ruleset.first.first;
            for(auto &rule: ruleset.second){
                vector<vector<string>> result; //Options while be placed in here
                vector<string> current; //Temp variable

                makeCombinations(QString, rule.second.size()-1, current, result); //Generate All Options in result

                current.clear();

                pair< vector<vector<string>> , vector<vector<string> >  > temp = GenerateStateNames(rule.first.lock()->getName(), input,rule.second, result); //State Going To + Terminal + StackSymbolSSS (Next to "State Going To") + Results
                for(auto &S2: Q){ //Add Other (Bigger) Rules
                    for(vector<string> i: temp.second){ //i = stateoptions in the middle = {q,p,r}
                        //Remember we still have to change the last state name!
                        auto ktemp = i[i.size()-1];
                        ktemp.append(S2->getName()+"]");
                        i.pop_back();
                        i.push_back(ktemp);

                        //Add it
                        P["["+State->getName()+","+stacktop +","+S2->getName()+"]"].push_back( i );
                    }
                }
                temp.second.clear();
                for(vector<string> &i: temp.first){ //Add Terminal Rules
                    P["["+State->getName()+","+stacktop +","+rule.first.lock()->getName()+"]"].push_back( i );
                }
            }
        }
    }
    std::sort(V.begin(), V.end());
    return {V,Alphabet,P,"S"};
}

