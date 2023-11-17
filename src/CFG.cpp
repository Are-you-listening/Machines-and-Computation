//
// Created by anass on 2-11-2023.
//

#include "src/CFG.h"



void CFG::toGNF() { // I used the algorithm described by https://www.geeksforgeeks.org/converting-context-free-grammar-greibach-normal-form/
    // step 1
    if(!CNF){
        CFG::toCNF();
    }
    // step 2
    std::map<std::string,std::string> originals;
    unsigned long int n=1;
    for(auto & i : P){
        if(originals.find(i.first)!=originals.cend()){
            i.first=originals[i.first];
        }
        if(i.first.size()==1||i.first[0]!='A'){
            originals[i.first]="A"+ std::to_string(n);
            i.first="A"+ std::to_string(n);
            n++;
        }
        for(auto it=i.second.begin(); it!=i.second.end(); it++){
            auto Variable= std::find(V.cbegin(),V.cend(),*it);
            if(Variable!=V.cend()&&originals.find(*it)==originals.cend()){
                originals[*it]="A"+ std::to_string(n);
                *it="A"+ std::to_string(n);
                n++;
            } else if(Variable!=V.cend()) {
                *it=originals[*it];
            }
        }
    }
    for(auto & it : V){
        it=originals[it];
    }
    S=originals[S];

    //step 3, I replace A3 also, on the website this isn't done for some reason. A4 → b | A2A3A4 | A4A4A4
    for(long int m=0; m<P.size(); m++){
        if(P[m].first=="Z"){
            continue;
        }
        unsigned long int i= stoi(P[m].first.substr(1, std::string::npos));
        for(auto it=P[m].second.begin(); it!=P[m].second.end(); it++){
            if(std::find(T.cbegin(),T.cend(),*it)!=T.cend()||*it=="Z"){
                continue;
            }
            unsigned long int j=stoi((*it).substr(1, std::string::npos));
            if(i>j){
                bool first= true;
                for(auto & rules2 : P){
                    if(first&&*it==rules2.first){
                        std::string temp;
                        for(const auto& it3 :rules2.second){
                            temp+=it3;
                        }
                        *it=temp;
                        first= false;
                    } else if(*it==rules2.first) {
                        bool second=false;
                        std::pair<std::string,std::vector<std::string>> temp;
                        temp.first=P[m].first;
                        temp.second=P[m].second;
                        for(auto & it2 : temp.second){
                            if(it2==rules2.first){
                                if(second){
                                    std::string c;
                                    for(const auto& it3 :rules2.second){
                                        c+=it3;
                                    }
                                    it2=c;
                                }else{
                                    second= true;
                                }
                            }
                        }
                        P.push_back(temp);
                    }
                }
            } else if(i==j) {
                auto it2=std::find(it,P[m].second.end(),*it);
                while(true){
                    if(std::find(it2+1,P[m].second.end(),*it)!=P[m].second.end()){
                        it2=std::find(it2+1,P[m].second.end(),*it);
                    } else {
                        break;
                    }
                }
                std::pair<std::string,std::vector<std::string>> temp;
                temp.first="Z";
                std::vector<std::string> vcopy =P[m].second;
                vcopy.erase(vcopy.begin()+std::distance(P[m].second.begin(),it2));
                temp.second=vcopy;
                P.push_back(temp);
                temp.second.push_back(temp.first);
                P.push_back(temp);
                unsigned long int originalSize=P.size();
                for(unsigned long int d=0; d<originalSize; d++){
                    if(P[m].first==P[d].first&&P[d].second!=P[m].second){
                        std::pair<std::string,std::vector<std::string>> temp2;
                        temp2.first=P[m].first;
                        temp2.second=P[d].second;
                        temp2.second.emplace_back("Z");
                        P.push_back(temp2);
                    }
                }
                P.erase(P.begin()+m);
                m=-1;
                break;
            }
        }
    }

    //step 4
    for(auto & rules : P){
        for(auto & it:rules.second){
            if(std::find(T.cbegin(),T.cend(),it)!=T.cend()){
                for(auto & rules2 : P){
                    if(rules2.second.size()==1&&rules2.second[0]==it&&rules.first!=rules2.first){
                        it=rules2.first;
                    }
                }
            }
        }
    }
    GNF= true;
}
