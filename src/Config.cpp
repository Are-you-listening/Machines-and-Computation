//
// Created by watson on 11/19/23.
//

#include "Config.h"


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