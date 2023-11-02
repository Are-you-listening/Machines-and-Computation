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
    Tape* t = new Tape{3};
    ASSERT_EQ(t->getTapeSize(), 3);
    ASSERT_EQ(t->getTapeData(), "[   ]");
    t->write('a');
    ASSERT_EQ(t->getTapeData(), "[a  ]");
    ASSERT_EQ(t->getSymbol(), 'a');
    t->moveHead(0);
    ASSERT_EQ(t->getSymbol(), 'a');
    t->moveHead(1);
    ASSERT_EQ(t->getSymbol(), '\0');
    t->write('b');
    ASSERT_EQ(t->getTapeData(), "[ab ]");
    t->moveHead(-2);
    t->write('c');
    ASSERT_EQ(t->getTapeData(), "[  cab ]");
    t->moveHead(5);
    t->write('d');
    ASSERT_EQ(t->getTapeData(), "[  cab  d    ]");
}