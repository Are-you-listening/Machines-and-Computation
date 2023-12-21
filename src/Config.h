//
// Created by watson on 11/19/23.
//

#ifndef TOG_CONFIG_H
#define TOG_CONFIG_H

#include "lib/DesignByContract.h"
#include "lib/helper.h"
#include "lib/json.hpp"
#include "Orchestrator.h"

#include <string>

using json = nlohmann::json;
using namespace std;

/**
 * Class holding the Config data. It is constructed following the Singleton Design Patter.
 */
class Config {
private:
    int max_nesting; //0 is the main loop's {}, gives the value of max nesting that might occur in the generated program
    int split_nesting; //0 is an exact copy and paste, gives the amount of transferred nesting
    int ifElse_nesting; //Specifies if if-else statements needs to be taken care off. 0 includes no changes

    bool threading; //Specifies if threading should be used to optimise the code
    bool staticMemory; //Specifies if variables may be manipulated and set into static memory of the processed code

    static Config * init; //Hold a ptr to itself

    explicit Config();//Private Constructor

public:
    /**
     * Singletons should not be cloneable.
     */
    Config(Config &other) = delete;

    /**
     * Singletons should not be assignable.
     */
    void operator=(const Config &) = delete;

    /**
     * Simple Destructor
     */
    virtual ~Config();

    /**
     * //Used as constructor
     * @return
     */
    static Config *getConfig();

    /**
     * Simple getter
     * @return
     */
    [[nodiscard]] int getMaxNesting() const;

    /**
     * Simple getter
     * @return
     */
    [[nodiscard]] int getSplitNesting() const;

    /**
     * Simple getter
     * @return
     */
    [[nodiscard]] int getIfElseNesting() const;

    /**
     * Simple getter
     * @return
     */
    [[nodiscard]] bool isThreading() const;

    /**
     * Simple getter
     * @return
     */
    [[nodiscard]] bool isStaticMemory() const;

    /**
     * Simple Setter
     * @param maxNesting
     */
    void setMaxNesting(int maxNesting);

    /**
     * Simple setter
     * @param splitNesting
     */
    void setSplitNesting(int splitNesting);

    /**
     * Simple setter
     * @param ifElseNesting
     */
    void setIfElseNesting(int ifElseNesting);

    /**
     * Simple setter
     * @param threading
     */
    void setThreading(bool threading);

    /**
     * Simple setter
     * @param staticMemory
     */
    void setStaticMemory(bool staticMemory);

    /**
     * Simple setter
     * @param init
     */
    static void setInit(Config *init);
};


#endif //TOG_CONFIG_H
