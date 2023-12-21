#include <gtest/gtest.h>
#include <iostream>
#include <thread>

#include "src/Tokenisation.h"
#include "src/ThreadFunction.h"
#include "filesystem"

static unsigned int core_amount = std::thread::hardware_concurrency();

class ThreadingTest: public ::testing::Test {
protected:
    virtual void SetUp() {}
    virtual void TearDown() {}
};

