//
// Created by tibov on 02/11/23.
//

#include <cstdlib>
#include "Tape.h"

Tape::Tape(): tape_size{20} {
    tape_ptr = (char*) calloc(tape_size, 1);
    tape_head = tape_ptr;
}

void Tape::increase_size(optional<unsigned long> optional_size) {
    unsigned long new_size = tape_size*2;
    if (optional_size){
        new_size = optional_size.value();
    }

    if (new_size < tape_size){
        throw invalid_argument("new size is smaller");
    }

    char* new_tape_ptr = (char*) calloc(new_size, 1);

    char* old_head = tape_ptr;
    char* move_head = new_tape_ptr;
    char* new_head = nullptr;

    for (int i=0; i<tape_size; i++){
        char c = *old_head;
        *move_head= c;

        if (old_head == tape_head){
            new_head = new_tape_ptr;
        }

        old_head++;
        move_head++;
    };

    tape_head = new_head;

    free(tape_ptr);

}

unsigned long Tape::getTapeSize() const noexcept{
    return tape_size;
}
