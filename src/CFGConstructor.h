//
// Created by watson on 11/20/23.
//

#ifndef TOG_CFGCONSTRUCTOR_H
#define TOG_CFGCONSTRUCTOR_H

#include "Config.h"
#include "CFG.h"

#include <string>
#include <vector>
#include <map>

using namespace std;

/**
 * All used Terminals from Tokenisation
 * @param {
 * @param }
 * @param F, Function call
 * @param C, general Code
 * @param I, If Statement
 * @param E, Else Statement
 * @param e, else if Statement
 * @param D, Declaration
 * @param V, Variable
 */
static vector<string> T{"{","}", "F","C","I","E","e","D","V"};

/**
 * Create a CFG, accepting the Language made by Tokenisation, from the Config Data
 * @return
 */
CFG* createCFG();

#endif //TOG_CFGCONSTRUCTOR_H
