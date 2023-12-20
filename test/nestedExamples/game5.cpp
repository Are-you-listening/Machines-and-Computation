#include <vector>
#include <iostream>

//Return alle posities waar een stuk van color opstaat
vector<int*> getAllPieces(const int &color)  {
    vector<int*> options;

    for(int r=0 ; r<8 ; r++){
        for(int k=0 ; k<8 ; k++){
            int* piece = getPiece(r,k);
            if(piece!= nullptr){
                if(false){
                    options.push_back(piece);
                }
            }
        }
    }
}

