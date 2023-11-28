//
// Created by watson on 11/28/23.
//
#include <iostream>
#include <sstream>
#include <vector>

std::vector<std::string> splitByPipeWithSpaces(const std::string& input) {
    std::vector<std::string> result;
    std::istringstream iss(input);

    std::string temp;

    for (char c : input) {
        if(c =='|'){
            result.push_back(temp);
            temp.clear();
            continue;
        }
        if(c == ' '){
            continue;
        }
        temp+=(std::string(1, c));

        // Add a space after each character (excluding the last character)
        if (c != input.back()) {
            temp+=" ";
        }
    }

    return result;
}

int main() {
    std::string input = "cBWW | dBWW | eBWW | fBWW | iBWW | vBWW | aBWW | cWW | dWW | eWW | fWW | iWW | vWW | aWW | {GWW | {DWW | ε";

    std::vector<std::string> result = splitByPipeWithSpaces(input);

    // Display the result
    for (const auto& token : result) {
        std::cout << "W -> "+token << std::endl;
    }

    return 0;
}