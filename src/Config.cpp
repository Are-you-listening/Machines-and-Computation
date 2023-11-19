//
// Created by watson on 11/19/23.
//

#include "Config.h"

Config *Config::createInstance(const string &filename) {
    REQUIRE(FileExists(filename),"File cannot be found!");

    std::ifstream f(c);
    json data = json::parse(f);


    return nullptr;
}
