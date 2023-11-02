//
// Created by tibov on 02/11/23.
//

#ifndef TOG_TURINGMACHINE_H
#define TOG_TURINGMACHINE_H
#include "TM/Tape.h"
#include <vector>
#include <map>
class TuringMachine {
public:
    TuringMachine(unsigned int tape_amount);
private:
    vector<Tape*> tapes;


};


#endif //TOG_TURINGMACHINE_H
