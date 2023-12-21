//
// Created by watson on 10/11/23.
//

#include "src/Tokenisation.h"
#include "src/CFG.h"
#include "src/CFGConstructor.h"
#include "src/LALR.h"
#include "src/GUI/GUI.h"

#include <thread>
#include <string>

static unsigned int core_amount = std::thread::hardware_concurrency(); // Gets "core amount",
// in windows you can allocate infinite threads.
// In linux this isn't possible, I believe.
// So please care about this.

int main() {
    //GUI g;

    //Create tokenVector in parallel
    Tokenisation tokenVector; // sometimes variables in a nesting that should be passed in a function call aren't passed because it isn't found in the source file, this is done on purpose.
    std::string Filelocation="../test/testFiles/TM_test_54.cpp";
    std::thread Tokenizer(&Tokenisation::Tokenize, &tokenVector, Filelocation); // i ignore rvalues in function calls
    core_amount--;

    //Construct GNF in parallel
    Orchestrator("../test/testFiles/TM_test_54.cpp");
    auto cfg = createCFG();
    cfg->toGNF();
    const CFG a = *cfg;
    LALR lalr(a);
    lalr.createTable();

    Tokenizer.join(); core_amount++; //Join both operations

    //create LARL parser with tokenvector
    auto vec = tokenVector.getTokenVector();
    lalr.parse(vec);
    lalr.generate();
    Orchestrator::tabber(); //Cleanup output file; match tabs & spaces
    Orchestrator::threadingTest(); //Perform a speed test

    return 0;
}