//
// Created by anass on 1-11-2023.
//

#ifndef TOG_TOKENAZATION_H
#define TOG_TOKENAZATION_H

#include <vector>
#include <string>
#include <fstream>

class Tokenazation {
private:
    std::vector<std::pair<std::string, std::string>> tokenVector;
    
public:
    Tokenazation()=default;
    void Tokenize(const std::string& FileLocation);

};


#endif //TOG_TOKENAZATION_H
