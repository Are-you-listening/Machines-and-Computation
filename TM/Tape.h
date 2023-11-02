//
// Created by tibov on 02/11/23.
//

#ifndef TOG_TAPE_H
#define TOG_TAPE_H
#include <optional>
#include <iostream>
#include "lib/DesignByContract.h"
using namespace std;
class Tape {
public:
    Tape();

    [[nodiscard]] unsigned long getTapeSize() const noexcept;

private:
    unsigned long tape_size;
    char* tape_ptr;
    char* tape_head;

    void increase_size(optional<unsigned long> optional_size);
};


#endif //TOG_TAPE_H
