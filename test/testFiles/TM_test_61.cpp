#include <vector>
#include <iostream>

class SchaakStuk;

class Pion{
private:
    int zwart;
public:
    Pion(int zwart,std::string t): zwart(zwart){}

    std::string getKleur();

    std::pair<int,int> getPosition();
};

static int zwart = 1;
static int wit = 0;
static std::vector<std::vector<Pion*>> board;

void setStartBord();

// Geeft een pointer naar het schaakstuk dat op rij r, kolom k staat
// Als er geen schaakstuk staat op deze positie, geef nullptr terug
Pion* getPiece(const int r, const int k) {
    //return board[r][k];
}

// Zet het schaakstuk waar s naar verwijst neer op rij r, kolom k.
// Als er al een schaakstuk staat, wordt het overschreven.
// Bewaar in jouw datastructuur de *pointer* naar het schaakstuk,
// niet het schaakstuk zelf.
void setPiece(const int r, const int k, Pion* s){
    board[r][k]=s;
    //return;
}


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