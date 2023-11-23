//
// Created by anass on 1-11-2023.
//

#include "Tokenisation.h"

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
        if(line.substr(0,3)=="for"){
            tokenVector.emplace_back("F",line.substr(0,line.size()-1));
            forloop= true;
        } else if(line.substr(0,2)=="if"){
            tokenVector.emplace_back("I",line);
        } else if(line.find("else if")!=std::string::npos){
            tokenVector.emplace_back("e",line);
        } else if(line.find("else")!=std::string::npos){
            tokenVector.emplace_back("E",line);
        } else if(line.find(' ')!=std::string::npos&&(line.find('=')!=std::string::npos||((line.find('{')!=std::string::npos||line.find('(')!=std::string::npos)&&!(line.find('{')!=std::string::npos&&line.find('(')!=std::string::npos)))){ // this part finds declarations in code
            for(auto it=line.cbegin(); it!=line.cend(); it++){
                if(*it==' '){
                    it++; 
                    if(*it!='='){
                        tokenVector.emplace_back("D",line);
                        tokenVector.emplace_back("D0",line.substr(line.find(' ')+1,line.find('=')-(line.find(' ')+1))); // variable name;
                        if(tokenVector[tokenVector.size()-1].second.find(" ")!=std::string::npos){
                            tokenVector[tokenVector.size()-1].second=tokenVector[tokenVector.size()-1].second.substr(0,tokenVector[tokenVector.size()-1].second.size()-1);
                        }
                        break;
                    }
                }
            }
        } else if(line.find(' ')!=std::string::npos&&line.find('=')==std::string::npos&&line.find(';')!=std::string::npos){
            unsigned int isDeclarationCount=0;
            for(char it : line){
                if(it==' '){
                    isDeclarationCount++;
                }
            }
            if(isDeclarationCount==1){
                tokenVector.emplace_back("D",line);
                tokenVector.emplace_back("D0",line.substr(line.find(' ')+1,line.size()-1));
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
        line.find(')')!=std::string::npos&&line.find("()")==std::string::npos))&&line.find('#')==std::string::npos
        &&line.find("){")==std::string::npos){
            unsigned long int count=0;
            tokenVector.emplace_back("V",line);
            std::string tempV;
            bool append=false;
            bool SecondNotV=false;
            bool SecondNotVother=false;
            unsigned long int Oldsize=tokenVector.size();
            for(const char C: line){
                if(append){
                    if(VariableCharSecond.find(C)!=VariableCharSecond.end()){
                        tempV+=C;
                        continue;
                    } else if(C=='('){
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
                    } else if(C=='>'&&SecondNotVother){
                        append= false;
                        tempV="";
                        SecondNotV= true;
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
                if(VariableChar.find(C)!=VariableChar.end()){
                    append= true;
                    tempV+=C;
                } else{
                    append=false;
                }
            }
            for(unsigned long int i=Oldsize; i<tokenVector.size();i++){
                std::ifstream File1(FileLocation);
                std::string line1;
                while(getline(File1,line1)){
                    if((line1.find(tokenVector[i].second+" ")!=std::string::npos||line1.find(tokenVector[i].second+"=")!=std::string::npos||line1.find(tokenVector[i].second+"(")!=std::string::npos||line1.find(tokenVector[i].second+")")!=std::string::npos||line1.find(tokenVector[i].second+";")!=std::string::npos||line1.find(tokenVector[i].second+"{")!=std::string::npos||line1.find(tokenVector[i].second+",")!=std::string::npos)&&line1.find(' ')!=std::string::npos&&(line1.find(';')!=std::string::npos||line1.find(',')!=std::string::npos)&&line1.find("return")==std::string::npos){
                        std::string copy=tokenVector[i].second;
                        tokenVector[i].second="";
                        if(line1.find('(')!=std::string::npos){
                            std::string TempSave;
                            std::string lastSave;
                            for(const auto & LC:line1){
                                if(TempSave.find(copy)!=std::string::npos&&(LC==' '||LC==','||LC=='='||LC==')')&&(TempSave.size()-copy.size())<=1){
                                    tokenVector[i].second=lastSave+" "+TempSave;
                                    break;
                                }
                                if(LC!=' '&&LC!='('&&LC!=','){
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
                            tokenVector[i].second=line1.substr(0,line1.find(' '))+" "+copy;
                            break;
                        }
                    }
                }
            }
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
                if(!forloop){
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
}
