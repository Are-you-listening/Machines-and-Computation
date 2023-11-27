//
// Created by watson on 11/19/23.
//

#include "CFGConstructor.h"

CFG* createCFG(){
    //Declare used variables
    vector<string> V{"S","B", "N"+to_string(Config::getConfig()->getMaxNesting()), "T"}; //S is StartSymbol, B off Bullshit in Between, Last variable Nx
    vector<pair< string, vector<string>>> P{ {"S", {"N0"} },  {"B",{}} , {"N"+to_string(Config::getConfig()->getMaxNesting()), {"[", "N1", "]"} }, {"N"+to_string(Config::getConfig()->getMaxNesting()), {"[", "B", "]"} }}; //Already define the simplest rule, epsilon rule, rule back to start

    //Define V names
    for(unsigned int i = 0 ; i<Config::getConfig()->getMaxNesting(); i++){
        V.push_back("N"+to_string(i));
        P.push_back({"N"+to_string(i), {"N"+to_string(i), "N"+to_string(i)}});
        P.push_back( {"N"+to_string(i), {"[", "N"+to_string(i+1), "]"} } ); // Add Nx -> { Nx+1 } - Speedup
        P.push_back( {"N"+to_string(i), {"[","B", "]"} } );
    }

    //Create other Productions
    for(auto &k: T){ //"Generic Programming"
        if (k == "[" || k == "]"){
            continue;
        }

        P.push_back( {"T",{k}}); //B van Bullshit // B -> B Terminal B
    }
    P.push_back({"B", {"T", "B"}});

    return new CFG(V,T,P,"S");
}
