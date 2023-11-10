//
// Created by anass on 1-11-2023.
//

#include "Tokenazation.h"

void Tokenazation::Tokenize(const std::string &FileLocation) {
    std::ifstream File(FileLocation);
    std::string line;
    while(getline(File,line)){
        unsigned long int tokenVectorSize=tokenVector.size();
        if(line.substr(0,5)=="std::"){
            line=line.substr(5,std::string::npos);
        } 
        while(line.substr(0,1)==" "){
            line=line.substr(1,std::string::npos);
        }
        bool forloop=false;
        if(line.substr(0,3)=="for"){
            tokenVector.emplace_back("F",line.substr(0,line.size()-1));
            forloop= true;
        } else if(line.substr(0,2)=="if"){
            tokenVector.emplace_back("I",line);
        } else if(line.find("else if")!=std::string::npos){
            tokenVector.emplace_back("e",line);
        } else if(line.find("else")!=std::string::npos){
            tokenVector.emplace_back("E",line);
        }
        long int bracecount=0;
        for(char it: line){
            if(it=='{'){
                bracecount++;
            }
            if(it=='}'){
                bracecount--;
            }
        }
        switch(bracecount){
            case 1:
                if(!forloop){
                    tokenVector.emplace_back("C",line.substr(0,line.size()-1));
                }
                tokenVector.emplace_back("{","{");
                break;
            case -1:
                tokenVector.emplace_back("}","}");
                break;
            default:
                break;
        }
        if(tokenVectorSize==tokenVector.size()){
            tokenVector.emplace_back("C",line);
        }
    }
    File.close();
}
