//
// Created by watson on 10/11/23.
//

#include <iostream>
#include <thread>
#include <ctime>
#include "Tokenazation.h"
#include "CFG.h"

static unsigned int core_amount = std::thread::hardware_concurrency(); // gets "core amount", in windows you can allocate infinite threads. in linux this isn't possible i believe. so pls care about this.
// so whenever you thread something, pls change core_amount. Also core_amount isn't the perfect name for this

int main() {
    Tokenazation tokenVector;
    std::string Filelocation="Nested/engine.cc";
    std::thread Tokenizer(&Tokenazation::Tokenize, &tokenVector, Filelocation);
    core_amount--;
    
    CFG cfg("TestFiles/CFG.json"); //don't forget to add CFG back into cmakelist in right set
    cfg.toGNF(); // this still needs massive debugging. 
    
    Tokenizer.join();
    core_amount++;
    //create larl parser with tokenvector
    std::cout << "We do really love Tibo" << std::endl;
    return 0;
}