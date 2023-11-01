//
// Created by watson on 10/11/23.
//

#include <iostream>
#include <thread>
#include <ctime>
#include "Tokenazation.h"

static unsigned int core_amount = std::thread::hardware_concurrency(); // gets "core amount", in windows you can allocate infinite threads. in linux this isn't possible i believe. so pls care about this.

int main() {
    Tokenazation tokenVector;
    std::string Filelocation="Nested-code/engine.cc";
    std::thread Tokenizer(&Tokenazation::Tokenize, &tokenVector, Filelocation);
    //construct CFG here and convert to CNF and lather to GNF
    Tokenizer.join();
    //create larl parser with tokenvector
    std::cout << "We do really love Tibo" << std::endl;
    return 0;
}