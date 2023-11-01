//
// Created by anass on 1-11-2023.
//

#include "Tokenazation.h"

void Tokenazation::Tokenize(const std::string &FileLocation) {
    std::ifstream File(FileLocation);
    std::string line;
    while(getline(File,line)){
        if(line.substr(0,5)=="std::"){
            
        } else{
            
        }
    }
    
}
