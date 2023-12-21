//
// Created by watson on 10/11/23.
//

#include <iostream>
#include <thread>
#include <ctime>
#include <filesystem>

static unsigned int core_amount = std::thread::hardware_concurrency(); // gets "core amount", in windows you can allocate infinite threads. In linux this isn't possible, I believe. so pls care about this.
// so whenever you thread something, pls change core_amount. Also core_amount isn't the perfect name for this

#include "src/Tokenisation.h"
#include "src/CFG.h"
#include "src/ThreadFunction.h"
#include "src/Config.h"
#include "src/CFGConstructor.h"
#include "src/LALR.h"
#include "src/GUI/GUI.h"

//!!!!!!!!!!!!!!!!!!! Default Config Location is already SET in Orchestrator.cpp
// Variabel Define might be a problem?

int main() { // Function names we create to replace nesting should have F or I in their names, so we know if they were for-loops or If-loops
    GUI g;

    Tokenisation tokenVector; // sometimes variables in a nesting that should be passed in a function call aren't passed because it isn't found in the source file, this is done on purpose.
    //std::string Filelocation="input/nestedExamples/engine.cc"; // for now, doesn't support double declarations like int a,d;
    std::string Filelocation="../test/nestedExamples/game5.cpp";
    //std::string Filelocation="../test/testFiles/TM_test_53.cpp";
    std::thread Tokenizer(&Tokenisation::Tokenize, &tokenVector, Filelocation); // i ignore rvalues in function calls
    core_amount--;
    //Tokenizer.join();

    Orchestrator();

    auto cfg = createCFG();
    cfg->toGNF();

    const CFG a = *cfg;
    LALR lalr(a);
    lalr.createTable();

    Tokenizer.join();
    core_amount++;

    //create LARL parser with tokenvector
    auto vec = tokenVector.getTokenVector();

    lalr.parse(vec);
    lalr.generate();
    std::ifstream File910("output/result.cpp");
    std::vector<std::string> V10;
    std::string C10;
    while(getline(File910,C10)){
        while(C10.substr(0,1)==" "){
            C10=C10.substr(1,std::string::npos);
        }
        V10.push_back(C10);
    }


    std::ofstream File1010("output/result.cpp");
    unsigned long int nestingcounter=0;
    std::string tab;
    for(const auto& it:V10){
        File1010 <<tab<< it <<std::endl;

        int add = std::count(it.begin(), it.end(), '{');
        int remove = std::count(it.begin(), it.end(), '}');

        for (int i = 0; i<add; i++){
            tab+="    ";
        }

        for (int i = 0; i<remove; i++){
            for (int j =0; j<4; j++){
                tab.pop_back();
            }
        }
    }
    File910.close();
    File1010.close();
    
    std::cout << "We do really love Tibo" << std::endl;
    std::string ResultFileLocation="output/result.cpp";
    ThreadFunction::threadFILE(ResultFileLocation);
    
    auto start1=std::chrono::high_resolution_clock::now();
    std::string SystemString= "g++ " + ResultFileLocation + "result.cc";
    system(SystemString.c_str());
    auto second1=std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(second1 - start1);

    cout << "normal: "<< duration1.count() << " microseconds" << endl;

    auto start=std::chrono::high_resolution_clock::now();
    const std::string SystemString2= "g++ " + ResultFileLocation;
    system(SystemString2.c_str());
    auto second=std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(second - start);

    cout << "threaded: "<< duration.count() << " microseconds" << endl;
    return 0;
}