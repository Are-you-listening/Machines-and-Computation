//
// Created by tibov on 02/11/23.
//

#include "TuringMachine.h"

TuringMachine::TuringMachine(unsigned int tape_amount) {
    for (int i=0; i<tape_amount; i++){
        tapes.push_back(new Tape{64});
    }
}
