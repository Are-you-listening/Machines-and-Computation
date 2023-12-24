//
// Created by anass on 10-11-2023.
//

#ifndef TOG_THREADFUNCTION_H
#define TOG_THREADFUNCTION_H

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <mutex>
#include <thread>
#include <filesystem>
#include <set>

class ThreadFunction {
public:
    void ThreadFunctionCall(const std::string& FileLocation, const std::string& Function);

    static void threadFILE(const std::string &ResultFileLocation);
};

#endif //TOG_THREADFUNCTION_H