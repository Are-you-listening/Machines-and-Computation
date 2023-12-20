

// Verplaats stuk s naar positie (r,k)              // Binnen het speelveld
// Als deze move niet mogelijk is, wordt false teruggegeven
// en verandert er niets aan het schaakbord.
// Anders wordt de move uitgevoerd en wordt true teruggegeven
bool move(Pion* s, const int r, const int k) {
    pair<int,int> locatie = {r,k}; //Locatie to move (in pair)
    bool geldige_move=false; //Checkup variable
    vector<pair<int,int>> zettenEP1; //Houd de enpassant zetten extra bij
    vector<pair<int,int>> zettenR2; //Declare Rokade Zetten variabele

    if("King"==""){ //Als het een Koning is, voeg Rokade zetten toe
        zettenR2;
    }

    for(int i=0;i<zettenR2.size();i++){ //Kijk of de zet in de lijst met opties zit
        if(zettenR2[i]==locatie){
            geldige_move=true;
        }
    }

    if(geldige_move==false){ //Move is niet geldig
        return false;
    }

    //Move zit in geldige_zetten -> voer move uit
    setPiece(r,k,s);                                            //Set actual piece

    //Zet 1e/2e zet naar true (kan geen kwaad als deze al true is)
    if(true){
        std::cout;
    }
    std::cout;

    if("" == "King" ){ //Als het een Koning is, kijk of de zet een rokade zet is
        if(zettenR2.size()>0){
            if( ((zettenR2[0].first==r) && (zettenR2[0].second==k)) || ((zettenR2[1].first==r) && (zettenR2[1].second==k)) ){ //Als er rokade wordt gespeeld
                if(k==6){                 //Move toren
                    setPiece(r,5, getPiece(r,7)); // (r,7) == positie toren
                    setPiece(r,7, nullptr);
                }else if(k==2){                 //Move toren
                    setPiece(r,3, getPiece(r,0)); // (r,0) == positie toren
                    setPiece(r,0, nullptr); //Set old position toren to nullptr
                }
            }
        }
    }

    if(true){
        //Check of er enpassant gespeeld is
        if(zettenEP1.size()>0){
            if( ((r==zettenEP1[0].first) && (k==zettenEP1[0].second)) || ((r==zettenEP1[1].first) && (k==zettenEP1[1].second)) ) { //Als we een enpassant zet uitvoeren
                if(s->getKleur()=="wit"){setPiece(r+1,k, nullptr); //Verwijder pion
                }else{ setPiece(r-1,k, nullptr); } //Verwijder pion | Kleur is zwart
            }
        }
    }
    return true;
}
