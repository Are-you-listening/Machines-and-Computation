//
// Created by watson on 11/19/23.
//

#include "Config.h"

string Orchestrator::file = "input/config/config.json";
Config* Config::init{nullptr}; //Static Members should be initialised outside the Class

Config::Config(){
    REQUIRE(FileExists(Orchestrator::file), "File cannot be found!");

    std::ifstream f(Orchestrator::file);
    json data = json::parse(f);

    Config::max_nesting = data["max_nesting"];
    Config::split_nesting = data["split_nesting"];
    Config::ifElse_nesting = data["ifElse_nesting"];
    Config::threading = data["threading"];
    Config::staticMemory = data["staticMemory"];
}

Config::~Config(){
    delete init;
    init = nullptr; //Set back to nullptr
}

Config * Config::getConfig() {
    if (init == nullptr) {
        init = new Config();
    }
    return init;
}

int Config::getMaxNesting() const {
    return max_nesting;
}

int Config::getSplitNesting() const {
    return split_nesting;
}

int Config::getIfElseNesting() const {
    return ifElse_nesting;
}

bool Config::isThreading() const {
    return threading;
}

bool Config::isStaticMemory() const {
    return staticMemory;
}

void Config::setMaxNesting(int maxNesting) {
    max_nesting = maxNesting;
}

void Config::setSplitNesting(int splitNesting) {
    split_nesting = splitNesting;
}

void Config::setIfElseNesting(int ifElseNesting) {
    ifElse_nesting = ifElseNesting;
}

void Config::setThreading(bool threading) {
    Config::threading = threading;
}

void Config::setStaticMemory(bool staticMemory) {
    Config::staticMemory = staticMemory;
}

void Config::setInit(Config *init) {
    Config::init = init;
}
