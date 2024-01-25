//
// Created by watson on 10/11/23.
//

#include "src/Tokenisation.h"
#include "src/CFG.h"
#include "src/CFGConstructor.h"
#include "src/LALR.h"
#include "src/GUI/GUI.h"

#include <thread>
#include <string>

static unsigned int core_amount = std::thread::hardware_concurrency(); // Gets "core amount",
// in windows you can allocate infinite threads.
// In linux this isn't possible, I believe.
// So please care about this.

int main() {
    GUI g;
    return 0;

    bool tm = true;
    bool parse_tree = true;

    //Create tokenVector in parallel
    Tokenisation tokenVector; // sometimes variables in a nesting that should be passed in a function call aren't passed because it isn't found in the source file, this is done on purpose.
    std::string Filelocation="demo/knapsack.cpp";
    //std::string Filelocation="demo/combination.cpp";

    //std::string Filelocation="demo/forif.cpp";
    //std::string Filelocation="demo/ifElseAntinestingTM.cpp";
    //std::string Filelocation="demo/class.cpp";
    //std::string Filelocation="demo/main.cpp";
    //std::string Filelocation="demo/singletape.cpp";
    //std::string Filelocation="demo/variables.cpp";

    if (parse_tree){
        std::thread Tokenizer(&Tokenisation::Tokenize, &tokenVector, Filelocation); // i ignore rvalues in function calls
        core_amount--;

        //Construct GNF in parallel
        Orchestrator("");
        auto cfg = createCFG();
        cfg->toGNF();
        const CFG a = *cfg;
        LALR lalr(a);

        Tokenizer.join(); core_amount++; //Join both operations

        //create LARL parser with tokenvector
        auto vec = tokenVector.getTokenVector();

        auto cfg_cyk = createCFG();
        cfg_cyk->toCNF();

        string check_str;
        for (auto& v: vec){
            check_str += std::get<0>(v);
        }
        bool b = cfg_cyk->accepts(check_str);
        cout << "valid file " << b << endl;

        lalr.createTable();
        lalr.parse(vec);
        lalr.generate();
        Orchestrator::tabber(); //Cleanup output file; match tabs & spaces
        if(Config::getConfig()->isThreading()){
            ThreadFunction::threadFILE("result.cpp");
            Orchestrator::threadingTest(); //Perform a speed test
        }
    }

    if (tm){
        bool single_tape = false;

        ifstream in{Filelocation};
        string text_string;
        while (!in.eof()){
            char c = (char) in.get();
            if (c == '\377'){
                break;
            }
            text_string += c;
        }

        auto tm_b = new TMBuilder(4, Config::getConfig()->getIfElseNesting(), Config::getConfig()->getSplitNesting(), Config::getConfig()->getMaxNesting());
        auto data = tm_b->generateTM();
        delete tm_b;

        auto tm_machine = TuringMachine{};
        tm_machine.clear(true);
        tm_machine.load(data.states, data.start_state, data.input, data.tape_size, data.productions);
        tm_machine.load_input(text_string, 1);

        if (single_tape){
            tm_machine.clear(true);
            tm_machine.load(data.states, data.start_state, data.input, data.tape_size, data.productions);
            tm_machine.setSingleTape(false);
            tm_machine.load_input(text_string, 1);
            if (!tm_machine.isSingleTape()){
                tm_machine = *tm_machine.toSingleTape();
            }
        }

        for (int m=0; m<500000; m++){
            if (tm_machine.isHalted()){
                break;
            }

            tm_machine.move();
        }

        cout << endl;
        for (int i = 0; i < tm_machine.getTapeAmount(); i++){
            cout << tm_machine.getTapeData(i) << endl;
        }

        ofstream out{"output/result_TM.cpp"};
        out << tm_machine.exportTapeData(1);
        out.close();
    }
    return 0;
}