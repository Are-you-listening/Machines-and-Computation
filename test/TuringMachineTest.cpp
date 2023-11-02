#include <gtest/gtest.h>
#include "TM/Tape.h"
#include "TM/TuringProduction.h"
class TuringMachineTest: public ::testing::Test {
protected:
    virtual void SetUp() {



    }
    virtual void TearDown() {
    }



};

TEST(TuringMachineTest, default_constructor) {

}

TEST(TuringMachineTest, tape) {
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
    t->write('e');
    ASSERT_EQ(t->getTapeData(), "[  cab  e    ]");
    t->moveHead(1);
    t->write('f');
    ASSERT_EQ(t->getTapeData(), "[  cab  ef   ]");
}

TEST(TuringMachineTest, tape_production) {
    TuringProduction* tp = new TuringProduction{};

    Production p;
    p.replace_val = {'b','b','b'};
    p.movement = {1,1,1};
    p.new_state = 'q';

    queue<char> q;
    for (int i=0; i<3; i++){
        q.push('a');
    }

    tp->addRoute(q, std::move(p));
    Production rep = tp->getProduction("aaa");
    for (char b: rep.replace_val){
        ASSERT_EQ(b, 'b');
    }
    for (int b: rep.movement){
        ASSERT_EQ(b, 1);
    }

    ASSERT_EQ(rep.new_state, "q");
}