//
// Created by anass on 22-12-2023.
//
#include <vector>

int main(){
    std::vector<int> V={1,3,4,5,6};
    for(std::vector<int>::iterator i=V.begin(); i!=V.end(); i++){
        i++;
    }
    /*
     * other code
     */
    int a=V[5];
    return 0;
}