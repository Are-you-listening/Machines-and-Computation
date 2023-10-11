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
