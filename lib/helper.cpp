//
// Created by watson on 10/6/23.
//
#include "helper.h"

void printVector(const string &name, const vector<string> &v){
    cout << name + " = {";
    for (int i = 0; i<v.size()-1; i++){
        cout << v[i]+", ";
    }
    cout << v[v.size()-1]+"}" << endl;
}

/**
Auxiliary functions for file manipulation.
*/
bool DirectoryExists(const std::string &dirname) {
    struct stat st;
    return stat(dirname.c_str(), &st) == 0;
}

bool FileExists(const std::string &filename) {
    struct stat st;
    if (stat(filename.c_str(), &st) != 0) return false;
    ifstream f(filename.c_str());
    if (f.good()) {
        f.close();
        return true;
    } else {
        f.close();
        return false;
    }
}

bool FileIsEmpty(const std::string &filename) {
    struct stat st;
    if (stat(filename.c_str(), &st) != 0) return true; // File does not exist; thus it is empty
    return st.st_size == 0;
}

bool FileCompare(const std::string &leftFileName, const std::string &rightFileName) {
    ifstream leftFile, rightFile;
    char leftRead, rightRead;
    bool result;

    // Open the two files.
    leftFile.open(leftFileName.c_str());
    if (!leftFile.is_open()) {
        return false;
    }
    rightFile.open(rightFileName.c_str());
    if (!rightFile.is_open()) {
        leftFile.close();
        return false;
    }

    result = true; // files exist and are open; assume equality unless a counterexamples shows up.
    while (result && leftFile.good() && rightFile.good()) {
        leftFile.get(leftRead);
        rightFile.get(rightRead);
        result = (leftRead == rightRead);
    }
    if (result) {
        // last read was still equal; are we at the end of both files ?
        result = (!leftFile.good()) && (!rightFile.good());
    }

    leftFile.close();
    rightFile.close();
    return result;
}

string getCurrTime(){
    //Code from https://stackoverflow.com/questions/16357999/current-date-and-time-as-string
    time_t rawtime;
    struct tm * timeInfo;
    char buffer[80];

    time (&rawtime);
    timeInfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeInfo);
    std::string str(buffer);
    return "["+str+"]";
}

void makeCombinations(vector<string>& input, long unsigned int length, vector<string>& current, vector<vector<string>>& result) {
    if (length == 0) {
        result.push_back(current);
        return;
    }

    for (const string& str : input) {
        current.push_back(str);
        makeCombinations(input, length - 1, current, result);
        current.pop_back();
    }
}

void printVVS(const vector<vector<string>>& vvs) {
    for (const vector<string>& t : vvs) {
        for (const string& str : t) {
            cout << str << ' ';
        }
        cout << endl;
    }
}

pair< vector<vector<string>> , vector<vector<string> >  > GenerateStateNames(const string &A,const string &input,const vector<string> &stacksymbols, const vector<vector<string>> &result){
    pair< vector<vector<string>> , vector<vector<string> >  > rules; // .first = Terminal Rules & .second = Other Rules (Also see construction Page 82 of CFG G)
    auto it= result.begin();

    while(it!=result.end()) {
        auto it2 = it->begin();
        if(it->empty()){
            if(stacksymbols.empty() || stacksymbols[0].empty()){
                rules.first.push_back({input});
            }else{
                rules.second.push_back({input,"["+A+","+stacksymbols[0]+","});
            }
            it++;
        }else{
            vector<string> temp;
            for (int i = 0; i < stacksymbols.size(); ++i) {
                if (i == 0) {
                    temp.push_back(input);
                    temp.push_back("[" + A+"," + stacksymbols[i] +","+ (it2[i]) + "]");
                } else {
                    if(i==stacksymbols.size()-1){
                        temp.push_back("[" + it2[i-1] +","+ stacksymbols[i] + ","); //End State will be added later on =)
                    }
                    else{
                        temp.push_back("[" + it2[i-1] +","+ stacksymbols[i] + "," + it2[i] + "]");
                    }
                }
            }

            it2++;
            it++;
            rules.second.push_back(temp);
        }
    }
    return rules;
}

void printVVSS(const vector<vector<set<string>>> &vvvs){
    char c = 125;

    for(auto vvs = vvvs.rbegin(); vvs!=vvvs.rend(); vvs++){
        string line="| ";
        for( auto &vs: *vvs){
            string set="{";
            for(const string &s: vs){
                set+=s+", ";
            }
            if(set.size()>1){
                set[set.size()-2]=c;
            }else{
                set+="}";
            }

            line+=set+" | "; //set
        }
        if(line!="| "){
            cout << line << endl;
        }
    }
}

vector<vector<string>> Cartesian(set<string> &a , set<string> &b){
    vector<vector<string>> temp{};

    if(a.empty() || b.empty()){
        return temp;
    }

    for(auto &A: a){
        for(auto &B: b){
            temp.push_back({A,B});
        }
    }
    return temp;
}
void Merge(set<vector<string>> &result, vector<vector<string>> &extra){
    for(auto &k: extra){
        result.insert(k);
    }
}

void Merge(vector<string> &result, vector<string> &extra){
    cout << "this merge doesn't yet? anything!!" << endl;
}

bool SubsetCheck(const vector<string> &uset, const vector<string> &subset){
    vector<bool> temp;

    for(auto &it: subset){
        temp.push_back(false);
        for(auto &u: uset){
            if(it==u){
                temp.pop_back();
            }
        }
    }
    return temp.empty();
}