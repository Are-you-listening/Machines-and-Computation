//
// Created by anass on 14-10-2023.
//

#include "CFG.h"

CFG::CFG() {
    std::sort(V.begin(), V.end());
    std::sort(T.begin(), T.end());
    std::sort(P.begin(), P.end());
}

void CFG::print() {
    if(V.size()==1){
        std::cout << "V = {" << V[0] <<"}"<<std::endl;
    } else {
        std::cout << "V = {";
        for(auto it=V.begin(); it!=V.end()-1;it++){
            std::cout << *it <<", ";
        }
        std::cout << V[V.size()-1] <<"}" <<std::endl;
    }

    if(T.size()==1){
        std::cout << "T = {" << T[0] <<"}"<<std::endl;
    } else {
        std::cout << "T = {";
        for(auto it=T.begin(); it!=T.end()-1;it++){
            std::cout << *it <<", ";
        }
        std::cout << T[T.size()-1] <<"}" <<std::endl;
    }

    std::cout << "P = {" << std::endl;
    for(auto it=P.begin(); it!=P.end();it++){
        std::string c="`";
        if(it->second.empty()){
            c+="`";
        } else {
            for (auto it2 = it->second.begin(); it2 != it->second.end() - 1; it2++) {
                c += *it2 + " ";
            }
            c += it->second[it->second.size() - 1] + "`";
        }
        std::cout <<"   "<< it->first <<" -> " << c << std::endl;
    }
    std::cout << "}" << std::endl;

    std::cout <<"S = "<< S << std::endl;
}


CFG::CFG(const std::string& c) {
    REQUIRE(FileExists(c),"Can't find file!");

    std::ifstream f(c);
    json data = json::parse(f);

    std::vector<std::string> B1=data["Variables"];
    V=B1;

    std::vector<std::string> B2=data["Terminals"];
    T=B2;
    for(auto it=data["Productions"].begin(); it!=data["Productions"].end();it++){
        json data2=*it;
        std::string HEAD= data2["head"];
        std::vector<std::string> BODY =data2["body"];
        P.emplace_back(HEAD,BODY);
    }
    S=data["Start"];

    std::sort(V.begin(), V.end());
    std::sort(T.begin(), T.end());
    std::sort(P.begin(), P.end());

}

CFG::CFG(const std::vector<std::string> &v, const std::vector<std::string> &t,
         const std::vector<std::pair<std::string, std::vector<std::string>>> &p, const std::string &s) : V(v), T(t),
                                                                                                         P(p), S(s) {
    std::sort(V.begin(), V.end());
    std::sort(T.begin(), T.end());
    std::sort(P.begin(), P.end());
}

void sort(std::vector<std::pair<std::string,std::vector<std::string>>>& P){
    for(unsigned long int i=0; i<P.size()-1; i++){
        if(P[i].first==P[i+1].first&&P[i].second.size()<P[i+1].second.size()){
            bool swap=false;
            for(unsigned long int j=0; j<P[i].second.size(); j++){
                if(P[i].second[j]==P[i+1].second[j]){
                    swap=true;
                } else {
                    swap= false;
                }
            }
            if(swap){
                auto temp=P[i];
                P[i]=P[i+1];
                P[i+1]=temp;
                i=-1;
            }
        }
    }
}

void CFG::toCNF(){
    for(auto &it: P){
        it.second.emplace_back("`");
    }
    std::sort(V.begin(), V.end());
    std::sort(T.begin(), T.end());
    std::sort(P.begin(), P.end());
    for(auto &it: P){
        it.second.erase(it.second.end());
    }
    sort(P);

    std::cout << "Original CFG: " <<std::endl; // many couts, annoying for debug purposes.
    std::cout << std::endl;
    CFG::print();
    std::cout << std::endl;
    std::cout << "-------------------------------------" << std::endl;
    std::cout << std::endl;
    std::cout << " >> Eliminating epsilon productions" << std::endl;

    unsigned int originalSize=P.size();
    std::set<std::string> Nullables;
    for(auto &it: P){
        if(it.second.empty()){
            Nullables.emplace(it.first);
        }
    }
    for(auto &it2: P){ // can maybe be optimized
        if(it2.second.size()==1){
            for(auto it3=it2.second.begin(); it3!=it2.second.end(); it3++){
                for(auto it=Nullables.begin(); it!=Nullables.end(); it++){
                    if(*it==*it3){
                        Nullables.emplace(it2.first);
                        break;
                    }
                }
            }
        }
    }

    std::string c="{";
    for(auto &it: Nullables){
        c+=it+", ";
    }
    if(c.size()==1){
        c+="}";
    } else {
        c[c.size()-2]='}';
        c.erase(c.size()-1);
    }

    std::cout <<"  Nullables are " << c <<std::endl;

    for(unsigned int i=0; i<P.size(); i++){
        if(P[i].second.empty()){
            P.erase(P.begin()+i);
            i--;
        }
    }
    auto copy=P;
    for(auto &it2: copy){
        if(it2.second.size()>1){
            std::vector<int> Combination;
            int countOnes=0;
            for(auto &it3: it2.second){
                if(Nullables.find(it3)!=Nullables.end()){
                    Combination.push_back(1);
                    countOnes++;
                } else{
                    Combination.push_back(0);
                }
            }
            unsigned long int count=0;
            std::pair<std::string,std::vector<std::string>> temp2;
            temp2.first=it2.first;
            bool b= false;
            if(countOnes>1){
                b= true;
            }
            for(auto it4=Combination.begin(); it4!=Combination.end(); it4++, count++){
                std::pair<std::string,std::vector<std::string>> temp;
                temp.first=it2.first;
                if(*it4==1){
                    std::vector<std::string> Vtemp=it2.second;
                    Vtemp.erase(Vtemp.begin()+(int) count);
                    temp.second=Vtemp;
                    P.push_back(temp);
                }else{
                    temp2.second.push_back(it2.second[count]);
                }
            }
            if(b){
                P.push_back(temp2);
            }
        }
    }

    for(auto &it: P){
        it.second.emplace_back("`");
    }
    std::sort(V.begin(), V.end());
    std::sort(T.begin(), T.end());
    std::sort(P.begin(), P.end());
    for(auto &it: P){
        it.second.erase(it.second.end());
    }
    sort(P);


    std::cout << "  Created " << P.size() << " productions, original had " << originalSize << std::endl;
    std::cout<<std::endl;
    CFG::print();
    std::cout<<std::endl;
    std::cout << " >> Eliminating unit pairs" << std::endl;

    std::set<std::pair<std::string, std::string>> unitPairs;
    std::set<std::pair<std::string, std::string>> unitProductions;
    std::set<std::string> Vtemp;
    for(auto &it: V){
        Vtemp.emplace(it);
        unitPairs.emplace(it,it);
    }
    for(auto &it: P){
        if(it.second.size()==1&&Vtemp.find(it.second[0])!=Vtemp.end()){
            unitPairs.emplace(it.first,it.second[0]);
            std::string unit=it.second[0];
            unitProductions.emplace(it.first,it.second[0]);
            label:
            for(auto &it2: P){
                if(it2.first==unit){
                    if(it2.second.size()==1&&Vtemp.find(it2.second[0])!=Vtemp.end()&&!(it2.second[0]==it.first)){
                        unitPairs.emplace(it.first,it2.second[0]);
                        unit=it2.second[0];
                        goto label;
                    }
                }
            }
        }
    }
    std::string c1= "  Unit pairs: {";
    for(auto &it: unitPairs){
        c1+="("+it.first+", "+it.second+")"+", ";
    }
    if(c1.size()==1){
        c1+="}";
    } else {
        c1[c1.size()-2]='}';
        c1.erase(c1.size()-1);
    }

    std::cout << "  Found " << unitProductions.size() <<" unit productions" <<std::endl;
    std::cout << c1 << std::endl;

    originalSize=P.size();
    for(unsigned int i=0; i<P.size(); i++){
        if(P[i].second.size()==1&&Vtemp.find(P[i].second[0])!=Vtemp.end()){
            std::pair<std::string,std::vector<std::string>> temp2;
            temp2.first=P[i].first;
            bool a= true;
            std::string ctemp=P[i].second[0];
            for(unsigned int j=0;j<P.size(); j++){
                if(ctemp==P[j].first){
                    temp2.second=P[j].second;
                    if(std::find(P.begin(),P.end(),temp2)==P.end()){
                        if(a){
                            P[i]=temp2;
                            a=false;
                        } else {
                            P.push_back(temp2);
                        }
                    }
                }
                temp2.second.clear();
            }
        }
    }
    for(unsigned int i=0; i<P.size(); i++){
        if(P[i].second.size()==1&&Vtemp.find(P[i].second[0])!=Vtemp.end()){
            P.erase(P.begin()+i);
            i--;
        }
    }

    for(auto &it: P){
        it.second.emplace_back("`");
    }
    std::sort(V.begin(), V.end());
    std::sort(T.begin(), T.end());
    std::sort(P.begin(), P.end());
    sort(P);
    for(auto &it: P){
        it.second.erase(it.second.end());
    }
    sort(P);

    std::cout << "  Created " << P.size() << " new productions, original had " << originalSize << std::endl;
    std::cout << std::endl;
    CFG::print();
    std::cout<<std::endl;
    std::cout << " >> Eliminating useless symbols" << std::endl;
    std::set<std::string> Generating;
    std::set<std::string> Reachable;
    unsigned long int ProductionsORGsize=P.size();
    for(auto & it : P) {
        bool a1= true;
        for (auto & it2 : it.second) {
            bool a=false;
            for (auto & it3 : P) {
                if (it2 == it3.first||std::find(T.begin(),T.end(),it2)!=T.end()) {
                    a= true;
                }
            }
            if(!a){
                a1= false;
            }
        }
        if(a1){
            Generating.emplace(it.first);
        }
    }
    for(auto & it : P) {
        for(auto it2=it.second.begin(); it2!=it.second.end(); it2++){
            if(Generating.find(it.first)!=Generating.end()&&std::find(T.begin(),T.end(),*it2)!=T.end()){
                Generating.emplace(*it2);
            }
        }
    }

    for(unsigned int i=0; i<P.size(); i++){
        for (auto it2 = P[i].second.begin(); it2 != P[i].second.end(); it2++) {
            if(Generating.find(*it2)==Generating.end()){
                P.erase(P.begin()+i);
                i--;
                break;
            }
        }
    }
    Reachable.emplace(S);
    std::vector<std::string> symbols={S};
    while(!symbols.empty()){
        auto symbol = symbols[0];
        for(auto & it : P){
            if(it.first==symbol){
                for(auto it2=it.second.begin(); it2!=it.second.end(); it2++){
                    if(std::find(V.begin(), V.end(), *it2)!=V.end()&&it.first!=*it2&&Reachable.find(*it2)==Reachable.end()){
                        symbols.push_back(*it2);
                    }
                    Reachable.emplace(*it2);
                }
            }
        }
        symbols.erase(symbols.begin());
    }
    for(unsigned int i=0; i<P.size(); i++){
        if(Reachable.find(P[i].first)==Reachable.end()){
            P.erase(P.begin()+i);
            i--;
        }
    }
    unsigned long int VariablesORGsize=V.size();
    unsigned long int TerminalsORGsize=T.size();
    for(unsigned int i=0; i<V.size(); i++){
        if(Reachable.find(V[i])==Reachable.end()){
            V.erase(V.begin()+i);
            i--;
        }
    }
    for(unsigned int i=0; i<T.size(); i++){
        if(Reachable.find(T[i])==Reachable.end()){
            T.erase(T.begin()+i);
            i--;
        }
    }

    for(auto & it : P){
        it.second.emplace_back("`");
    }
    std::sort(V.begin(), V.end());
    std::sort(T.begin(), T.end());
    std::sort(P.begin(), P.end());
    for(auto & it : P){
        it.second.erase(it.second.end());
    }
    sort(P);


    std::string c2= "  Generating symbols: {";
    for(const auto & it : Generating){
        c2+=it+", ";
    }
    if(c2.size()==1){
        c2+="}";
    } else {
        c2[c2.size()-2]='}';
        c2.erase(c2.size()-1);
    }
    std::cout << c2 << std::endl;

    std::string c3= "  Reachable symbols: {";
    std::string c4= "  Useful symbols: {";
    for(const auto & it : Reachable){
        c3+=it+", ";
        c4+=it+", ";
    }
    if(c3.size()==1){
        c3+="}";
        c4+="}";
    } else {
        c3[c3.size()-2]='}';
        c3.erase(c3.size()-1);
        c4[c4.size()-2]='}';
        c4.erase(c4.size()-1);
    }
    std::cout << c3 << std::endl;
    std::cout << c4 << std::endl;
    std::cout << "  Removed " <<VariablesORGsize-V.size()<<" variables, " <<TerminalsORGsize-T.size()<< " terminals and " <<ProductionsORGsize-P.size()<<" productions" << std::endl;
    std::cout << std::endl;
    CFG::print();
    std::cout<<std::endl;
    std::cout << " >> Replacing terminals in bad bodies" << std::endl;
    unsigned long int VariableCount=0;
    unsigned long int Poriginal=P.size();
    std::vector<std::string> newVariables;
    for(unsigned int i=0; i<P.size(); i++){
        if(P[i].second.size()>1){
            for(auto it = P[i].second.begin(); it != P[i].second.end(); it++){
                if(std::find(T.begin(),T.end(),*it)!=T.end()){
                    bool a= true;
                    for(auto & it2 : P){
                        if(it2.second.size()==1&&std::find(it2.second.begin(), it2.second.end(), *it)!=it2.second.end()){
                            *it=it2.first;
                            a=false;
                        }
                    }
                    if(a){
                        bool b=true;
                        for(auto & it3 : P){
                            if(it3.second.size()==1&&*it==it3.second[0]){
                                *it=it3.first;
                                b=false;
                            }
                        }
                        if(b){
                            std::string tempChar="_"+*it;
                            V.push_back(tempChar);
                            std::pair<std::string,std::vector<std::string>> temp2;
                            temp2.first=tempChar;
                            temp2.second={*it};
                            *it=tempChar;
                            VariableCount++;
                            newVariables.push_back(tempChar);
                            P.push_back(temp2);
                        }
                    }
                }
            }
        }
    }

    for(auto & it : P){// technically only needed for the couts, last sort is needed
        it.second.emplace_back("`");
    }
    std::sort(V.begin(), V.end());
    std::sort(T.begin(), T.end());
    std::sort(P.begin(), P.end());
    for(auto & it : P){
        it.second.erase(it.second.end());
    }
    sort(P);

    std::string c5= "{";
    for(auto & newVariable : newVariables){
        c5+=newVariable+",";
    }
    if(c5.size()==1){
        c5+="}";
    } else {
        c5[c5.size()-1]='}';
    }

    std::cout << "  Added "<< VariableCount<<" new variables: " << c5 << std::endl;
    std::cout << "  Created "<< P.size() <<" new productions, original had "<<Poriginal <<std::endl;

    for(auto & it : P){
        it.second.emplace_back("`");
    }
    std::sort(V.begin(), V.end());
    std::sort(T.begin(), T.end());
    std::sort(P.begin(), P.end());
    for(auto & it : P){
        it.second.erase(it.second.end());
    }
    sort(P);

    std::cout << std::endl;
    CFG::print();
    std::cout<<std::endl;

    unsigned long int BrokeBodies=0;
    unsigned long int newerVariables=0;

    label2:
    for(unsigned int i=0; i<P.size(); i++){
        if(P[i].second.size()>2){
            std::pair<std::string,std::vector<std::string>> temp2;
            temp2.first=P[i].first;
            temp2.second.push_back(*P[i].second.begin());
            std::string tempChar=P[i].first;
            unsigned long int count=2;
            while(std::find(V.begin(), V.end(), tempChar)!=V.end()){
                tempChar.clear();
                tempChar=P[i].first+"_"+std::to_string(count);
                count++;
            }
            V.push_back(tempChar);
            newerVariables++;
            BrokeBodies++;
            temp2.second.push_back(tempChar);

            std::pair<std::string,std::vector<std::string>> temp3;
            temp3.first=tempChar;

            for(auto it=P[i].second.begin()+1; it!=P[i].second.end(); it++){
                temp3.second.push_back(*it);
            }
            P[i]=temp2;
            P.push_back(temp3);
            goto label2;
        }
    }

    for(auto & it : P){
        it.second.emplace_back("`");
    }
    std::sort(V.begin(), V.end());
    std::sort(T.begin(), T.end());
    std::sort(P.begin(), P.end());
    for(auto & it : P){
        it.second.erase(it.second.end());
    }
    sort(P);

    std::cout << " >> Broke " << BrokeBodies << " bodies, added " << newerVariables << " new variables" << std::endl;
    std::cout << ">>> Result CFG:" << std::endl;
    std::cout << std::endl;
    CFG::print();
}


void CFG::toGNF() { // I used the algorithm described by https://www.geeksforgeeks.org/converting-context-free-grammar-greibach-normal-form/
    // step 1
    if(!CNF){
        CFG::toCNF();
    }
    std::cout <<"Start GNF convert" << std::endl;
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
        for(auto & it : i.second){
            auto Variable= std::find(V.cbegin(),V.cend(),it);
            if(Variable!=V.cend()&&originals.find(it)==originals.cend()){
                originals[it]="A"+ std::to_string(n);
                it="A"+ std::to_string(n);
                n++;
            } else if(Variable!=V.cend()) {
                it=originals[it];
            }
        }
    }
    for(auto & it : V){
        it=originals[it];
    }
    S=originals[S];

    //step 3, I replace A3 also, on the website this isn't done for some reason. A4 → b | A2A3A4 | A4A4A4
    std::cout << "step 3" <<std::endl;
    for(long int m=0; m<P.size(); m++){ //Step 3.1 , replace j>i
        unsigned long int i= stoi(P[m].first.substr(1, std::string::npos));
        for(auto it=P[m].second.begin(); it!=P[m].second.end(); it++){
            if(std::find(T.cbegin(),T.cend(),*it)!=T.cend()||*it=="Z"){
                continue;
            }
            unsigned long int j=stoi((*it).substr(1, std::string::npos));
            if(i>j){ // A4 -> A1A4 , replace A1 met alle rules van A1
                //Find rules of A1
                vector<vector<string>> RHS; //{ {"A2","A3"} , {A4,A4} }
                for(const auto &rule :P){
                    if(rule.first=="A"+to_string(j)){
                        //Find Firsthalf
                        vector<string> firsthalf;
                        std::copy(P[m].second.begin(),it-1,back_inserter(firsthalf));
                        vector<string> secondhalf;
                        std::copy(it+1,P[m].second.end(),back_inserter(secondhalf));
                        for(const auto & it2: rule.second){ //Add RHS in between
                            firsthalf.push_back(it2);
                        }
                        for(const auto & it2: secondhalf){ //Add secondhalf
                            firsthalf.push_back(it2);
                        }
                        RHS.push_back(firsthalf); //Firshalf is now complete, add it to RHS
                    }
                }
                //Replace A1 met RHS
                bool first=true;
                for(const auto & rhsit: RHS){
                    if(first){
                        P[m].second=rhsit;
                        first=false;
                    } else {
                        P.emplace_back(P[m].first,rhsit);   
                    }
                }
                break;
            }
        }
    }
    
    
    
    for(long int m=0; m<P.size(); m++) { //Step 3.2, replace i==j
        if (P[m].first == "Z") {
            continue;
        }
        unsigned long int i = stoi(P[m].first.substr(1, std::string::npos));
        for (auto it = P[m].second.begin(); it != P[m].second.end(); it++) {
            if (std::find(T.cbegin(), T.cend(), *it) != T.cend() || *it == "Z") {
                continue;
            }
            unsigned long int j=stoi((*it).substr(1, std::string::npos));
            if(i==j){
                
            }
        }
    }

    //step 4
    std::cout << "step 4" <<std::endl;
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

bool CFG::accepts(const string &w) const {
    return convert().accepts(w); //Construct CFG & Check Membership
}

CFGKars CFG::convert() const {
    map<string,vector<vector<string>>> P2; //Convert P to Kars' P=P2
    for(auto &tup: P){
        P2[tup.first].push_back(tup.second);
    }

    return CFGKars{V,T,P2,S}; //Construct CFG
}

set<string> CFG::First(const string &input) {
    set<string> result;
    for (const auto& rule : P){
        if (rule.second.empty()){   // the empty string has no useful information for the parser, so we ignore it (left production side)
            continue;
        }
        if (rule.first == input){
            result.emplace(rule.second[0]); //P -> A , A wordt meegegeven, format: {"P",{"A"}})
        }
    }
    return result;
}

const vector<std::string> &CFG::getV() const {
    return V;
}

const vector<std::string> &CFG::getT() const {
    return T;
}

const vector<std::pair<std::string, std::vector<std::string>>> &CFG::getP() const {
    return P;
}

const string &CFG::getS() const {
    return S;
}

void CFG::setCnf(bool cnf) {
    CNF = cnf;
}

