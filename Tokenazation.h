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
    /* pair.first, F declares for, C declares code, { declares for/if start
     *  I declares if, E declares else, e declares else if
     * 
     * if you encounter other symbols those are initializations of variables or uses.
     */
    std::vector<std::pair<std::string, std::string>> tokenVector;
    
public:
    Tokenazation()=default;
    void Tokenize(const std::string& FileLocation);

};


#endif //TOG_TOKENAZATION_H
