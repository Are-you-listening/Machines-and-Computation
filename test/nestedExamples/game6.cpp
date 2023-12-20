#include <vector>
#include <iostream>

using namespace std;

class SchaakStuk;
class Pion{
private:
    int zwart;
public:
    Pion(int zwart,string t): zwart(zwart){}

    std::string getKleur();

    pair<int,int> getPosition();
};

static int zwart = 1;
static int wit = 0;
static vector<vector<Pion*>> board;

void setStartBord();

// Geeft een pointer naar het schaakstuk dat op rij r, kolom k staat
// Als er geen schaakstuk staat op deze positie, geef nullptr terug
Pion* getPiece(const int r, const int k) {
    return board[r][k];
}

// Zet het schaakstuk waar s naar verwijst neer op rij r, kolom k.
// Als er al een schaakstuk staat, wordt het overschreven.
// Bewaar in jouw datastructuur de *pointer* naar het schaakstuk,
// niet het schaakstuk zelf.
void setPiece(const int r, const int k, Pion* s){
    board[r][k]=s;
    return;
}

// Geeft true als kleur schaak staat
bool schaak(const zw &color)  {
    vector<pair<int, int>> treats = CollectAllTreats(color);

    //Loop over treats
    for(int i=0 ; i<treats.size() ; i++){
        if(getPiece(treats[i].first , treats[i].second) == getKing(color)){
            return true;
        }
    }
    return false;
}