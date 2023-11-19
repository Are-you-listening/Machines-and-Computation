//
// Created by watson on 10/6/23.
//

#ifndef TOI_HELPER_H
#define TOI_HELPER_H

#endif //TOI_HELPER_H

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sys/stat.h>
#include <ctime>
#include <set>
#include <algorithm>

using namespace std;

void printVector(const string &name, const vector<string> &v);

/**
 * \brief Check if the given directory exists
 * @param dirname
 * @return
 */
bool DirectoryExists(const std::string &dirname);

/**
 * \brief Check if a File is existing
 * @param dirname
 * @return
 */
bool FileExists(const std::string &dirname);

/**
 * \brief Check if a file is empty
 * @param filename
 * @return
 */
bool FileIsEmpty(const std::string &filename);

/**
 * \brief Compare 2 files
 * @param leftFileName
 * @param rightFileName
 * @return
 */
bool FileCompare(const std::string &leftFileName, const std::string &rightFileName);

/**
 * \brief Export Curr time to String
 * @return
 */
string getCurrTime();


/**
 * Helper function to print all options
 */
void printVVS(const vector<vector<string>>& vvs);

/**
 * Helper function to print all options
 */
void printVVSS(const vector<vector<set<string>>> &vvvs);

 /**
  * Makes combinations of given strings for a specified length
  * @param input
  * @param length
  * @param current
  *
  * @param result
  */
void makeCombinations(vector<string>& input, long unsigned int length, vector<string>& current, vector<vector<string>>& result);

/**
 * Generate all state name options for given inputs
 * @param A
 * @param input
 * @param stacksymbols
 * @param result
 * @return
 */
pair< vector<vector<string>> , vector<vector<string> >  > GenerateStateNames(const string &A,const string &input,const vector<string> &stacksymbols, const vector<vector<string>> &result);

/**
 * Make a Cartesian Products of given sets
 * @param a
 * @param b
 * @return
 */
vector<vector<string>>  Cartesian(set<string> &a , set<string> &b);

/**
 * Add non-existing ones to the current vector
 * @param result
 * @param extra
 */
void Merge(set<vector<string>> &result, vector<vector<string>> &extra);
void Merge(vector<string> &result, vector<string> &extra);

/**
 * Check if a given set is a subset of a given vector
 * @param uset
 * @param subset
 * @return
 */
bool SubsetCheck(const vector<string> &uset, const vector<string> &subset);