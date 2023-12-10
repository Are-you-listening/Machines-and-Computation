//
// Created by watson on 10/11/23.
//

#include <iostream>
#include <thread>
#include <ctime>

#include "src/Tokenisation.h"
#include "src/CFG.h"
#include "src/ThreadFunction.h"
#include "filesystem"
#include "src/Config.h"
#include "src/CFGConstructor.h"
#include "src/LALR.h"

//!!!!!!!!!!!!!!!!!!! Default Config Location is already SET in Orchestrator.cpp
// Variabel Define might be a problem?


static unsigned int core_amount = std::thread::hardware_concurrency(); // gets "core amount", in windows you can allocate infinite threads. In linux this isn't possible, I believe. so pls care about this.
// so whenever you thread something, pls change core_amount. Also core_amount isn't the perfect name for this

int main() { // Function names we create to replace nesting should have F or I in their names, so we know if they were for-loops or If-loops
    Tokenisation tokenVector; // sometimes variables in a nesting that should be passed in a function call aren't passed because it isn't found in the source file, this is done on purpose.
    std::string Filelocation="input/nestedExamples/test3.cc"; // for now, doesn't support double declarations like int a,d;
    std::thread Tokenizer(&Tokenisation::Tokenize, &tokenVector, Filelocation); // i ignore rvalues in function calls
    //core_amount--;
    //Tokenizer.join();
    
    Orchestrator();

    auto cfg = createCFG();
    auto cfg3 = createCFG();
    CFG cfg2("input/CFG/testGNF.json");
    cfg2.setCnf(true);
    cfg2.toGNF();
    cfg2.print();
    cfg->print();
    cfg->toGNF(); // this still needs massive debugging.
    cfg->print();
    //cfg->toCNF();
    cfg3->toCNF();

    //string permutatar found online
    /*std::string str="{}FCIEeDV";
    unsigned int n = str.length();
    unsigned long int opsize = pow(2, n);
    
    for (int counter = 1; counter < opsize; counter++)
    {
        string subs;
        for (int j = 0; j < n; j++)
        {
            if (counter & (1<<j))
                subs.push_back(str[j]);
        }
        
        do
        {
            cout << std::endl;
            cout << subs << " "<<std::endl;
            cout << std::endl;
            // do here CYK checks
            if(cfg3->accepts(subs)!=cfg->accepts(subs)){
                std::cout << "oops i did it again" << std::endl;
                return -3;
            }
        }
        while (next_permutation(subs.begin(), subs.end()));
    }*/
    //
    
    const CFG a = *cfg;
    LALR lalr(a);
    lalr.createTable();
    
    Tokenizer.join();
    core_amount++;
    
    //create LARL parser with tokenvector
    auto vec = tokenVector.getTokenVector();
    lalr.parse(vec);
    std::cout << "debug" << std::endl;
    //cleanup
    //if-else antinesting
    //move
    //naam wijzinging states, zie cleanup.
    
    
    /*
    //threading every function for now, will later be changed
    // I also assume that every function we create to replace nesting is only called upon once
    // result don't work for now, will be changed
    // Function calls in Function calls don't work for now, another function that split those calls up is needed
    std::string ResultFileLocation="nestedExamples/copyengine.cc";
    std::string line;
    std::string line2;
    std::ifstream File(ResultFileLocation);
    std::vector<std::string> FunctionCalls;
    while(getline(File,line)){
        if(line[0]!='#'&&line[0]!=' '&&line.substr(0,6)!="static"&&line.substr(0,6)!="struct"&&!line.empty()&&line!="}"&&line!="{"){ // I assume the code we check people don't write variables or classes above a function, also needs debugging
            FunctionCalls.push_back(line);
        }
    }
    std::vector<std::thread> Threads; // still doesn't work, remember void functions and their returns, etc.
    ThreadFunction threading; // maybe create a function that turns every function into a void one.
    unsigned long int count=0; // I also assume calling join() on a thread that's already joined is not harmful.
    for(const auto & i : FunctionCalls){
        if(core_amount!=0){
            std::filesystem::copy(ResultFileLocation,ResultFileLocation + std::to_string(count));
            std::thread temp(&ThreadFunction::ThreadFunctionCall, &threading, ResultFileLocation + std::to_string(count), i);
            Threads.push_back(std::move(temp));
            count++;
            core_amount--;
        } else {
            for(std::vector<std::thread>::iterator it=Threads.begin(); it!=Threads.end(); it++){
                it->join();
                core_amount++;
            }
            Threads.clear();
            std::filesystem::copy(ResultFileLocation,ResultFileLocation + std::to_string(count));
            std::thread temp(&ThreadFunction::ThreadFunctionCall, &threading, ResultFileLocation+ std::to_string(count), i);
            Threads.push_back(std::move(temp));
            count++;
            core_amount--;
        }
    }
    for(std::vector<std::thread>::iterator it=Threads.begin(); it!=Threads.end(); it++){
        it->join();
        core_amount++;
    }
    File.close();

    std::ofstream File2(ResultFileLocation+"result.cc");
    std::ifstream File1(ResultFileLocation);
    for(unsigned long int i=0; i<count; i++){
        std::ifstream File4(ResultFileLocation + std::to_string(i));
        std::string line4;
        while(getline(File1,line)){
            getline(File4,line4);
            if(line4.find("std::thread a")!=std::string::npos){
                File2 << line4 << std::endl;
            } else {
                File2 << line << std::endl;
            }
            while(line4.find("a000")!=std::string::npos&&line4.find("thread ")==std::string::npos){
                getline(File4,line4);
            }
        }
        std::ifstream File7(ResultFileLocation+"result.cc");
        std::string line7;
        std::ofstream File8(ResultFileLocation+"tempresult.cc0");
        while(getline(File7,line7)){
            File8 << line7 << std::endl;
        }
        File7.close();
        File8.close();
        File2=std::ofstream(ResultFileLocation+"result.cc");
        File1=std::ifstream(ResultFileLocation+"tempresult.cc0");
    }
    File2.close();
    File1.close();
    
    std::ofstream File5(ResultFileLocation+"result.cc");
    std::ifstream File6(ResultFileLocation+"tempresult.cc0");
    for(unsigned long int i=0; i<count; i++){
        std::ifstream File3(ResultFileLocation + std::to_string(i));
        std::string line3;
        while(getline(File6,line)){
            getline(File3,line3);
            while(line.find("a000")!=std::string::npos&&line.find("thread ")==std::string::npos){
                File5 << line << std::endl;
                getline(File6,line);
            }
            while(line3.find("a000")!=std::string::npos&&line3.find("thread ")==std::string::npos){
                File5 << line3 << std::endl;
                getline(File3,line3);
            }
            File5 << line << std::endl;
        }
        std::ifstream File7(ResultFileLocation+"result.cc");
        std::string line7;
        std::ofstream File8(ResultFileLocation+"tempresult.cc0");
        while(getline(File7,line7)){
            File8 << line7 << std::endl;
        }
        File7.close();
        File8.close();
        File5=std::ofstream(ResultFileLocation+"result.cc");
        File6=std::ifstream(ResultFileLocation+"tempresult.cc0");
    }
    std::ofstream File7(ResultFileLocation+"result.cc");
    std::string line8;
    std::ifstream File8(ResultFileLocation+"tempresult.cc0");
    while(getline(File8,line8)){
        File7 << line8 << std::endl;
    }
    File5.close();
    File6.close();
    File7.close();
    File8.close();
    
    for(unsigned long int i=0; i<count; i++){
        std::string c=ResultFileLocation + std::to_string(i);
        std::remove(c.c_str());
    }
    std::string c=ResultFileLocation +"tempresult.cc0";
    std::remove(c.c_str());
    std::cout << "We do really love Tibo" << std::endl;*/

    return 0;
}
