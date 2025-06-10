//
// Created by ben on 10/06/25.
//

#ifndef PERFORMANTBOARD_H
#define PERFORMANTBOARD_H
#include "MoveList.h"
#include "magics/MagicSearch.h"


class PerformantBoard {
    bool whiteToMove;

    Bitboard whitePawns;
    Bitboard whiteRooks;
    Bitboard whiteKnights;
    Bitboard whiteBishops;
    Bitboard whiteQueens;
    Bitboard whiteKing;

    Bitboard blackPawns;
    Bitboard blackRooks;
    Bitboard blackKnights;
    Bitboard blackBishops;
    Bitboard blackQueens;
    Bitboard blackKing;

    Bitboard occ;

    AllMagicBitboards* magicBitboards;

public:
    PerformantBoard();
    void SetFEN(const std::string&);
    void SetMagicBitboards(AllMagicBitboards *bitboardSet);

    void UpdateOccupancy();

    int GetAllMoves(MoveList &moves);

    int GetQueenMoves(int piece_index, MoveList &moves);

    int GetRookMoves(int piece_index, MoveList &moves);

    int GetBishopMoves(int piece_index, MoveList &moves);

    int BitboardToMoveList(int piece_index, Bitboard bitboard, MoveList &moves);

    friend std::ostream& operator<<(std::ostream& os, const PerformantBoard& b);
};



#endif //PERFORMANTBOARD_H
