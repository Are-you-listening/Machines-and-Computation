#include <gtest/gtest.h>
#include <memory>

#include "src/TM/Tape.h"
#include "src/TM/TuringMachine.h"
#include "src/TM/TransitieGenerators/TuringTokenizer.h"
#include "lib/helper.h"
#include "src/TM/Creation/TMBuilder.h"

class TuringMachineTest: public ::testing::Test {
protected:
    virtual void SetUp() {}

    virtual void TearDown() {}
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
    ASSERT_EQ(t->getTapeData(), "[         cab ]");
    t->moveHead(5);
    t->write('d');
    ASSERT_EQ(t->getTapeData(), "[         cab  d           ]");
    t->write('e');
    ASSERT_EQ(t->getTapeData(), "[         cab  e           ]");
    t->moveHead(1);
    t->write('f');
    ASSERT_EQ(t->getTapeData(), "[         cab  ef          ]");
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
    TuringMachine t{"../test/testFiles/TM_1.json"};
    ASSERT_EQ(t.getTapeData(0),"[abcdefg                                                         ]");
    ASSERT_EQ(t.getTapeData(1),"[                                                                ]");
    t.move();
    ASSERT_EQ(t.getTapeData(0),"[ bcdefg                                                         ]");
    ASSERT_EQ(t.getTapeData(1),"[b                                                               ]");
    t.move();
}

TEST(TuringMachineTest, tape_TM_procedure) {
    ofstream output("output/TM_2.txt");

    TuringMachine t{"../test/testFiles/TM_2.json"};
    while (!t.isHalted()){
        t.move();
        output << t.getTapeData(0) << endl;
    }

    EXPECT_TRUE(FileCompare("output/TM_2.txt", "../test/compareFiles/TM_2.txt"));
}

TEST(TuringMachineTest, large_runtime_test) {
    ofstream output("output/TM_3.txt");

    TuringMachine t{"../test/testFiles/TM_3.json"};
    while (!t.isHalted()){

        for (int i = 0; i < t.getTapeAmount(); i++){
            output << t.getTapeData(i) << endl;
        }
        output << endl;

        t.move();
    }

    EXPECT_TRUE(FileCompare("output/TM_3.txt", "../test/compareFiles/TM_3.txt"));
}

TEST(TuringMachineTest, TM_4_test) {
    ofstream output("output/TM_4.txt");

    TuringMachine t{"../test/testFiles/TM_4.json"};
    while (!t.isHalted()){

        for (int i = 0; i < t.getTapeAmount(); i++){
            output << t.getTapeData(i) << endl;
        }
        output << endl;

        t.move();
    }

    EXPECT_TRUE(FileCompare("output/TM_4.txt", "../test/compareFiles/TM_4.txt"));
}

TEST(TuringMachineTest, TM_tools){
    TuringTools* tools = TuringTools::getInstance(0);
    IncompleteSet s("start", "start");
    tools->go_to(s, {'E'}, 0, 1);
    vector<IncompleteTransition> results1 = s.transitions;
    ASSERT_EQ(results1.size(), 3);
    ASSERT_EQ(results1[1].def_move, 1);
    ASSERT_EQ(results1[1].input.size(), 0);
    ASSERT_EQ(results1[1].output.size(), 1);
    ASSERT_EQ(results1[2].def_move, 0);
    ASSERT_EQ(results1[2].input.size(), 1);
    ASSERT_EQ(results1[2].output.size(), 0);
    IncompleteSet s2(results1[1].state, results1[1].state);
    TuringTools::link(s, s2);
    auto link = s.transitions[s.transitions.size()-1];
    ASSERT_EQ(link.def_move, 0);
    ASSERT_EQ(link.input.size(), 0);
    ASSERT_EQ(link.output.size(), 0);
    TuringTools::link_put(s, s2, {'a'}, {0});
    link = s.transitions[s.transitions.size()-1];
    ASSERT_EQ(link.def_move, 0);
    ASSERT_EQ(link.input.size(), 0);
    ASSERT_EQ(link.output.size(), 1);
}

TEST(TuringMachineTest, TM_tokenazation) {
    int index = 7;
    ifstream test_file("../test/testFiles/TM_test_"+ to_string(index)+".cpp");
    string test_string;
    cout << test_file.is_open() << endl;
    while (!test_file.eof()){
        string line;
        getline(test_file, line);
        test_string  += line+'\n';
    }

    //test_string = "class base{}; class derived{}";
    //test_string = "class base{}; class derived{int test(){};int taco(){};}";
    // not in real use yet
    TuringTools::reset();
    auto t = new TMBuilder(4);
    TMBuilder_output data = t->generateTM();
    //ofstream o("output/TM_test.json");
    //o << data;
    TuringMachine tm;
    tm.load(data.states, data.start_state, data.input, data.tape_size, data.productions);
    tm.load_input(test_string, 1);

    int halted_time = -1;
    for (int i = 0; i<500000; i++){

        if (tm.isHalted()){
            if (halted_time == -1){
                halted_time = i;
            }

            break;
        }
        tm.move();
        if (tm.getCurrentState() == "4413"){

            int j=0;

        }

        if (i >= 37600000){
            int j = 0;

            for (int i = 0; i < tm.getTapeAmount(); i++){
                cout << tm.getTapeData(i) << endl;
            }
            cout << endl;
        }
    }

    cout << endl;
    for (int i = 0; i < tm.getTapeAmount(); i++){
        cout << tm.getTapeData(i) << endl;
    }

    for (int i = 0; i < tm.getTapeAmount(); i++){
        cout << "i " << tm.getTuringIndex(i) << endl;
    }

    cout << tm.getCurrentState() << std::endl;

    cout << "halted time " << halted_time << endl;

    string b = tm.exportTapeData(3);
    int a1 = 0;
    int a2 = 0;
    int a3 = 0;
    int a4 = 0;
    for (char c: b){
        if (c == '('){
            a1 += 1;
        }
        if (c == ')'){
            a2 += 1;
        }
        if (c == '{'){
            a3 += 1;
        }
        if (c == '}'){
            a4 += 1;
        }
    }
    std::cout << "(): " << a1 << " " << a2 << std::endl;
    std::cout << "{}: " << a3 << " " << a4 << std::endl;
    ofstream out{"../test/results/TM_test_"+ to_string(index)+".cpp"};
    out << tm.exportTapeData(1);
    out.close();
}


TEST(TuringMachineTest, TM_builder) {

    TuringTools::reset();
    auto t = new TMBuilder(4);
    TMBuilder_output data = t->generateTM();
    //ofstream o("output/TM_test.json");
    //o << data;
    TuringMachine tm;
    tm.load(data.states, data.start_state, data.input, data.tape_size, data.productions);

    for (int k=1; k<= 49; k++){
        ifstream test_file("../test/testFiles/TM_test_"+ to_string(k)+".cpp");
        string test_string;
        cout << test_file.is_open() << endl;
        while (!test_file.eof()){
            string line;
            getline(test_file, line);
            test_string  += line+'\n';
        }

        tm.clear();
        tm.load_input(test_string, 1);

        int halted_time = -1;
        for (int i = 0; i<1000200; i++){


            if (tm.isHalted()){
                if (halted_time == -1){
                    halted_time = i;
                }

                break;
            }
            tm.move();
        }

        for (int i = 0; i < tm.getTapeAmount(); i++){
            cout << tm.getTapeData(i) << endl;
        }

        cout << endl;
        cout << "halted time of " << to_string(k)<< ": " << halted_time << endl;

        ofstream out{"../test/results/TM_test_"+ to_string(k)+".cpp"};
        out << tm.exportTapeData(1);
        out.close();
        ASSERT_NE(halted_time, -1);
    }


}

TEST(TuringMachineTest, TM_single_tape) {
    TuringMachine tm;
    tm.load("../test/testFiles/TM_4.json");
    auto o = tm.toSingleTape();
    auto out = *o;

    int halted_time = -1;
    for (int i=0; i<1000; i++){
        if (out.isHalted()){
            if (halted_time == -1){
                halted_time = i;
            }

            break;
        }


        out.move();



    }

    for (int i = 0; i < out.getTapeAmount(); i++){
        cout << out.getTapeData(i) << endl;
    }

    for (int i = 0; i < out.getTapeAmount(); i++){
        cout << "i " << out.getTuringIndex(i) << endl;
    }


    cout << "current state " << out.getCurrentState() << std::endl;
    cout << "storage " << out.getControlStorage() << endl;
    cout << "halted time"<< ": " << halted_time << endl;
    cout << tm.getProductions().size() << endl;
    cout << out.getProductions().size() << endl;

}

TEST(TuringMachineTest, TM_single_tape_2){
    /*
    int index = 9;
    ifstream test_file("../test/testFiles/TM_test_"+ to_string(index)+".cpp");
    string test_string;
    cout << test_file.is_open() << endl;
    while (!test_file.eof()){
        string line;
        getline(test_file, line);
        test_string  += line+'\n';
    }

    TuringTools::reset();
    auto t = new TMBuilder(4);
    TMBuilder_output data = t->generateTM();
    //ofstream o("output/TM_test.json");
    //o << data;
    TuringMachine tm;
    tm.load(data.states, data.start_state, data.input, data.tape_size, data.productions);
    tm.load_input(test_string, 1);

    int size = 0;
    for (auto [k,v]: tm.getProductions()){
        size += v.size();
    }
    cout << size << endl;
    auto tw = tm.toSingleTape();
    tm = *tw;

    cout << "created" << endl;
    cout << tm.getProductions().size() << endl;

    int halted_time = -1;
    for (int i = 0; i<50000; i++){


        if (tm.isHalted()){
            if (halted_time == -1){
                halted_time = i;
            }

            break;
        }
        tm.move();


        if (i >= 246800000){
            int j = 0;

            for (int i = 0; i < tm.getTapeAmount(); i++){
                cout << tm.getTapeData(i) << endl;
            }
            cout << endl;
        }
    }

    cout << endl;
    for (int i = 0; i < tm.getTapeAmount(); i++){
        cout << tm.getTapeData(i) << endl;
    }

    for (int i = 0; i < tm.getTapeAmount(); i++){
        cout << "i " << tm.getTuringIndex(i) << endl;
    }

    cout << tm.getCurrentState() << std::endl;

    cout << "halted time " << halted_time << endl;

    string b = tm.exportTapeData(3);
    int a1 = 0;
    int a2 = 0;
    int a3 = 0;
    int a4 = 0;
    for (char c: b){
        if (c == '('){
            a1 += 1;
        }
        if (c == ')'){
            a2 += 1;
        }
        if (c == '{'){
            a3 += 1;
        }
        if (c == '}'){
            a4 += 1;
        }
    }
    std::cout << "(): " << a1 << " " << a2 << std::endl;
    std::cout << "{}: " << a3 << " " << a4 << std::endl;
    ofstream out{"../test/results/TM_test_"+ to_string(index)+".cpp"};
    out << tm.exportTapeData(1);
    out.close();*/

}