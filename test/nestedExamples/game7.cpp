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

//Verzamel alle treats van een kleur
vector<pair<int, int>> CollectAllTreats(const zw &color)  {
    vector<Pion*>  options;
    vector<pair<int, int>> treats;

    if(color==wit){options;
    }else{options;}

    for(int i=0 ; i<options.size() ; i++){ //Loop over stukken van de andere kleur
        vector<pair<int, int>> geldige_zetten; //Verzamel de acties van het stuk
        for(int j=0 ; j<geldige_zetten.size() ; j++){ //Loop over de acties van het stuk
            if( (getPiece(geldige_zetten[j].first , geldige_zetten[j].second) != nullptr) && (getPiece(geldige_zetten[j].first , geldige_zetten[j].second)->getKleur() == color )){ //Als 1 v/d actie posities een ander stuk is van de te vinden kleur
                treats.push_back(geldige_zetten[j]);
            }
        }
        vector<pair<int, int>> pion_zetten; //Verzamel ook de enpassant zetten & bijbehorende treats (enpassant zetten gaan niet op een andere kleur staan)
        for(int s = 0 ; s<pion_zetten.size() ; s++){
            int r = pion_zetten[s].first;
            int k = pion_zetten[s].second;
            if(options[i]->getKleur() == "wit"){ //Als het een witte pion is;
                treats.push_back( {r+1,k} ); //(Zwart moved rij+1
            }else{ //Kleur = zwart
                treats.push_back( {r-1,k} );
            }
        }
    }
    return treats;
}