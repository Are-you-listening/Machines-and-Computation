#include <vector>
#include <iostream>


//Hulpfunctie: verwijdert alle pionen van het bord
void reset() {
    for(int r=0 ; r<8 ; r++){ //Loop over spelbord en verwijder iedere stuk
        for(int k=0 ; k<8 ; k++){
            if(getPiece(r,k)!= nullptr){
                delete getPiece(r,k);
            }
            setPiece(r,k, nullptr);
        }
    }
}