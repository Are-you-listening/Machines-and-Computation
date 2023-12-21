//
// Created by watson on 12/21/23.
//

#include <vector>
#include <iostream>

int* function2(int r, int k){
    return nullptr;
}

//Return alle posities waar een stuk van color opstaat
std::vector<int*> function(const int &color)  {
    std::vector<int*> options;

    for(int r=0 ; r<8 ; r++){
        std::string temp = "yay";
        for(int k=0 ; k<8 ; k++){
            int* piece = function2(r,k);
            if(piece!= nullptr){
                if(false){
                    options.push_back(piece);
                }
            }
        }
    }
}