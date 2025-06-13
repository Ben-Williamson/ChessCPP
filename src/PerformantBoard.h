//
// Created by ben on 10/06/25.
//

#ifndef PERFORMANTBOARD_H
#define PERFORMANTBOARD_H
#include <stack>

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

    Bitboard whiteOcc;
    Bitboard blackOcc;
    Bitboard occ;

    AllMagicBitboards* magicBitboards;
    LookupTables* lookupTables;

    std::stack<UnmakeMove> unmakeStack;

public:
    PerformantBoard();
    void SetFEN(const std::string&);
    void SetMagicBitboards(AllMagicBitboards *bitboardSet);

    void UpdateOccupancy();

    bool WhiteKingInCheck();

    bool BlackKingInCheck();

    void MakeMove(Move move);

    void UndoMove();

    bool KingInCheck();

    int GetAllMoves(MoveList &moves);

    int GetPieceMoves(int piece_index, MoveList &moves);

    int GetQueenMoves(int piece_index, MoveList &moves);

    int GetRookMoves(int piece_index, MoveList &moves);

    int GetBishopMoves(int piece_index, MoveList &moves);

    int GetKnightMoves(int piece_index, MoveList &moves);

    int GetKingMoves(int piece_index, MoveList &moves);

    int GetPawnMoves(int piece_index, MoveList &moves);

    int BitboardToMoveList(int piece_index, Bitboard bitboard, MoveList &moves);

    friend std::ostream& operator<<(std::ostream& os, const PerformantBoard& b);

    void SetLookupTables(LookupTables *lookup_tables);

    int PieceValues();
};



#endif //PERFORMANTBOARD_H
