//
// Created by ben on 05/06/25.
//

#ifndef MOVE_H
#define MOVE_H

#include <ostream>

#include "magics/MagicSearch.h"

struct Move {
    int from;
    int to;
    char promotionPiece;
    friend std::ostream& operator<<(std::ostream& os, const Move& m);
};

struct UnmakeMove {
    Bitboard prevWhitePawns;
    Bitboard prevWhiteRooks;
    Bitboard prevWhiteKnights;
    Bitboard prevWhiteBishops;
    Bitboard prevWhiteQueens;
    Bitboard prevWhiteKing;

    Bitboard prevBlackPawns;
    Bitboard prevBlackRooks;
    Bitboard prevBlackKnights;
    Bitboard prevBlackBishops;
    Bitboard prevBlackQueens;
    Bitboard prevBlackKing;
};

struct MoveEval {
    Move move;
    bool mateFound;
    float eval;
};

#endif //MOVE_H
