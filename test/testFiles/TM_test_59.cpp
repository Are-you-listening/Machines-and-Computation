#include <vector>
#include <iostream>

using namespace std;

class SchaakStuk;
class Pion{
private:
    int zwart;
public:
    Pion(int zwart, string t): zwart(zwart){}

    std::string getKleur();

    pair<int,int> getPosition();
};

static int zwart = 1;
static int wit = 0;
static vector<vector<Pion*>> board;

void setStartBord();


Pion* getPiece(const int r, const int k) {
    return board[r][k];
}


void setPiece(const int r, const int k, Pion* s){
    board[r][k]=s;
    return;
}



bool check_zero_options(const int &color){
    vector<int> pieces;
    for (int i = 0; i < pieces.size(); i++) {
        if (pieces.size() != 0) {
            return false;
        }
    }
    return true;
}








