#include <vector>
#include <iostream>

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