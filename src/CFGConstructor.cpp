//
// Created by watson on 11/19/23.
//

#include "CFGConstructor.h"

CFG* createCFG(){
    //Declare used variables
    vector<string> V{"S","B", "N"+to_string(Config::getConfig()->getMaxNesting())}; //S is StartSymbol, B off Bullshit in Between, Last variable Nx
    vector<pair< string, vector<string>>> P{ {"S", {"B","N0","B"} },  {"B",{""}} , {"N"+to_string(Config::getConfig()->getMaxNesting()), {"B","N0","B"} }}; //Already define the simplest rule, epsilon rule, rule back to start

    //Define V names
    for(unsigned int i = 0 ; i<Config::getConfig()->getMaxNesting(); i++){
        V.push_back("N"+to_string(i));
        P.push_back( {"N"+to_string(i), {"B","N"+to_string(i+1),"B"} } ); // Add Nx -> { Nx+1 } - Speedup
    }

    //Create other Productions
    for(auto &k: T){ //"Generic Programming"
        P.push_back( {"B",{"B",k,"B"}}); //B van Bullshit // B -> B Terminal B
    }

    return new CFG(V,T,P,"S");
}
