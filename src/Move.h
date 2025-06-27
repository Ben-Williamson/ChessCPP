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

    std::string CoordsToBoardSquares() const {
        return {
            static_cast<char>('a' + (from & 7)),
            static_cast<char>('1' + (from >> 3)),
            static_cast<char>('a' + (to & 7)),
            static_cast<char>('1' + (to >> 3))
        };
    }

    static int coordToSquare(std::string_view coord) {
        return (coord[1] - '1') * 8    // rank
            + (coord[0] - 'a');       // file
    }

    static Move fromUci(std::string_view uci) {
        Move m;
        m.from = coordToSquare(uci.substr(0, 2));
        m.to = coordToSquare(uci.substr(2, 2));
        m.promotionPiece = (uci.size() == 5) ? uci[4] - 32 : '\0';
        return m;
    }
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
