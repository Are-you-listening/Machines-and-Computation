//
// Created by anass on 10-11-2023.
//

#include "ThreadFunction.h"
#include <iostream>
#include <mutex>
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
    std::string Functionname=Function.substr(left,right-left);
    bool appending= false;
    std::string temp;
    for(const auto& i: Function){
        if(i=='(') {
            appending= true;
        }
        if(i==','){
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
        if(line3.find(Functionname+"(")!=std::string::npos){
            std::vector<std::string> usedVariables;
            std::string temp2;
            bool appending2=false;
            for(const auto& i: line3){
                if(i=='(') {
                    appending2= true;
                }
                if(i==','){
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
    std::stringstream Tempname;
    thread_name_lock.lock();
    Tempname<<std::hex<<ThreadNameFunction;
    std::string FunctionCall="std::thread "+ Tempname.str()+"("+Functionname+", ";
    joins.push_back(Tempname.str());
    ThreadNameFunction++;
    thread_name_lock.unlock();
    std::vector<std::string> copy=VusedVariables[0];
    for(unsigned long int i=0; i<copy.size(); i++){
        for(unsigned long int j=0; j<copy[i].size(); j++){
            if(copy[i][j]==' '||copy[i][j]=='&'){
                copy[i]=copy[i].substr(j+1,std::string::npos);
                i--;
                break;
            }
        }
    }
    for(unsigned long int i=0; i<copy.size(); i++){
        FunctionCall+=VusedVariables[0][i]+", ";
    }
    FunctionCall[FunctionCall.size()-2]=')';
    FunctionCall[FunctionCall.size()-1]=';';
    std::fstream File(FileLocation);
    std::ofstream File2(FileLocation+"thread");
    std::string line2;
    std::string uselessline;
    std::vector<std::string> FunctionCalls;
    unsigned long int count=0;
    while(getline(File,line2)){
        if(line2!=Function&&line2.find(Functionname+"(")!=std::string::npos){
            std::string temp2;
            for(auto S: line2){
                if(S==' '){
                    temp2+=' ';
                } else {
                    break;
                }
            }
            File2 << temp2<<FunctionCall<<std::endl;
            FunctionCalls.push_back(FunctionCall);
            FunctionCall.clear();
            Tempname.str("");
            thread_name_lock.lock();
            Tempname<<std::hex<<ThreadNameFunction;
            FunctionCall="std::thread "+ Tempname.str()+"("+Functionname+", ";
            joins.push_back(Tempname.str());
            ThreadNameFunction++;
            thread_name_lock.unlock();
            count++;
            for(unsigned long int i=0; i<copy.size(); i++){
                FunctionCall+=VusedVariables[count][i]+", ";
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
            ChangedVariables.erase(ChangedVariables.cbegin()+i);
            i--;
        } else if(ChangedVariables[i].find('&')==std::string::npos){
            ChangedVariables.erase(ChangedVariables.cbegin()+i);
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
                File4<<temp3<<joins[reachedFunctions[i]]<<".join()"<<std::endl;
                reachedFunctions.erase(reachedFunctions.cbegin()+i);
                i=-1;
                continue;
            }
            for(unsigned long int j=0; j<VusedVariables[reachedFunctions[i]].size();j++){
                if((line.find(VusedVariables[reachedFunctions[i]][j])!=std::string::npos||line.find("return")!=std::string::npos||line.find('}')!=std::string::npos)&&line!=temp3+FunctionCalls[reachedFunctions[i]]){
                    File4<<temp3<<joins[reachedFunctions[i]]<<".join()"<<std::endl;
                    reachedFunctions.erase(reachedFunctions.cbegin()+i);
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