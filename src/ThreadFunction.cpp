//
// Created by anass on 10-11-2023.
//

#include "ThreadFunction.h"

static unsigned long int ThreadNameFunction=0xA0000000;
static std::mutex thread_name_lock;

void ThreadFunction::ThreadFunctionCall(const std::string& FileLocation, const std::string& Function){
    std::vector<std::string> ChangedVariables;
    unsigned long int left=0;
    unsigned long int right=0;
    for(unsigned long int i=0; i<Function.size(); i++){
        if(Function[i]==' '&&left==0){
            left=i+1;
        } else if (Function[i]=='('&&right==0){
            right=i;
        }
    }
    std::string FunctionName=Function.substr(left, right - left);
    bool appending= false;
    std::string temp;
    for(const auto& i: Function){
        if(i=='(') {
            appending= true;
        }
        if(i==','||i==')'){
            temp=temp.substr(1,std::string::npos);
            ChangedVariables.push_back(temp);
            temp.clear();
        }
        if(appending){
            temp+=i;
        }
    }
    std::ifstream File5(FileLocation);
    std::string line3;
    std::vector<std::vector<std::string>> VusedVariables;
    while(getline(File5,line3)){
        if(line3.find(FunctionName + "(") != std::string::npos){
            std::vector<std::string> usedVariables;
            std::string temp2;
            bool appending2=false;
            for(const auto& i: line3){
                if(i=='(') {
                    appending2= true;
                }
                if(i==','||i==')'){
                    temp2=temp2.substr(1,std::string::npos);
                    usedVariables.push_back(temp2);
                    temp2.clear();
                }
                if(appending2){
                    temp2+=i;
                }
            }
            for(unsigned long int i=0; i<usedVariables.size(); i++){
                for(unsigned long int j=0; j<usedVariables[i].size(); j++){
                    if(usedVariables[i][j]==' '||usedVariables[i][j]=='&'){
                        usedVariables[i]=usedVariables[i].substr(j+1,std::string::npos);
                        i--;
                        break;
                    }
                }
            }
            VusedVariables.push_back(usedVariables);
        }
    }
    std::vector<std::string> joins;
    std::stringstream tempName;
    thread_name_lock.lock();
    tempName << std::hex << ThreadNameFunction;
    std::string FunctionCall= "std::thread " + tempName.str() + "(" + FunctionName + ", ";
    joins.push_back(tempName.str());
    ThreadNameFunction++;
    thread_name_lock.unlock();
    std::vector<std::string> copy=VusedVariables[0];
    for(std::vector<std::vector<std::string>>::iterator it=VusedVariables.begin(); it!=VusedVariables.end(); it++){
        for(std::vector<std::string>::iterator it2=it->begin(); it2!=it->end(); it2++){
            if((*it2).empty()){
                *it2="a665a45920422f9d417e4867efdc4fb8a04a1f3fff1fa07e998e86f7f7a27ae3";
            }
        }
    }
    
    for(std::vector<std::string>::iterator it2=ChangedVariables.begin(); it2!=ChangedVariables.end(); it2++){
        if((*it2).empty()){
            *it2="a665a45920422f9d417e4867efdc4fb8a04a1f3fff1fa07e998e86f7f7a27ae3";
        }
    }
    for(unsigned long int i=0; i<copy.size(); i++){
        for(unsigned long int j=0; j<copy[i].size(); j++){
            if(copy[i][j]==' '||copy[i][j]=='&'){
                copy[i]=copy[i].substr(j+1,std::string::npos);
                i--;
                break;
            }
        }
    }
    for(unsigned long int i=0; i<copy.size()-1; i++){
        FunctionCall+="std::ref("+VusedVariables[0][i]+")"+", ";
    }
    FunctionCall[FunctionCall.size()-2]=')';
    FunctionCall[FunctionCall.size()-1]=';';
    std::fstream File(FileLocation);
    std::ofstream File2(FileLocation+"thread");
    std::string line2;
    std::string uselessLine;
    std::vector<std::string> FunctionCalls;
    unsigned long int count=0;
    while(getline(File,line2)){
        std::string temp2;
        for(auto S: line2){
            if(S==' '){
                temp2+=' ';
            } else {
                break;
            }
        }
        if(line2!=Function&& line2.find(FunctionName + "(") != std::string::npos&&line2.find(';')!=std::string::npos&&line2.find('(')==(temp2+FunctionName).size()){
            File2 << temp2<<FunctionCall<<std::endl;
            FunctionCalls.push_back(FunctionCall);
            FunctionCall.clear();
            tempName.str("");
            thread_name_lock.lock();
            tempName << std::hex << ThreadNameFunction;
            FunctionCall= "std::thread " + tempName.str() + "(" + FunctionName + ", ";
            joins.push_back(tempName.str());
            ThreadNameFunction++;
            thread_name_lock.unlock();
            count++;
            if(count<VusedVariables.size()-1){
                for(unsigned long int i=0; i<copy.size()-1; i++){
                    FunctionCall+="std::ref("+VusedVariables[count][i]+")"+", ";
                }
            }
            FunctionCall[FunctionCall.size()-2]=')';
            FunctionCall[FunctionCall.size()-1]=';';
        } else {
            File2 <<line2<< std::endl;
        }
    }
    File2.close();
    for(unsigned long int i=0; i<ChangedVariables.size(); i++){
        if(ChangedVariables[i].find("const")!=std::string::npos){
            ChangedVariables.erase(ChangedVariables.cbegin()+ (int) i);
            i--;
        } else if(ChangedVariables[i].find('&')==std::string::npos){
            ChangedVariables.erase(ChangedVariables.cbegin()+ (int) i);
            i--;
        }
    }
    for(unsigned long int i=0; i<ChangedVariables.size(); i++){
        for(unsigned long int j=0; j<ChangedVariables[i].size(); j++){
            if(ChangedVariables[i][j]==' '||ChangedVariables[i][j]=='&'){
                ChangedVariables[i]=ChangedVariables[i].substr(j+1,std::string::npos);
                i--;
                break;
            }
        }
    }
    std::ifstream File3(FileLocation+"thread");
    std::ofstream File4(FileLocation);

    std::string line;
    File.clear();
    File.seekg(0);
    std::vector<unsigned long int> reachedFunctions;

    while(getline(File3,line)){
        for(unsigned long int i=0; i<FunctionCalls.size(); i++){
            if(line.find(FunctionCalls[i])!=std::string::npos){
                reachedFunctions.push_back(i);
            }
        }
        std::string temp3;
        for(auto S: line){
            if(S==' '){
                temp3+=' ';
            } else {
                break;
            }
        }
        for(unsigned long int i=0; i<reachedFunctions.size(); i++){
            if(VusedVariables[reachedFunctions[i]].empty()&&line!=temp3+FunctionCalls[reachedFunctions[i]]){
                File4<<temp3<<joins[reachedFunctions[i]]<<".join();"<<std::endl;
                reachedFunctions.erase(reachedFunctions.cbegin()+ (int) i);
                i=-1;
                continue;
            }
            for(unsigned long int j=0; j<VusedVariables[reachedFunctions[i]].size();j++){
                if((line.find(VusedVariables[reachedFunctions[i]][j])!=std::string::npos||line.find("return")!=std::string::npos||line.find('}')!=std::string::npos)&&line!=temp3+FunctionCalls[reachedFunctions[i]]){
                    File4<<temp3<<joins[reachedFunctions[i]]<<".join();"<<std::endl;
                    reachedFunctions.erase(reachedFunctions.cbegin()+ (int) i);
                    i--;
                    break;
                }
            }
        }
        File4 << line << std::endl;
    }
    File.close();
    File2.close();
    File3.close();
    File4.close();
    std::string temp3=FileLocation+"thread";
    std::remove(temp3.c_str());
}

static unsigned int core_amount = std::thread::hardware_concurrency();

void ThreadFunction::threadFILE(const std::string& ResultFileLocation){
    //threading every function for now, will later be changed
    // I also assume that every function we create to replace nesting is only called upon once
    // result don't work for now, will be changed
    // Function calls in Function calls don't work for now, another function that split those calls up is needed
    //std::string ResultFileLocation="../test/results/TM_handmatig_result.cpp";
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
}