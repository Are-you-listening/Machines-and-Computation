//
// Created by watson on 10/11/23.
//

#include "src/Tokenisation.h"
#include "src/CFG.h"
#include "src/ThreadFunction.h"
#include "src/CFGConstructor.h"
#include "src/LALR.h"
#include "src/GUI/GUI.h"

#include <iostream>
#include <thread>
#include <ctime>
#include <filesystem>
#include <string>

static unsigned int core_amount = std::thread::hardware_concurrency(); // gets "core amount", in windows you can allocate infinite threads. In linux this isn't possible, I believe. so pls care about this.
// so whenever you thread something, pls change core_amount. Also core_amount isn't the perfect name for this

int main() {
    //GUI g;

    Tokenisation tokenVector; // sometimes variables in a nesting that should be passed in a function call aren't passed because it isn't found in the source file, this is done on purpose.
    std::string Filelocation="../test/testFiles/TM_test_54.cpp";
    std::thread Tokenizer(&Tokenisation::Tokenize, &tokenVector, Filelocation); // i ignore rvalues in function calls
    core_amount--;
    //Tokenizer.join();

    Orchestrator("../test/testFiles/TM_test_54.cpp");

    auto cfg = createCFG();
    cfg->toGNF();

    const CFG a = *cfg;
    LALR lalr(a);
    lalr.createTable();

    Tokenizer.join();
    core_amount++;


    auto vec = tokenVector.getTokenVector(); //create LARL parser with tokenvector
    lalr.parse(vec);
    lalr.generate();

    Orchestrator::tabber();

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