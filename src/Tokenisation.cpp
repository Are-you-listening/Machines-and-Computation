//
// Created by anass on 1-11-2023.
//

#include "Tokenisation.h"

/*
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
 */
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
/*
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
    bool skipCS= false;
    for(const char C: line){
        if(C=='\"'){
            if(skipCS){
                skipCS= false;
            } else{
                skipCS= true;
            }
        }
        if(skipCS){
            continue;
        }
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
 */


void vindVn(std::vector<std::tuple<std::string, std::string, std::set<std::string>>>& tokenTupleVector, const std::string& line, unsigned long int count2, std::set<char> VariableChar, std::set<char> VariableCharSecond){
    std::string tempV;
    unsigned long int count=0;
    bool append=false;
    bool SecondNotV=false;
    bool SecondNotVother=false;
    bool skipCS= false;
    for(const char C: line){
        if(C=='\"'){
            if(skipCS){
                skipCS= false;
            } else{
                skipCS= true;
            }
        }
        if(skipCS){
            continue;
        }
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
                    std::get<2>(tokenTupleVector[tokenTupleVector.size()-count2-1]).insert(tempV);
                    count++;
                    append= false;
                }
                tempV="";
                SecondNotV= true;
                continue;
            } else if(C=='-'){
                if(!SecondNotV){
                    std::get<2>(tokenTupleVector[tokenTupleVector.size()-count2-1]).insert(tempV);
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
                    std::get<2>(tokenTupleVector[tokenTupleVector.size()-count2-1]).insert(tempV);
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
    std::string line1;
    std::set<char> VariableChar= {'_','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
                                  'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
    std::set<char> VariableCharSecond= {'_','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
                                        'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
                                        '0','1','2','3','4','5','6','7','8','9'};
    std::string temp;
    while(getline(File,line1)){
        temp+=line1;
    }
    std::vector<std::string> lines;
    std::string temp2;
    bool include=false;
    for(const auto & C: temp){
        if(include){
            if(C=='>'||C=='"'){
                include=false;
                temp2+=C;
                lines.push_back(temp2);
                temp2.clear();
            }else{
                temp2+=C;
            }
            continue;
        }
        if(C=='{'){
            //temp2.push_back('{');
            lines.push_back(temp2);
            lines.emplace_back("{");
            temp2.clear();
        } else if(C=='}'){
            //temp2.push_back('}');
            lines.push_back(temp2);
            lines.emplace_back("}");
            temp2.clear();
        } else if(C==';'){
            temp2.push_back(';');
            lines.push_back(temp2);
            temp2.clear();
        } else if(C=='#'){
            include = true;
            temp2+=C;
        } else{
            temp2+=C;
        }
    }
    temp.clear();
    temp2.clear();
    
    for(auto & line : lines){
        unsigned long int tokenVectorSize=tokenVector.size();
        if(line.substr(0,5)=="std::"){
            line=line.substr(5,std::string::npos);
        }
        while(line.substr(0,1)==" "){
            line=line.substr(1,std::string::npos);
        }
        if(line.substr(0,2)=="//"){
            continue;
        }
        if(line.substr(0,3)=="for"){
            tokenVector.emplace_back("F",line);
        } else if(line.substr(0,2)=="if"){
            tokenVector.emplace_back("I",line);
        } else if(line.find("else if")!=std::string::npos){
            tokenVector.emplace_back("e",line);
        } else if(line.find("else")!=std::string::npos){
            tokenVector.emplace_back("E",line);
        } else if(line.find("while")!=std::string::npos){
            tokenVector.emplace_back("F",line);
        } else if((line.find('+')!=std::string::npos||line.find('-')!=std::string::npos||line.find('*')!=std::string::npos // this just gives on when a variable is used, not which or even which scope the variable is from
                   ||line.find('/')!=std::string::npos||line.find('%')!=std::string::npos||line.find("++")!=std::string::npos // so use in combination of (D,line) to deter the scopes.
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
        }
        if(line=="{"){
            tokenVector.emplace_back("{","{");
        } else if(line=="}"){
            tokenVector.emplace_back("}","}");
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
    unsigned long int count=0;
    for(auto it=tokenTupleVector.rbegin(); it!=tokenTupleVector.rend(); it++,count++){
        if(std::get<0>(*it)=="V"){
            vindVn(tokenTupleVector,std::get<1>(*it),count, VariableChar, VariableCharSecond);
            std::set<std::string> visited;
            for(auto it2=std::get<2>(*it).begin(); it2!=std::get<2>(*it).end();it2++){
                if(visited.find(*it2)!=visited.end()){
                    continue;
                }
                for(auto & it3 : tokenTupleVector){
                    if(std::get<1>(it3).find(*it2)!=std::string::npos){
                        if(std::get<0>(it3)=="V"){
                            std::get<0>(it3)="D";
                        }
                        std::string D0;
                        for(auto C=std::get<1>(it3).end()-std::get<1>(it3).size()-1+std::get<1>(it3).find(*it2); C!=std::get<1>(it3).begin();C--){
                            if(*C==' '||VariableCharSecond.find(*C)!=VariableCharSecond.end()||*C=='&'){
                                D0+=*C;
                            } else{
                                break;
                            }
                        }
                        std::reverse(D0.begin(),D0.end());
                        D0+=*it2;
                        std::get<2>(*it).erase(*it2);
                        if(D0.find(' ')!=std::string::npos){
                            std::get<2>(*it).insert(D0);
                        }
                        visited.insert(D0);
                        it2=std::get<2>(*it).begin();
                    }
                }
            }
            if(std::get<2>(*it).empty()){
                std::get<0>(*it)="C";
            }
        }
    }
    
    for(const auto & i:tokenTupleVector){
        std::cout<<std::get<0>(i)+", " << std::get<1>(i) << std::endl;
        for(const auto &V:std::get<2>(i)){
            std::cout<<V<< std::endl;
        }
    }
    tokenVector.clear();
}

const std::vector<std::tuple<std::string, std::string, std::set<std::string>>> &Tokenisation::getTokenVector() const {
    return tokenTupleVector;
}
