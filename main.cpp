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
#include "src/GUI/GUI.h"

//!!!!!!!!!!!!!!!!!!! Default Config Location is already SET in Orchestrator.cpp
// Variabel Define might be a problem?


static unsigned int core_amount = std::thread::hardware_concurrency(); // gets "core amount", in windows you can allocate infinite threads. In linux this isn't possible, I believe. so pls care about this.
// so whenever you thread something, pls change core_amount. Also core_amount isn't the perfect name for this


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

    //threading every function for now, will later be changed
    // I also assume that every function we create to replace nesting is only called upon once
    // result don't work for now, will be changed
    // Function calls in Function calls don't work for now, another function that split those calls up is needed
    //std::string ResultFileLocation="../test/results/TM_handmatig_result.cpp";
    /*
    std::string ResultFileLocation="output/result.cpp";
    std::string line;
    std::string line2;
    std::ifstream File(ResultFileLocation);
    std::vector<std::string> FunctionCalls;
    while(getline(File,line)){
        if(line[0]!='#'&&line[0]!=' '&&line.substr(0,6)!="static"&&line.substr(0,6)!="struct"&&!line.empty()&&line!="}"&&line!="{"&&line.substr(0,7)!="typedef"&&line.substr(0,8)!="namespace"&&line.substr(0,6)=="void A"&&line.substr(0,2)!="//"){ // I assume the code we check people don't write variables or classes above a function, also needs debugging
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

    std::ifstream File9(ResultFileLocation+"result.cc");
    std::vector<std::string> V;
    std::string C;
    while(getline(File9,C)){
        V.push_back(C);
    }

    std::ofstream File10(ResultFileLocation+"result.cc");
    File10<<"#include <thread>"<<std::endl;
    for(const auto& it:V){
        File10 << it <<std::endl;
    }
    std::cout << "We do really love Tibo" << std::endl;
     */

    return 0;
}