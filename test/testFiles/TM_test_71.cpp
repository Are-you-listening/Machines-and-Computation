#include <vector>

using namespace std;

class Pion;

static vector<vector<Pion*>> board;



Pion* getPiece(const int r, const int k) {
    return board[r][k];
}

pair<int, int> getPosition(const Pion*s) {
    for(int r = 0 ; r<8 ; r++){
        for(int k = 0 ; k<8 ; k++){
            if(getPiece(r,k)==s){
                return {r,k};
            }
        }
    }

}


