//
// Created by ben on 05/06/25.
//

#ifndef MOVELIST_H
#define MOVELIST_H

#include "Move.h"

struct MoveList {
    Move   list[256];   // 256 is an absolute maximum in chess
    int    size = 0;

    void clear() { size = 0; }
    void push(Move m) { list[size++] = m; }
};



#endif //MOVELIST_H
