//
// Created by watson on 11/21/23.
//

#include <gtest/gtest.h>

#include "src/Tokenisation.h"
#include "lib/helper.h"
#include "src/Orchestrator.h"

using namespace std;

class CFGTokenTest: public ::testing::Test {
protected:
    virtual void SetUp() {}

    virtual void TearDown() {}
};

/**
  * GOAL: Test if a newly generated Token Vector is accepted by the generated CFG
  * Validating Plan: For a given .cpp File, create a tokenVector, convert to string and use CYK for membership of the CFG
 */

//General Function to be called

TEST(CFGTokenTest, t1) {
    //Orchestrator();

    //Define string/filenames

    //Tokenisation tokenVector;
    //string Filelocation="input/nestedExamples/engine.cc";
}
