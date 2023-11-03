#include <gtest/gtest.h>
#include "TM/Tape.h"
#include "TM/TuringMachine.h"
#include "TM/TuringProduction.h"
#include "lib/helper.h"
#include <memory>
class TuringMachineTest: public ::testing::Test {
protected:
    virtual void SetUp() {



    }
    virtual void TearDown() {
    }



};


TEST(TuringMachineTest, tape) {

    shared_ptr<Tape> t{new Tape{3}};
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
    shared_ptr<TuringProduction> tp{new TuringProduction{}};

    Production p;
    p.replace_val = {'b','b','b'};
    p.movement = {1,1,1};
    p.new_state = 'q';

    queue<char> q;
    for (int i=0; i<3; i++){
        q.push('a');
    }

    tp->addRoute(q, std::move(p));

    queue<char> qc;
    for (int i=0; i<3; i++){
        qc.push('a');
    }

    Production rep = tp->getProduction(qc);
    ASSERT_EQ(rep.replace_val.size(), 3);
    ASSERT_EQ(rep.movement.size(), 3);
    for (char b: rep.replace_val){
        ASSERT_EQ(b, 'b');
    }
    for (int b: rep.movement){
        ASSERT_EQ(b, 1);
    }

    ASSERT_EQ(rep.new_state, "q");
}

TEST(TuringMachineTest, tape_TM_construction) {
    TuringMachine t{"TestFiles/TM_1.json"};
    ASSERT_EQ(t.getTapeData(0),"[abcdefg                                                         ]");
    ASSERT_EQ(t.getTapeData(1),"[                                                                ]");
    t.move();
    ASSERT_EQ(t.getTapeData(0),"[ bcdefg                                                         ]");
    ASSERT_EQ(t.getTapeData(1),"[b                                                               ]");
    t.move();
}

TEST(TuringMachineTest, tape_TM_procedure) {
    ofstream output("output/TM_2.txt");

    TuringMachine t{"TestFiles/TM_2.json"};
    while (!t.isHalted()){
        t.move();
        output << t.getTapeData(0) << endl;
    }

    EXPECT_TRUE(FileCompare("output/TM_2.txt", "TestCompareFiles/TM_2.txt"));
}

TEST(TuringMachineTest, large_runtime_test) {
    ofstream output("output/TM_3.txt");

    TuringMachine t{"TestFiles/TM_3.json"};
    while (!t.isHalted()){

        for (int i = 0; i < t.getTapeAmount(); i++){
            output << t.getTapeData(i) << endl;
        }
        output << endl;

        t.move();
    }

    EXPECT_TRUE(FileCompare("output/TM_3.txt", "TestCompareFiles/TM_3.txt"));
}

TEST(TuringMachineTest, TM_4_test) {
    ofstream output("output/TM_4.txt");

    TuringMachine t{"TestFiles/TM_4.json"};
    while (!t.isHalted()){

        for (int i = 0; i < t.getTapeAmount(); i++){
            output << t.getTapeData(i) << endl;
        }
        output << endl;

        t.move();
    }

    EXPECT_TRUE(FileCompare("output/TM_4.txt", "TestCompareFiles/TM_4.txt"));
}