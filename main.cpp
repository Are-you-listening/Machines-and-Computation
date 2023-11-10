//
// Created by watson on 10/11/23.
//

#include <iostream>
#include <thread>
#include <ctime>
#include "Tokenazation.h"
#include "CFG.h"
#include "ThreadFunction.h"
#include "filesystem"

static unsigned int core_amount = std::thread::hardware_concurrency(); // gets "core amount", in windows you can allocate infinite threads. in linux this isn't possible i believe. so pls care about this.
// so whenever you thread something, pls change core_amount. Also core_amount isn't the perfect name for this

int main() { // Function names we create to replace nesting should have F or I in their names so we know if they were for-loops or If-loops
    Tokenazation tokenVector;
    std::string Filelocation="Nested/engine.cc";
    std::thread Tokenizer(&Tokenazation::Tokenize, &tokenVector, Filelocation);
    core_amount--;
    
    //CFG cfg("TestFiles/CFG.json"); //don't forget to add CFG back into cmakelist in right set
    //cfg.toGNF(); // this still needs massive debugging.
    
    Tokenizer.join();
    core_amount++;
    //create larl parser with tokenvector
    
    //threading every function for now, will later be changed
    // i also assume that every function we create to replace nesting is only called upon once
    // result don't work for now, will be changed
    std::string ResultFileLocation="Nested/copyengine.cc";
    std::string line;
    std::string line2;
    std::ifstream File(ResultFileLocation);
    std::vector<std::string> Functioncalls;
    while(getline(File,line)){
        if(line[0]!='#'&&line[0]!=' '&&line.substr(0,6)!="static"&&line.substr(0,6)!="struct"&&!line.empty()&&line!="}"&&line!="{"){ // i assume the code we check people don't write variables or classes above a function
            Functioncalls.push_back(line);
        }
    }
    std::vector<std::thread> Threads;
    ThreadFunction threading;
    unsigned long int count=0;
    for(const auto & i : Functioncalls){
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
    File.clear();
    File.seekg(0);
    File.close();
    for(unsigned long int i=0; i<count; i++){
        std::ofstream File2(ResultFileLocation+"tempresult.cc" + std::to_string(i%2));
        std::ifstream File3(ResultFileLocation + std::to_string(i));
        std::ifstream File4;
        if(i==0){
            File4 = std::ifstream(ResultFileLocation);
        } else {
            File4 = std::ifstream(ResultFileLocation + std::to_string(i) + std::to_string(!(i%2)));
        }
        std::string line3;
        File.clear();
        File.seekg(0);
        while(getline(File3,line3)){
            getline(File4,line);
            unsigned int temp3=0;
            for(auto S: line){
                if(S==' '){
                    temp3++;
                } else {
                    break;
                }
            }
            if(line!=line3){
                if(line3.size()>temp3){
                    if(line3.substr(temp3,temp3)!="a000"){
                        File2 << line3 << std::endl;
                    }
                } else {
                    File2 << line3 << std::endl;
                }
            } else {
                File2 << line << std::endl;
            }
        }
        File3.close();
        File2.close();
        File4.close();
    }
    std::ofstream File5(ResultFileLocation+"result.cc");
    std::ifstream File6(ResultFileLocation+"tempresult.cc" + std::to_string((count-1)%2));
    for(unsigned long int i=0; i<count; i++){
        std::ifstream File3(ResultFileLocation + std::to_string(i));
        std::string line3;
        while(getline(File3,line3)){
            getline(File6,line);
            unsigned int temp3=0;
            for(auto S: line){
                if(S==' '){
                    temp3++;
                } else {
                    break;
                }
            }
            if(line3.size()>temp3){
                if (line3.substr(temp3, temp3) == "a000") {
                    File5 << line3 << std::endl;
                }
            }
            File5 << line << std::endl;
        }
    }
    
    for(unsigned long int i=0; i<count; i++){
        std::string c=ResultFileLocation + std::to_string(i);
        std::remove(c.c_str());
    }
    std::string c=ResultFileLocation +"tempresult.cc"+std::to_string(0);
    std::remove(c.c_str());
    c=ResultFileLocation +"tempresult.cc"+std::to_string(1);
    std::remove(c.c_str());
    std::cout << "We do really love Tibo" << std::endl;
    return 0;
}