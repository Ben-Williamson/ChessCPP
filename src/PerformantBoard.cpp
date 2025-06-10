//
// Created by ben on 10/06/25.
//

#include "PerformantBoard.h"

#include <map>

#include "MoveList.h"


PerformantBoard::PerformantBoard() :
    whitePawns(0ULL), whiteRooks(0ULL), whiteKnights(0ULL), whiteBishops(0ULL),
    whiteQueens(0ULL), whiteKing(0ULL),
    blackPawns(0ULL), blackRooks(0ULL), blackKnights(0ULL), blackBishops(0ULL),
    blackQueens(0ULL), blackKing(0ULL),
    occ(0ULL)
{
}

void PerformantBoard::SetFEN(const std::string& FEN) {
    int index = 0;
    int string_index = 0;

    while (true) {
        char c = FEN[string_index];
        string_index++;
        if (c == ' ') {
            break;
        }
        if (c == '/') {
            continue;
        }

        int shift_amount = 56 - (index / 8 * 8) + index % 8;

        switch (c) {
            case 'P':
                whitePawns |= 1ULL << shift_amount;
                break;
            case 'R':
                whiteRooks |= 1ULL << shift_amount;
                break;
            case 'N':
                whiteKnights |= 1ULL << shift_amount;
                break;
            case 'B':
                whiteBishops |= 1ULL << shift_amount;
                break;
            case 'Q':
                whiteQueens |= 1ULL << shift_amount;
                break;
            case 'K':
                whiteKing |= 1ULL << shift_amount;
                break;
            case 'p':
                blackPawns |= 1ULL << shift_amount;
                break;
            case 'r':
                blackRooks |= 1ULL << shift_amount;
                break;
            case 'n':
                blackKnights |= 1ULL << shift_amount;
                break;
            case 'b':
                blackBishops |= 1ULL << shift_amount;
                break;
            case 'q':
                blackQueens |= 1ULL << shift_amount;
                break;
            case 'k':
                blackKing |= 1ULL << shift_amount;
                break;
            default:
                break;
        }


        if (48 <= c && c <= 57) {
            // c is a number.
            index += (c - 48);
        }
        else {
            index++;
        }
    }
    whiteToMove = FEN[string_index] == 'w';

    UpdateOccupancy();
}

void PerformantBoard::SetMagicBitboards(AllMagicBitboards *bitboardSet) {
    magicBitboards = bitboardSet;
}

void PerformantBoard::UpdateOccupancy() {
    occ = 0ULL;
    occ |= whitePawns | whiteRooks | whiteKnights | whiteBishops | whiteQueens | whiteKing;
    occ |= blackPawns | blackRooks | blackKnights | blackBishops | whiteQueens | whiteKing;
}

int PerformantBoard::GetAllMoves(MoveList& moves) {
    return GetRookMoves(42, moves);
}

inline int PerformantBoard::GetQueenMoves(int piece_index, MoveList& moves) {
    return GetRookMoves(piece_index, moves) + GetBishopMoves(piece_index, moves);
}

inline int PerformantBoard::GetRookMoves(int piece_index, MoveList& moves) {
    MagicBitboardSet* bbSet = &magicBitboards->rookMagicBitboard;

    int key = (occ & bbSet->masks[piece_index]) * bbSet->magics[piece_index] >> bbSet->shifts[piece_index];
    return BitboardToMoveList(piece_index, bbSet->attacks[piece_index][key], moves);
}

inline int PerformantBoard::GetBishopMoves(int piece_index, MoveList& moves) {
    MagicBitboardSet* bbSet = &magicBitboards->bishopMagicBitboard;

    int key = (occ & bbSet->masks[piece_index]) * bbSet->magics[piece_index] >> bbSet->shifts[piece_index];
    return BitboardToMoveList(piece_index, bbSet->attacks[piece_index][key], moves);
}

inline int PerformantBoard::BitboardToMoveList(int piece_index, Bitboard bitboard, MoveList& moves) {
    int num_trailing;
    int found = 0;
    for (Bitboard bb = bitboard; bb; bb &= bb - 1) {
        num_trailing = __builtin_ctzll(bb);

        moves.push(Move{piece_index, num_trailing});
        found++;
    }
    return found;
}

std::ostream& operator<<(std::ostream& os, const PerformantBoard& b)
{
    std::map<char, std::string> whitePieceIcons = {
        {'K', "♔"},
        {'R', "♖"},
        {'N', "♘"},
        {'B', "♗"},
        {'Q', "♕"},
        {'P', "♙"},
    };
    std::map<char, std::string> blackPieceIcons = {
        {'K', "♚"},
        {'R', "♜"},
        {'N', "♞"},
        {'B', "♝"},
        {'Q', "♛"},
        {'P', "♟"},
    };

    std::string empty = ". ";
    for (int i = 7; i >= 0; i--) {
        for (int j = 0; j < 8; j++) {
            Bitboard mask = (1ULL << j) << (i * 8);

            if (b.whitePawns & mask) {
                os << "P ";
            } else if (b.whiteRooks & mask) {
                os << "R ";
            } else if (b.whiteKnights & mask) {
                os << "N ";
            } else if (b.whiteBishops & mask) {
                os << "B ";
            } else if (b.whiteQueens & mask) {
                os << "Q ";
            } else if (b.whiteKing & mask) {
                os << "K ";
            } else if (b.blackPawns & mask) {
                os << "p ";
            } else if (b.blackRooks & mask) {
                os << "r ";
            } else if (b.blackKnights & mask) {
                os << "n ";
            } else if (b.blackBishops & mask) {
                os << "b ";
            } else if (b.blackQueens & mask) {
                os << "q ";
            } else if (b.blackKing & mask) {
                os << "k ";
            }
            else {
                os << empty;
            }
        }

        os << "    ";

        for (int j = 0; j < 8; j++) {
            if (i*8+j < 10) os << "0";
            os << i*8 + j << " ";
        }

        if (i == 7) {
            os << "    " << (b.whiteToMove ? "White" : "Black") << " to move.";
        }

        os << std::endl;
    }

    return os;
}