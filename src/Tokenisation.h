//
// Created by anass on 1-11-2023.
//

#ifndef TOG_TOKENISATION_H
#define TOG_TOKENISATION_H

#include <vector>
#include <string>
#include <fstream>
#include <set>
#include <tuple>
#include <algorithm>
#include <iostream>

class Tokenisation {
private:
    /** pair.first, F declares for-loop, C declares code, { declares for/if start
     *  I declares if, E declares else, e declares else if
     * 
     * if you encounter other symbols those are initializations of variables or uses.
     **/
    std::vector<std::pair<std::string, std::string>> tokenVector;
    std::vector<std::tuple<std::string, std::string, std::set<std::string>>> tokenTupleVector;

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
     * Getter
     * @return 
     */
    [[nodiscard]] const std::vector<std::tuple<std::string, std::string, std::set<std::string>>> &getTokenVector() const;
};

#endif //TOG_TOKENISATION_H
