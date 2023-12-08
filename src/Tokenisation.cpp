//
// Created by anass on 1-11-2023.
//

#include "Tokenisation.h"
#include <iostream>

void findD(unsigned long int Oldsize,std::vector<std::pair<std::string,std::string>>& tokenVector, const std::string& line, const std::string& FileLocation){
    for(unsigned long int i=Oldsize; i<tokenVector.size();i++){
        for(unsigned long int j=Oldsize; j>0;j--){
            if(tokenVector[j].first[0]=='D'){
                if(tokenVector[i].second==tokenVector[j].second){
                    std::cout << tokenVector[i].second << std::endl;
                    if(tokenVector[i].second=="typeParsed"){
                        std::cout << tokenVector[j-1].second << std::endl;
                        int a=0;
                    }
                    std::string TempSave;
                    std::string lastSave;
                    unsigned long int specialDeclaration= 0;
                    for(const auto & LC:tokenVector[j-1].second){
                        if(TempSave.find(tokenVector[i].second)!=std::string::npos&&(LC==' '||LC==','||LC=='='||LC==')'||LC=='(')&&(TempSave.size()-tokenVector[i].second.size())<=1&&specialDeclaration==0){
                            tokenVector[i].second=lastSave+" "+TempSave;
                            while(tokenVector[i].second[0]==' '){
                                tokenVector[i].second.erase(0,1);
                            }
                            if(tokenVector[i].second[tokenVector[i].second.size()-1]==';'){
                                tokenVector[i].second.erase(tokenVector[i].second.size()-1);
                            }
                            break;
                        }
                        if(LC=='<'){
                            specialDeclaration++;
                        } else if(LC=='>'){
                            if(specialDeclaration!=0){
                                specialDeclaration--;
                            }
                        }
                        if((LC!=' '&&LC!='('&&LC!=',')||specialDeclaration!=0){
                            TempSave+=LC;
                        } else {
                            lastSave=TempSave;
                            TempSave="";
                        }
                    }
                    std::cout << tokenVector[i].second << std::endl;
                    j--;
                }
            }
        }
    }
}
/*
void findD(unsigned long int Oldsize,std::vector<std::pair<std::string,std::string>>& tokenVector, const std::string& line, const std::string& FileLocation){
    for(unsigned long int i=Oldsize; i<tokenVector.size();i++){
        std::ifstream File1(FileLocation);
        std::string line2;

        std::vector<std::string> lines;
        while(getline(File1,line2)){
            unsigned long int count2=0;
            std::string spaces;
            while(line2[count2]==' '){
                spaces+=" ";
                count2++;
            }
            if(spaces+line==line2){
                break;
            }
            lines.push_back(line2);
        }

        for(auto it=lines.rbegin(); it!=lines.rend(); it++){
            std::string line1=*it;
            while(line1.substr(0,1)==" "){
                line1=line1.substr(1,std::string::npos);
            }
            if((line1.find(tokenVector[i].second+" ")!=std::string::npos||line1.find(tokenVector[i].second+"=")!=std::string::npos||line1.find(tokenVector[i].second+"(")!=std::string::npos||line1.find(tokenVector[i].second+")")!=std::string::npos||line1.find(tokenVector[i].second+";")!=std::string::npos||line1.find(tokenVector[i].second+"{")!=std::string::npos||line1.find(tokenVector[i].second+",")!=std::string::npos)&&line1.find(' ')!=std::string::npos&&(line1.find(';')!=std::string::npos||line1.find(',')!=std::string::npos)&&line1.find("return")==std::string::npos&&line1!=line&&line1.find(tokenVector[i].second)<line1.find('=')&&line1.find(' ')<line1.find(tokenVector[i].second)){
                std::string copy=tokenVector[i].second;
                try{
                    std::stoi(copy);
                } catch(...){
                    goto works;
                }
                break;
                works:
                tokenVector[i].second="";
                if(line1.find('(')!=std::string::npos){
                    std::string TempSave;
                    std::string lastSave;
                    unsigned long int specialDeclaration= 0;
                    for(const auto & LC:line1){
                        if(TempSave.find(copy)!=std::string::npos&&(LC==' '||LC==','||LC=='='||LC==')'||LC=='(')&&(TempSave.size()-copy.size())<=1&&specialDeclaration==0){
                            tokenVector[i].second=lastSave+" "+TempSave;
                            while(tokenVector[i].second[0]==' '){
                                tokenVector[i].second.erase(0,1);
                            }
                            if(tokenVector[i].second[tokenVector[i].second.size()-1]==';'){
                                tokenVector[i].second.erase(tokenVector[i].second.size()-1);
                            }
                            break;
                        }
                        if(LC=='<'){
                            specialDeclaration++;
                        } else if(LC=='>'){
                            if(specialDeclaration!=0){
                                specialDeclaration--;
                            }
                        }
                        if((LC!=' '&&LC!='('&&LC!=',')||specialDeclaration!=0){
                            TempSave+=LC;
                        } else {
                            lastSave=TempSave;
                            TempSave="";
                        }
                    }
                    break;
                } else {
                    while(line1[0]==' '){
                        line1.erase(0,1);
                    }
                    if(line1.find('>')!=std::string::npos){
                        tokenVector[i].second=line1.substr(0,line1.find("> ")+1)+" "+copy;
                    } else {
                        tokenVector[i].second=line1.substr(0,line1.find(' '))+" "+copy;
                    }
                    break;
                }
            }
        }
    }
}
*/
void vindVn(std::vector<std::pair<std::string,std::string>>& tokenVector, const std::string& line){
    std::set<char> VariableChar= {'_','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
                                  'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
    std::set<char> VariableCharSecond= {'_','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
                                        'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
                                        '0','1','2','3','4','5','6','7','8','9'};
    std::string tempV;
    unsigned long int count=0;
    bool append=false;
    bool SecondNotV=false;
    bool SecondNotVother=false;
    for(const char C: line){
        if(append){
            if(VariableCharSecond.find(C)!=VariableCharSecond.end()){
                tempV+=C;
                continue;
            } else if(C=='('||C==':'){
                tempV="";
                append= false;
                SecondNotV=false;
                continue;
            } else if(C=='.'){
                if(!SecondNotV){
                    tokenVector.emplace_back("V"+ std::to_string(count),tempV);
                    count++;
                    append= false;
                }
                tempV="";
                SecondNotV= true;
                continue;
            } else if(C=='-'){
                if(!SecondNotV){
                    tokenVector.emplace_back("V"+ std::to_string(count),tempV);
                    count++;
                    append= false;
                } else {
                    SecondNotV= false;
                }
                tempV="";
                SecondNotVother= true;
                continue;
            } else {
                if(!SecondNotV){
                    tokenVector.emplace_back("V"+ std::to_string(count),tempV);
                    count++;
                    append= false;
                } else {
                    SecondNotV= false;
                }
                tempV="";
                continue;
            }
        }
        if(C=='>'&&SecondNotVother){
            append= true;
            SecondNotV= true;
            SecondNotVother=false;
            continue;
        }
        if(VariableChar.find(C)!=VariableChar.end()){
            append= true;
            tempV+=C;
            SecondNotVother=false;
        } else{
            append=false;
        }
    }
}

void Tokenisation::Tokenize(const std::string &FileLocation) {
    std::ifstream File(FileLocation);
    std::string line;
    std::set<char> VariableChar= {'_','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
                                  'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
    std::set<char> VariableCharSecond= {'_','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
                                  'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
                                  '0','1','2','3','4','5','6','7','8','9'};
    while(getline(File,line)){
        unsigned long int tokenVectorSize=tokenVector.size();
        if(line.substr(0,5)=="std::"){
            line=line.substr(5,std::string::npos);
        } 
        while(line.substr(0,1)==" "){
            line=line.substr(1,std::string::npos);
        }
        bool forloop=false;
        bool whileloop=false;
        bool ifloop= false;
        if(line.substr(0,3)=="for"){
            tokenVector.emplace_back("F",line.substr(0,line.size()-1));
            if(line.find(':')!=std::string::npos&&line.find('=')==std::string::npos){
                tokenVector.emplace_back("V0",line.substr(line.find(':')+1,line.size()-1-line.find(':'))); // this still needs to be tested
                findD(tokenVector.size()-1,tokenVector,line,FileLocation);
            } else {
                unsigned long int Oldsize=tokenVector.size();
                unsigned long int LastCharPos;
                if(line.find('.')!=std::string::npos){
                    LastCharPos=line.find('.')-1;
                } else {
                    LastCharPos=line.find(';')-1;
                }
                std::string tempS = line.substr(line.find('=')+1,LastCharPos-line.find('='));
                tempS+="+"+line.substr(line.find(';')+1,line.size()-1-line.find(';'));
                vindVn(tokenVector,tempS);
                findD(Oldsize,tokenVector,line,FileLocation);
            }
            forloop= true;
        } else if(line.substr(0,2)=="if"){
            tokenVector.emplace_back("I",line);
            ifloop= true;
            goto Variables;
        } else if(line.find("else if")!=std::string::npos){
            tokenVector.emplace_back("e",line);
            ifloop= true;
            goto Variables;
        } else if(line.find("else")!=std::string::npos){
            tokenVector.emplace_back("E",line);
            ifloop= true;
        } else if(line.find("while")!=std::string::npos){
            tokenVector.emplace_back("F",line);
            std::string temps=line.substr(line.find("while")+5,std::string::npos);
            unsigned long int Oldsize=tokenVector.size();
            vindVn(tokenVector,temps);
            findD(Oldsize,tokenVector,line,FileLocation);
            whileloop= true;
        } else if(line.find(' ')!=std::string::npos&&(line.find('=')!=std::string::npos||((line.find('{')!=std::string::npos||line.find('(')!=std::string::npos)&&!(line.find('{')!=std::string::npos&&line.find('(')!=std::string::npos)))&&(line.find("return")==std::string::npos)&&(line.find('[')>line.find('='))){ // this part finds declarations in code
            unsigned long int spacecount=0;
            for(char it : line){
                if(it==' '){
                    spacecount++;
                } else if(it=='='){
                    break;
                }
            }
            if(line.find(" =")!=std::string::npos){
                spacecount--;
            }
            if(spacecount==0){
                tokenVector.emplace_back("V",line);
                goto Variables;
            }
            for(auto it=line.cbegin(); it!=line.cend(); it++){
                if(*it==' '){
                    it++; 
                    if(*it!='='){
                        tokenVector.emplace_back("D",line);
                        tokenVector.emplace_back("D0",line.substr(line.find(' ')+1,line.find('=')-(line.find(' ')+1))); // variable name;
                        while(tokenVector[tokenVector.size()-1].second.find(' ')!=std::string::npos){
                            tokenVector[tokenVector.size()-1].second.erase(tokenVector[tokenVector.size()-1].second.find(' '));
                        }
                        if(tokenVector[tokenVector.size()-1].second[tokenVector[tokenVector.size()-1].second.size()-1]==';'){
                            tokenVector[tokenVector.size()-1].second.erase(tokenVector[tokenVector.size()-1].second.size()-1);
                        }
                        break;
                    }
                }
            }
        } else if(line.find(' ')!=std::string::npos&&line.find('=')==std::string::npos&&line.find(';')!=std::string::npos&&line.find("return")==std::string::npos){
            unsigned int isDeclarationCount=0;
            bool isComma= false;
            for(char it : line){
                if(it==' '&&!isComma){
                    isDeclarationCount++;
                } else if(it==','){
                    isComma= true;
                } else {
                    isComma= false;
                }
            }
            if(isDeclarationCount==1){
                tokenVector.emplace_back("D",line);
                if(line.find('>')!=std::string::npos){
                    tokenVector.emplace_back("D0",line.substr(line.find("> ")+1,line.size()-1));
                } else {
                    tokenVector.emplace_back("D0",line.substr(line.find(' ')+1,line.size()-1));
                }
                while(tokenVector[tokenVector.size()-1].second[0]==' '){
                    tokenVector[tokenVector.size()-1].second.erase(0,1);
                }
                if(tokenVector[tokenVector.size()-1].second[tokenVector[tokenVector.size()-1].second.size()-1]==';'){
                    tokenVector[tokenVector.size()-1].second.erase(tokenVector[tokenVector.size()-1].second.size()-1);
                }
            }
        } else if((line.find('+')!=std::string::npos||line.find('-')!=std::string::npos||line.find('*')!=std::string::npos // this just gives on when a variable is used, not which or even which scope the variable is from
        ||line.find('/')!=std::string::npos||line.find('%')!=std::string::npos||line.find("++")!=std::string::npos // so use in combination of (D,line) to dither the scopes.
        ||line.find("--")!=std::string::npos||line.find("++")!=std::string::npos||line.find('=')!=std::string::npos
        ||line.find("+=")!=std::string::npos||line.find("-=")!=std::string::npos||line.find("*=")!=std::string::npos
        ||line.find("/=")!=std::string::npos||line.find("%=")!=std::string::npos||line.find("==")!=std::string::npos
        ||line.find("!=")!=std::string::npos||line.find('>')!=std::string::npos||line.find('<')!=std::string::npos
        ||line.find(">=")!=std::string::npos||line.find("<=")!=std::string::npos||line.find("&&")!=std::string::npos
        ||line.find("||")!=std::string::npos||line.find('!')!=std::string::npos||line.find('&')!=std::string::npos
        ||line.find("<<")!=std::string::npos||line.find(">>")!=std::string::npos||line.find("->")!=std::string::npos
        ||line.find('.')!=std::string::npos||line.find('[')!=std::string::npos||(line.find('(')!=std::string::npos&&
        line.find(')')!=std::string::npos&&line.find("()")==std::string::npos)||line.find('|')!=std::string::npos)&&
        line.find('#')==std::string::npos&&line.find("){")==std::string::npos){
            tokenVector.emplace_back("V",line);
            Variables:
            unsigned long int Oldsize=tokenVector.size();
            vindVn(tokenVector,line);
            findD(Oldsize,tokenVector,line,FileLocation);
        }
        long int bracecount=0;
        for(char it: line){
            if(it=='{'){
                bracecount++;
            }
            if(it=='}'){
                bracecount--;
            }
        }
        switch(bracecount){
            case 1:
                if(!(forloop||whileloop||ifloop)){
                    tokenVector.emplace_back("C",line.substr(0,line.size()-1));
                }
                tokenVector.emplace_back("{","{");
                break;
            case -1:
                tokenVector.emplace_back("}","}");
                break;
            default:
                break;
        }
        if(tokenVectorSize==tokenVector.size()){
            tokenVector.emplace_back("C",line);
        }
    }
    File.close();
    bool FoundVn= false;
    std::set<std::string> S={};
    for(const auto & token:tokenVector){
        if(token.first=="V"){
            tokenTupleVector.emplace_back(token.first,token.second,S);
            FoundVn= true;
        } else if(token.first[0]=='V'){
            if(token.second!="for"&&token.second!="if"&&token.second!="else"&&token.second.find(' ')!=std::string::npos){
                S.insert(token.second);
            }
        } else if(!FoundVn&&token.first!="D0"){
            tokenTupleVector.emplace_back(token.first,token.second,S);
            FoundVn= false;
        } else {
            std::get<2>(tokenTupleVector[tokenTupleVector.size()-1])=S;
            S.clear();
            FoundVn= false;
        }
    }
    for(const auto & i:tokenVector){
        std::cout<<i.first+", " << i.second << std::endl;
    }
    tokenVector.clear();
}

const std::vector<std::tuple<std::string, std::string, std::set<std::string>>> &Tokenisation::getTokenVector() const {
    return tokenTupleVector;
}
