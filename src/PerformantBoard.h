//
// Created by ben on 10/06/25.
//

#ifndef PERFORMANTBOARD_H
#define PERFORMANTBOARD_H
#include <stack>

#include "MoveList.h"
#include "magics/MagicSearch.h"
#include "Zobrist.h"

constexpr int pawnValue = 100;
constexpr int knightValue = 320;
constexpr int bishopValue = 330;
constexpr int rookValue = 500;
constexpr int queenValue = 900;

constexpr int pawnSquares[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,
   50, 50, 50, 50, 50, 50, 50, 50,
   10, 10, 20, 30, 30, 20, 10, 10,
    5,  5, 10, 25, 25, 10,  5,  5,
    0,  0,  0, 20, 20,  0,  0,  0,
    5, -5,-10,  0,  0,-10, -5,  5,
    5, 10, 10,-20,-20, 10, 10,  5,
    0,  0,  0,  0,  0,  0,  0,  0
};

constexpr int rookSquares[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    5, 10, 10, 10, 10, 10, 10,  5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    0,  0,  0,  5,  5,  0,  0,  0
};

constexpr int knightSquares[64] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50
};

constexpr int bishopSquares[64] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20
};

constexpr int queenSquares[64] = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
     -5,  0,  5,  5,  5,  5,  0, -5,
      0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};

constexpr int kingSquares[64] = {
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
     20, 20,  0,  0,  0,  0, 20, 20,
     20, 30, 10,  0,  0, 10, 30, 20
};

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

    uint64_t zobristHash;

    AllMagicBitboards* magicBitboards;
    LookupTables* lookupTables;
    Zobrist* zobristTables;

    std::stack<UnmakeMove> unmakeStack;

public:
    PerformantBoard();
    void SetFEN(const std::string&);
    void SetMagicBitboards(AllMagicBitboards *bitboardSet);

    void UpdateOccupancy();

    bool WhiteKingInCheck();

    bool BlackKingInCheck();

    bool GetWhiteToMove();

    void MakeMove(Move move);

    void UndoMove();

    uint64_t getZobrist();

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

    void SetZobristTables(Zobrist* zobrist_tables);

    int PieceValues() const;
};



#endif //PERFORMANTBOARD_H
