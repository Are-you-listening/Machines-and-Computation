//
// Created by watson on 11/19/23.
//

#include "CFGConstructor.h"

CFG* createCFG(){
    //Declare used variables
    vector<string> V{"S"}; //S is StartSymbol
    vector<pair< string, vector<string>>> P{ {"S", {"N0"} } }; //Already define the simplest rule

    //Define V names
    for(unsigned int i = 0 ; i<=Config::getConfig()->getMaxNesting(); i++){
        V.push_back("N"+to_string(i));
        P.push_back( {V[i] , {"{","N"+to_string(i+1),"}"} } ); // Add Nx -> { Nx+1 } - Speedup
    }

    //Create Productions

    //"Need all options" - make notes

    return new CFG(V,T,P,"S");
}
