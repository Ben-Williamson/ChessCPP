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

void PerformantBoard::SetLookupTables(LookupTables* lookup_tables) {
    lookupTables = lookup_tables;
}

int PerformantBoard::PieceValues() {
    int value = 0;

    for (Bitboard bb = whitePawns; bb; bb &= bb - 1) {
        int pos = __builtin_ctzll(bb);
        value += pawnValue + pawnSquares[56 - pos / 8 * 8 + pos % 8];
    }
    for (Bitboard bb = whiteRooks; bb; bb &= bb - 1) {
        int pos = __builtin_ctzll(bb);
        value += rookValue + rookSquares[56 - pos / 8 * 8 + pos % 8];
    }
    for (Bitboard bb = whiteKnights; bb; bb &= bb - 1) {
        int pos = __builtin_ctzll(bb);
        value += knightValue + knightSquares[56 - pos / 8 * 8 + pos % 8];
    }
    for (Bitboard bb = whiteBishops; bb; bb &= bb - 1) {
        int pos = __builtin_ctzll(bb);
        value += bishopValue + bishopSquares[56 - pos / 8 * 8 + pos % 8];
    }
    for (Bitboard bb = whiteQueens; bb; bb &= bb - 1) {
        int pos = __builtin_ctzll(bb);
        value += queenValue + queenSquares[56 - pos / 8 * 8 + pos % 8];
    }

    for (Bitboard bb = blackPawns; bb; bb &= bb - 1) {
        int pos = __builtin_ctzll(bb);
        pos = 63 - pos;
        value-= pawnValue + pawnSquares[56 - pos / 8 * 8 + pos % 8];
    }
    for (Bitboard bb = blackRooks; bb; bb &= bb - 1) {
        int pos = __builtin_ctzll(bb);
        pos = 63 - pos;
        value-= rookValue + rookSquares[56 - pos / 8 * 8 + pos % 8];
    }
    for (Bitboard bb = blackKnights; bb; bb &= bb - 1) {
        int pos = __builtin_ctzll(bb);
        pos = 63 - pos;
        value-= knightValue + knightSquares[56 - pos / 8 * 8 + pos % 8];
    }
    for (Bitboard bb = blackBishops; bb; bb &= bb - 1) {
        int pos = __builtin_ctzll(bb);
        pos = 63 - pos;
        value-= bishopValue + bishopSquares[56 - pos / 8 * 8 + pos % 8];
    }
    for (Bitboard bb = blackQueens; bb; bb &= bb - 1) {
        int pos = __builtin_ctzll(bb);
        pos = 63 - pos;
        value-= queenValue + queenSquares[56 - pos / 8 * 8 + pos % 8];
    }

    return whiteToMove ? value : -value;
}

void PerformantBoard::UpdateOccupancy() {
    whiteOcc = whitePawns | whiteRooks | whiteKnights | whiteBishops | whiteQueens | whiteKing;
    blackOcc = blackPawns | blackRooks | blackKnights | blackBishops | blackQueens | blackKing;

    occ = whiteOcc | blackOcc;
}

bool PerformantBoard::KingInCheck() {
    if (whiteToMove) return WhiteKingInCheck();
    return BlackKingInCheck();
}

bool PerformantBoard::WhiteKingInCheck() {
    // The king is in check if it has line of sight to a sliding piece,
    // or a knight could jump to it, or a pawn could take it.
    // We can check sliders and knights by imagining the king is a silder
    // or knight and seeing if it can take them.

    if (whiteKing == 0) return true;

    // Get king location
    const int kingLoc = __builtin_ctzll(whiteKing);

    // Helpers for using bitboards
    MagicBitboardSet* bbSet;
    int key;

    // Knights first:
    if (lookupTables->knightDestinations[kingLoc] & blackKnights)
        return true;

    // Now rooks/queens
    bbSet = &magicBitboards->rookMagicBitboard;
    key = (occ & ~whiteKing & bbSet->masks[kingLoc]) * bbSet->magics[kingLoc] >> bbSet->shifts[kingLoc];

    if (bbSet->attacks[kingLoc][key] & (blackQueens | blackRooks)) return true;

    // Now bishops/queens
    bbSet = &magicBitboards->bishopMagicBitboard;
    key = (occ & bbSet->masks[kingLoc]) * bbSet->magics[kingLoc] >> bbSet->shifts[kingLoc];
    if (bbSet->attacks[kingLoc][key] & (blackQueens | blackBishops)) return true;

    // Pawns
    const Bitboard aFile = 0x101010101010101;
    const Bitboard hFile = 0x8080808080808080;

    if ((whiteKing & ~aFile) << 7 & blackPawns) return true;
    if ((whiteKing & ~hFile) << 9 & blackPawns) return true;

    // King - this is only needed to stop the other king moving into check
    if (lookupTables->kingDestinations[kingLoc] & blackKing) return true;

    return false;
}

bool PerformantBoard::BlackKingInCheck() {
    // The king is in check if it has line of sight to a sliding piece,
    // or a knight could jump to it, or a pawn could take it.
    // We can check sliders and knights by imagining the king is a silder
    // or knight and seeing if it can take them.

    if (blackKing == 0) return true;

    // Get king location
    int kingLoc = __builtin_ctzll(blackKing);

    // Helpers for using bitboards
    MagicBitboardSet* bbSet;
    int key;

    // Knights first:
    if (lookupTables->knightDestinations[kingLoc] & whiteKnights)
        return true;

    // Now rooks/queens
    bbSet = &magicBitboards->rookMagicBitboard;
    key = (occ & bbSet->masks[kingLoc]) * bbSet->magics[kingLoc] >> bbSet->shifts[kingLoc];
    if (bbSet->attacks[kingLoc][key] & (whiteQueens | whiteRooks)) return true;

    // Now bishops/queens
    bbSet = &magicBitboards->bishopMagicBitboard;
    key = (occ & bbSet->masks[kingLoc]) * bbSet->magics[kingLoc] >> bbSet->shifts[kingLoc];
    if (bbSet->attacks[kingLoc][key] & (whiteQueens | whiteBishops)) return true;

    // Pawns
    const Bitboard aFile = 0x101010101010101;
    const Bitboard hFile = 0x8080808080808080;

    if ((blackKing & ~aFile) >> 9 & whitePawns) return true;
    if ((blackKing & ~hFile) >> 7 & whitePawns) return true;

    // King - this is only needed to stop the other king moving into check
    if (lookupTables->kingDestinations[kingLoc] & whiteKing) return true;

    return false;
}

void PerformantBoard::MakeMove(Move move) {
    UnmakeMove moveUnmaker{
        whitePawns, whiteRooks,
        whiteKnights, whiteBishops,
        whiteQueens, whiteKing,
        blackPawns, blackRooks,
        blackKnights, blackBishops,
        blackQueens, blackKing
    };

    unmakeStack.push(moveUnmaker);

    Bitboard fromMask = 1ULL << move.from;
    Bitboard toMask = 1ULL << move.to;

    Bitboard* bitboardPointers[12] = {
        &whitePawns, &whiteRooks, &whiteKnights, &whiteBishops, &whiteQueens, &whiteKing,
        &blackPawns, &blackRooks, &blackKnights, &blackBishops, &blackQueens, &blackKing
    };

    for (Bitboard* bitboard : bitboardPointers) {
        if (*bitboard & fromMask) {
            *bitboard = ((*bitboard) & ~fromMask) | toMask;
        }
        else if (*bitboard & toMask) {
            *bitboard &= ~toMask;
        }
    }

    whiteToMove = !whiteToMove;
    UpdateOccupancy();
}

void PerformantBoard::UndoMove() {
    UnmakeMove moveUnmaker = unmakeStack.top();
    unmakeStack.pop();

    whitePawns = moveUnmaker.prevWhitePawns;
    whiteRooks = moveUnmaker.prevWhiteRooks;
    whiteKnights = moveUnmaker.prevWhiteKnights;
    whiteBishops = moveUnmaker.prevWhiteBishops;
    whiteQueens = moveUnmaker.prevWhiteQueens;
    whiteKing = moveUnmaker.prevWhiteKing;

    blackPawns = moveUnmaker.prevBlackPawns;
    blackRooks = moveUnmaker.prevBlackRooks;
    blackKnights = moveUnmaker.prevBlackKnights;
    blackBishops = moveUnmaker.prevBlackBishops;
    blackQueens = moveUnmaker.prevBlackQueens;
    blackKing = moveUnmaker.prevBlackKing;

    whiteToMove = !whiteToMove;

    UpdateOccupancy();
}

int PerformantBoard::GetAllMoves(MoveList &moves) {
    MoveList pseudoMoves;
    int pseudoMovesFound = 0;
    int movesFound = 0;

    if (whiteToMove) {
        // for (int i = 0; i < 64; i++) {
        //     if (whiteOcc & (1ULL << i)) pseudoMovesFound += GetPieceMoves(i, pseudoMoves);
        // }
        for (Bitboard bb = whiteOcc; bb; bb &= bb-1) {
            int from = __builtin_ctzll(bb);
            pseudoMovesFound += GetPieceMoves(from, pseudoMoves);
        }
        for (int i = 0; i < pseudoMovesFound; i++) {
            MakeMove(pseudoMoves.list[i]);
            if (!WhiteKingInCheck()) {
                moves.push(pseudoMoves.list[i]);
                movesFound++;
            }
            UndoMove();
        }
    } else {
        for (Bitboard bb = blackOcc; bb; bb &= bb-1) {
            int from = __builtin_ctzll(bb);
            pseudoMovesFound += GetPieceMoves(from, pseudoMoves);
        }
        for (int i = 0; i < pseudoMovesFound; i++) {
            MakeMove(pseudoMoves.list[i]);
            if (!BlackKingInCheck()) {
                moves.push(pseudoMoves.list[i]);
                movesFound++;
            }
            UndoMove();
        }
    }
    return movesFound;
}

inline int PerformantBoard::GetPieceMoves(int piece_index, MoveList& moves) {
    const Bitboard mask = 1ULL << piece_index;

    if (mask & (whitePawns | blackPawns)) return GetPawnMoves(piece_index, moves);
    if (mask & (whiteRooks | blackRooks)) return GetRookMoves(piece_index, moves);
    if (mask & (whiteKnights | blackKnights)) return GetKnightMoves(piece_index, moves);
    if (mask & (whiteBishops | blackBishops)) return GetBishopMoves(piece_index, moves);
    if (mask & (whiteQueens | blackQueens)) return GetRookMoves(piece_index, moves) + GetBishopMoves(piece_index, moves);
    if (mask & (whiteKing | blackKing)) return GetKingMoves(piece_index, moves);

    // This is more correct in the case of an invalid piece index, but not the most performant.
    // Could just assume it's a king.
    return 0;
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

inline int PerformantBoard::GetKnightMoves(int piece_index, MoveList& moves) {
    return BitboardToMoveList(piece_index, lookupTables->knightDestinations[piece_index], moves);
}

inline int PerformantBoard::GetKingMoves(int piece_index, MoveList& moves) {
    return BitboardToMoveList(piece_index, lookupTables->kingDestinations[piece_index], moves);
}

inline int PerformantBoard::GetPawnMoves(int piece_index, MoveList& moves) {
    const Bitboard secondRank = 0xff00;
    const Bitboard seventhRank = 0xff000000000000;
    const Bitboard aFile = 0x101010101010101;
    const Bitboard hFile = 0x8080808080808080;
    Bitboard pawn = 1ULL << piece_index;
    Bitboard destinations = 0ULL;

    Bitboard whitePawn = pawn & whitePawns;

    // Can single push, if the square is empty
    Bitboard whiteSinglePush = (whitePawn << 8) & ~occ;
    // Can push again if the next square is empty
    whiteSinglePush |= (whitePawn & secondRank) << 16 & (whiteSinglePush << 8) & ~occ;
    destinations |= whiteSinglePush;

    // Can take to the left if not on the A file
    destinations |= (whitePawn & ~aFile) << 7 & blackOcc;

    // Can take to the right if not on the H file
    destinations |= (whitePawn & ~hFile) << 9 & blackOcc;

    Bitboard blackPawn = pawn & blackPawns;

    // Can single push, if the square is empty
    Bitboard blackSinglePush = (blackPawn >> 8) & ~occ;
    // Can push again, if the next square is empty
    blackSinglePush |= (blackPawn & seventhRank) >> 16 & (blackSinglePush >> 8) & ~occ;
    destinations |= blackSinglePush;

    // Can take to the left if not on the A file
    destinations |= (blackPawn & ~aFile) >> 9 & whiteOcc;

    // Can take to the right if not on the H file
    destinations |= (blackPawn & ~hFile) >> 7 & whiteOcc;

    return BitboardToMoveList(piece_index, destinations, moves);
}

inline int PerformantBoard::BitboardToMoveList(int piece_index, Bitboard bitboard, MoveList& moves) {
    if (1ULL << piece_index & whiteOcc) {
        bitboard &= ~whiteOcc;
    } else if (1ULL << piece_index & blackOcc) {
        bitboard &= ~blackOcc;
    }
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