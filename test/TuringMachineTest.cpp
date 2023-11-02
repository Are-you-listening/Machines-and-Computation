#include <gtest/gtest.h>
#include "TM/Tape.h"
class TuringMachineTest: public ::testing::Test {
protected:
    virtual void SetUp() {



    }
    virtual void TearDown() {
    }



};

TEST(TuringMachineTest, default_constructor) {

}

TEST(TuringMachineTest, tape_constructor) {
    Tape* t = new Tape{};
    ASSERT_EQ(t->getTapeSize(), 20);
}