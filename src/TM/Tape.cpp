//
// Created by tibov on 02/11/23.
//

#include "Tape.h"

Tape::Tape(): Tape{20} {}

Tape::Tape(unsigned long init_size): tape_size{init_size} {
    tape_ptr = (char*) calloc(tape_size, 1);
    tape_head_index = 0;
    tape_head = tape_ptr;
}

void Tape::increase_size(bool front, optional<unsigned long> optional_size) {
    unsigned long new_size = tape_size*2;
    if (optional_size){
        new_size = optional_size.value()+tape_size;
    }

    if (new_size < tape_size){
        throw invalid_argument("new size is smaller");
    }

    char* new_tape_ptr = (char*) calloc(new_size, 1);

    char* old_head = tape_ptr;
    char* move_head = new_tape_ptr;

    if (!front) {
        //guarantees that we will not fill up the start
        tape_head_index += (long) (new_size - tape_size);
        move_head += (long) (new_size - tape_size);
    }

    for (int i=0; i<tape_size; i++){
        char c = *old_head;
        *move_head= c;

        old_head++;
        move_head++;
    }

    free(tape_ptr);

    char* new_head = new_tape_ptr+tape_head_index;
    tape_head = new_head;
    tape_ptr = new_tape_ptr;
    tape_size = new_size;
}

unsigned long Tape::getTapeSize() const noexcept{
    return tape_size;
}

string Tape::getTapeData() const noexcept {
    string output = "[";
    char* tape_reader = tape_ptr;
    for (int i=0; i<tape_size; i++){
        char c = *tape_reader;
        if (c == '\0'){
            c = ' ';
        }
        if (c == '\n'){
            c = 'N';
        }
        output += c;
        tape_reader++;
    }
    output += "]";
    return output;
}

void Tape::write(char symbol) {
    if (symbol == '\1'){
        return;
    }
    *tape_head = symbol;
}

void Tape::moveHead(int move_direction) {
    if (tape_head_index+move_direction < 0){
        increase_size(false, optional<unsigned long>(10));
    }

    if (tape_head_index+move_direction >= tape_size){
        increase_size(true, optional<unsigned long>());
    }

    tape_head_index += move_direction;
    tape_head += move_direction;
}

char Tape::getSymbol() const noexcept {
    return *tape_head;
}

void Tape::load(const string &input) {
    for (char i : input){
        write(i);
        moveHead(1);
    }
    moveHead(-(int) input.size());
}

Tape::~Tape() {
    free(tape_ptr);
}

string Tape::exportTape() {
    string output = "";
    char* tape_reader = tape_ptr;
    for (int i=0; i<tape_size; i++){
        char c = *tape_reader;
        if (c != '\0'){
            output += c;
        }

        tape_reader++;
    }
    return output;
}

long Tape::getTapeHeadIndex() const {
    return tape_head_index;
}

void Tape::clear() {
    free(tape_ptr);
    tape_size = 20;
    tape_ptr = (char*) calloc(tape_size, 1);
    tape_head_index = 0;
    tape_head = tape_ptr;


}
