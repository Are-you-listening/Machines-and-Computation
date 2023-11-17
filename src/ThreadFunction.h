//
// Created by anass on 10-11-2023.
//

#ifndef TOG_THREADFUNCTION_H
#define TOG_THREADFUNCTION_H
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

class ThreadFunction {
private:
    std::vector<std::string> ChangedVariables;
public:
    void ThreadFunctionCall(const std::string& FileLocation, const std::string& Function);
};


#endif //TOG_THREADFUNCTION_H
