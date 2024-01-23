//
// Created by watson on 11/19/23.
//

#include "Orchestrator.h"

Orchestrator::Orchestrator(const std::string &cppFile) {
    //createCFG();
}

void Orchestrator::tabber() {
    std::ifstream File910("output/result.cpp");
    std::vector<std::string> V10;
    std::string C10;
    while(getline(File910,C10)){
        while(C10.substr(0,1)==" "){
            C10=C10.substr(1,std::string::npos);
        }
        V10.push_back(C10);
    }
    File910.close();


    std::ofstream File1010("output/result.cpp");
    std::string tab;
    for(const auto& it:V10){
        File1010 <<tab<< it <<std::endl;

        int add = std::count(it.begin(), it.end(), '{');
        int remove = std::count(it.begin(), it.end(), '}');

        for (int i = 0; i<add; i++){
            tab+="    ";
        }

        for (int i = 0; i<remove; i++){
            for (int j =0; j<4; j++){
                tab.pop_back();
            }
        }
    }
    File1010.close();
}

void Orchestrator::threadingTest() {
    std::string ResultFileLocation="output/result.cpp";
    ThreadFunction::threadFILE(ResultFileLocation);

    auto start1=std::chrono::high_resolution_clock::now();
    std::string SystemString= "g++ " + ResultFileLocation + "result.cc";
    system(SystemString.c_str());
    auto second1=std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(second1 - start1);

    cout << "normal: "<< duration1.count() << " microseconds" << endl;

    auto start=std::chrono::high_resolution_clock::now();
    const std::string SystemString2= "g++ " + ResultFileLocation;
    system(SystemString2.c_str());
    auto second=std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(second - start);

    cout << "threaded: "<< duration.count() << " microseconds" << endl;

}
