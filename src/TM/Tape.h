//
// Created by tibov on 02/11/23.
//

#ifndef TOG_TAPE_H
#define TOG_TAPE_H

#include <optional>
#include <iostream>
#include <cstdlib>

#include "lib/DesignByContract.h"

using namespace std;

class Tape {
public:
    Tape();
    ~Tape();
    explicit Tape(unsigned long init_size);

    virtual unsigned long getTapeSize() const noexcept;

    virtual string getTapeData() const noexcept;

    void write(char symbol);
    [[nodiscard]] char getSymbol() const noexcept;
    void moveHead(int move_direction);

    void load(const string& input);
    string exportTape(bool full=false);

    long getTapeHeadIndex() const;
    void clear();

private:
    unsigned long tape_size;
    char* tape_ptr;
    char* tape_head;
    long tape_head_index;

    void increase_size(bool front, optional<unsigned long> optional_size);
};

#endif //TOG_TAPE_H
