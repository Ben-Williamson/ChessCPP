//
// Created by ben on 13/05/25.
//

#ifndef BOARD_H
#define BOARD_H

#include <iosfwd>
#include <vector>

#include "Move.h"
#include "MoveList.h"
#include "magics/MagicSearch.h"

struct Piece {
    char code = 'E';
    bool isWhite = false;
};

class Board {
    Piece squares[64];
    bool whiteToMove;
    std::vector<Move> moveHistory;
    AllMagicBitboards* magicBitboards;

public:
    Board();

    void SetMagicBitboards(AllMagicBitboards *bitboardSet);

    int PieceValues();

    Board MakeMove(Move move);

    int MovePieces(Move move);

    void SetFEN(const std::string&);

    bool KingInCheck();

    bool CheckLegalMove(Move move);

    int GetPseudoMoves(std::vector<Move> &moves);

    int GetAllMoves(MoveList &moves);

    int GetPieceMoves(int, std::vector<Move>&);

    int GetPawnMoves(int piece_index, std::vector<Move> &moves);

    int GetRookMoves(int piece_index, std::vector<Move> &moves);

    int GetBishopMoves(int piece_index, std::vector<Move> &moves);

    int GetKnightMoves(int piece_index, std::vector<Move> &moves);

    int GetKingMoves(int piece_index, std::vector<Move> &moves);

    bool whiteToPlay() { return whiteToMove; };

    friend std::ostream& operator<<(std::ostream& os, const Board& b);
};



#endif //BOARD_H
