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


//Hulpfunctie schaakmat/pat: Geeft aan of er geldige moves zijn zonder schaak te staan
bool check_zero_options(const int &color){ //Hulpfunctie; anti-duplicated code
    vector<int> pieces;
    for (int i = 0; i < pieces.size(); i++) { //Loop over alle zetten/moves heen
        if (pieces.size() != 0) { //Wanneer de valid_moves > 0 , bestaat er een geldige move zodat we niet meer schaak staan
            return false;
        }
    }
    return true;
}








