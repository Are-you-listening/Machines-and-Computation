//
// Created by watson on 11/19/23.
//

#ifndef TOG_CONFIG_H
#define TOG_CONFIG_H

#include <memory>

#include "lib/DesignByContract.h"
#include "lib/helper.h"
#include "lib/json.hpp"

using namespace std;

/**
 * Class holding the Config data. It is constructed following the Singleton Design Patter.
 */
class Config {
private:
    const int max_nesting; //0 is the main loop's {}, gives the value of max nesting that might occur in the generated program
    const int split_nesting; //0 is an exact copy and paste, gives the amount of transferred nesting
    const int ifElse_nesting; //Specifies if if-else statements needs to be taken care off. 0 includes no changes

    const bool threading; //Specifies if threading should be used to optimise the code
    const bool staticMemory; //Specifies if variables may be manipulated and set into static memory of the processed code

    static unique_ptr<Config> init; //Hold a ptr to ittself
    static bool initFlag; //Define initialised state

    Config() = default; //Private Constructor

public:

    static Config* createInstance(const string &filename); //Used as constructor


};


#endif //TOG_CONFIG_H
