//
// Created by anass on 1-11-2023.
//

#ifndef TOG_TOKENISATION_H
#define TOG_TOKENISATION_H

#include <vector>
#include <string>
#include <fstream>
#include <set>

class Tokenisation {
private:
    /* pair.first, F declares for-loop, C declares code, { declares for/if start
     *  I declares if, E declares else, e declares else if
     * 
     * if you encounter other symbols those are initializations of variables or uses.
     */
    std::vector<std::pair<std::string, std::string>> tokenVector;

public:
    /**
     * Default Constructor
     */
    Tokenisation()=default;
    
    /**
     * Transform code file to tokenVector
     * @param FileLocation 
     */
    void Tokenize(const std::string &FileLocation);
    
    /**
     * Simple Getter
     * @return 
     */
    const std::vector<std::pair<std::string, std::string>> &getTokenVector() const;
};

#endif //TOG_TOKENISATION_H
