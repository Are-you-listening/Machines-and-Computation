//
// Created by watson on 10/6/23.
//
#include "lib/helper.h"

void printVector(const string &name, const vector<string> &v){
    cout << name + " = {";
    for (int i = 0; i<v.size()-1; i++){
        cout << v[i]+", ";
    }
    cout << v[v.size()-1]+"}" << endl;
};

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
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,sizeof(buffer),"%d-%m-%Y %H:%M:%S",timeinfo);
    std::string str(buffer);
    return "["+str+"]";
}