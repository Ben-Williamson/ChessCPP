//
// Created by ben on 13/05/25.
//

#include "Board.h"

#include <iosfwd>
#include <iostream>
#include <map>

#include "MoveList.h"
#include "Profiler.h"

#include "magics/MagicSearch.h"

Board::Board() : magicBitboards(nullptr) {
}

void Board::SetMagicBitboards(AllMagicBitboards* bitboardSet) {
    magicBitboards = bitboardSet;
}

int Board::PieceValues() {
    int value = 0;

    std::map<char, int> pieceMap {
        {'P', 1},
        {'R', 5},
        {'N', 3},
        {'B', 3},
        {'K', 0},
        {'Q', 9},
        {'E', 0}
    };

    for (int i = 0; i < 64; i++) {
        value += squares[i].isWhite ? pieceMap[squares[i].code] : -pieceMap[squares[i].code];
    }

    return value;
}

Board Board::MakeMove(const Move move) {
    Board after_move = *this;
    after_move.squares[move.to] = after_move.squares[move.from];
    if (move.promotionPiece) {
       after_move.squares[move.to].code = move.promotionPiece;
    }
    after_move.squares[move.from] = Piece{'E', false};
    after_move.whiteToMove = !after_move.whiteToMove;
    after_move.moveHistory.push_back(move);
    after_move.SetMagicBitboards(magicBitboards);
    return after_move;
}

void Board::SetFEN(const std::string& FEN) {
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

        if (97 <= c && c <= 122) {
            // c is lower case
            squares[index] = {(char)(c-32), false};
        }
        if (65 <= c && c <= 90) {
            squares[index] = {c, true};
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
}

bool Board::KingInCheck() {
    int king_index = -1;
    for (int i = 0; i < 64; i++) {
        Piece piece = squares[i];
        if (piece.code == 'K' && piece.isWhite == whiteToMove) {
            king_index = i;
            break;
        }
    }

    Board after_move = *this;
    after_move.whiteToMove = !whiteToMove;

    std::vector<Move> opponent_moves;
    after_move.GetPseudoMoves(opponent_moves);

    for (Move om: opponent_moves) {
        if (om.to == king_index) return true;
    }
    return false;
}

bool Board::CheckLegalMove(Move move) {
    ZoneScoped;

    Board after_move = MakeMove(move);
    after_move.whiteToMove = !after_move.whiteToMove;

    return !after_move.KingInCheck();
}

int Board::GetPseudoMoves(std::vector<Move>& moves) {
    ZoneScoped;

    int pseudo_moves_found = 0;

    for (int index = 0; index < 64; index++) {
        if (squares[index].code != 'E' && squares[index].isWhite == whiteToMove) {
            pseudo_moves_found += GetPieceMoves(index, moves);
        }
    }

    return pseudo_moves_found;
}

int Board::GetAllMoves(MoveList& moves) {
    ZoneScoped;

    std::vector<Move> pseudo_moves;
    GetPseudoMoves(pseudo_moves);

    int moves_found = 0;
    for (Move m: pseudo_moves) {
        if (CheckLegalMove(m)) {
            moves.push(m);
            moves_found++;
        }
    }

    return moves_found;
}

int Board::GetPieceMoves(int piece_index, std::vector<Move>& moves) {
    Piece p = squares[piece_index];

    if (p.code == 'E') return 0;
    if (p.code == 'P') return GetPawnMoves(piece_index, moves);
    if (p.code == 'R') return GetRookMoves(piece_index, moves);
    if (p.code == 'B') return GetBishopMoves(piece_index, moves);
    if (p.code == 'Q') return GetBishopMoves(piece_index, moves) + GetRookMoves(piece_index, moves);
    if (p.code == 'N') return GetKnightMoves(piece_index, moves);
    if (p.code == 'K') return GetKingMoves(piece_index, moves);

    return -1;
}

int Board::GetPawnMoves(const int piece_index, std::vector<Move>& moves) {
    Piece p = squares[piece_index];
    int moves_found = 0;

    int movement_offset = p.isWhite ? -8 : 8;
    int new_index;

    // Pawn can move one step forward if the square is empty.
    bool isValid = false;

    new_index = piece_index + movement_offset;
    isValid |= (0 <= new_index);
    isValid &= (new_index < 64);
    isValid &= squares[new_index].code == 'E';

    if (isValid) {
        if (new_index / 8 == 0 && p.isWhite) {
            // White pawn reached end
            moves.push_back(Move(piece_index, new_index, 'N'));
            moves.push_back(Move(piece_index, new_index, 'B'));
            moves.push_back(Move(piece_index, new_index, 'R'));
            moves.push_back(Move(piece_index, new_index, 'Q'));
            moves_found+=4;
        }
        else if (new_index / 8 == 7 && !p.isWhite) {
            // Black pawn reached end
            moves.push_back(Move(piece_index, new_index, 'N'));
            moves.push_back(Move(piece_index, new_index, 'B'));
            moves.push_back(Move(piece_index, new_index, 'R'));
            moves.push_back(Move(piece_index, new_index, 'Q'));
            moves_found+=4;
        }
        else {
            moves.push_back(Move(piece_index, new_index));
            moves_found++;
        }

    }

    // Pawns can sometimes move forwards again, provided they are on the 2nd rank
    new_index = new_index + movement_offset;
    isValid &= (0 <= new_index);
    isValid &= (new_index < 64);
    isValid &= squares[new_index].code == 'E';
    isValid &= p.isWhite ? (piece_index/8 == 6) : (piece_index/8 == 1);

    if (isValid) {
        moves.push_back(Move(piece_index, new_index));
        moves_found++;
    }

    movement_offset = p.isWhite ? -7 : 7;
    new_index = piece_index + movement_offset;
    isValid = (0 <= new_index);
    isValid &= (new_index < 64);
    isValid &= new_index / 8 != piece_index / 8;
    isValid &= squares[new_index].code != 'E';
    isValid &= squares[new_index].isWhite != p.isWhite;

    if (isValid) {
        if (new_index / 8 == 0 && p.isWhite) {
            // White pawn reached end
            moves.push_back(Move(piece_index, new_index, 'N'));
            moves.push_back(Move(piece_index, new_index, 'B'));
            moves.push_back(Move(piece_index, new_index, 'R'));
            moves.push_back(Move(piece_index, new_index, 'Q'));
            moves_found+=4;
        }
        else if (new_index / 8 == 7 && !p.isWhite) {
            // Black pawn reached end
            moves.push_back(Move(piece_index, new_index, 'N'));
            moves.push_back(Move(piece_index, new_index, 'B'));
            moves.push_back(Move(piece_index, new_index, 'R'));
            moves.push_back(Move(piece_index, new_index, 'Q'));
            moves_found+=4;
        }
        else {
            moves.push_back(Move(piece_index, new_index));
            moves_found++;
        }
    }

    movement_offset = p.isWhite ? -9 : 9;
    new_index = piece_index + movement_offset;
    isValid = (0 <= new_index);
    isValid &= (new_index < 64);
    isValid &= abs(new_index / 8 - piece_index / 8) < 2;
    isValid &= squares[new_index].code != 'E';
    isValid &= squares[new_index].isWhite != p.isWhite;


    if (isValid) {
        if (new_index / 8 == 0 && p.isWhite) {
            // White pawn reached end
            moves.push_back(Move(piece_index, new_index, 'N'));
            moves.push_back(Move(piece_index, new_index, 'B'));
            moves.push_back(Move(piece_index, new_index, 'R'));
            moves.push_back(Move(piece_index, new_index, 'Q'));
            moves_found+=4;
        }
        else if (new_index / 8 == 7 && !p.isWhite) {
            // Black pawn reached end
            moves.push_back(Move(piece_index, new_index, 'N'));
            moves.push_back(Move(piece_index, new_index, 'B'));
            moves.push_back(Move(piece_index, new_index, 'R'));
            moves.push_back(Move(piece_index, new_index, 'Q'));
            moves_found+=4;
        }
        else {
            moves.push_back(Move(piece_index, new_index));
            moves_found++;
        }
    }


    return moves_found;
}

int Board::GetRookMoves(const int piece_index, std::vector<Move>& moves) {
    Piece p = squares[piece_index];
    int moves_found = 0;

    constexpr int offsets[4] = {-8, 8, -1, 1};
    bool isValid = false;

    for (int offset : offsets) {
        for (int i = 1; i < 8; i++) {
            int new_index = piece_index + offset * i;
            isValid = (0 <= new_index);
            isValid &= (new_index < 64);

            if (abs(offset) == 1) {
                isValid &= new_index / 8 == piece_index / 8;
            }
            if (abs(offset) == 8) {
                isValid &= new_index % 8 == piece_index % 8;
            }


            if (!isValid) {
                break;
            }
            if (squares[new_index].code == 'E') {
                // moves.push_back(Move(piece_index, new_index));
                // moves_found++;
            }
            else if (squares[new_index].isWhite != p.isWhite) {
                // moves.push_back(Move(piece_index, new_index));
                // moves_found++;
                break;
            } else {
                break;
            }
        }
    }

    Bitboard occ = 0;

    for (int i = 0; i < 64; i++) {
        // std::cout << (63-i) / 8 * 8 + (i % 8) << ' ';

        // if (i > 0 && i % 8 == 7) std::cout << std::endl;
        if (squares[i].code != 'E') {
            occ |= (1ULL << (63-i) / 8 * 8 + (i % 8));
        }
    }

    int square = ((63-piece_index) / 8 * 8 + (piece_index % 8));

    MagicBitboardSet* bitboards = &(magicBitboards->rookMagicBitboard);

    int key = ((occ & bitboards->masks[square] & ~(1ULL << square)) * bitboards->magics[square]) >> bitboards->shifts[square];

    Bitboard destinations = magicBitboards->rookMagicBitboard.attacks[square][key];


    int num_trailing;
    for (Bitboard bb = destinations; bb; bb &= bb - 1) {
        num_trailing = __builtin_ctzll(bb);

        int old_index_type =  (63-num_trailing) / 8 * 8 + num_trailing % 8;

        if (piece_index == old_index_type) continue;
        if (squares[old_index_type].code != 'E' && squares[old_index_type].isWhite == p.isWhite) continue;

        moves.push_back(Move(piece_index,  old_index_type));
        moves_found++;
    }

    return moves_found;
}

int Board::GetBishopMoves(const int piece_index, std::vector<Move>& moves) {
    Piece p = squares[piece_index];
    int moves_found = 0;

    constexpr int offsets[4] = {-7, 7, -9, 9};

    for (int offset : offsets) {
        int prev_new_index = piece_index;
        bool isValid = false;

        for (int i = 1; i < 8; i++) {
            int new_index = piece_index + offset * i;

            isValid = (0 <= new_index);
            isValid &= (new_index < 64);

            isValid &= abs(new_index % 8 - prev_new_index % 8) < 2;
            isValid &= abs(new_index / 8 - prev_new_index / 8) < 2;

            if (!isValid) {
                break;
            }

            if (squares[new_index].code == 'E') {
                moves.push_back(Move(piece_index, new_index));
                moves_found++;
            }
            else if (squares[new_index].isWhite != p.isWhite) {
                moves.push_back(Move(piece_index, new_index));
                moves_found++;
                break;
            } else {
                break;
            }

            prev_new_index = new_index;
        }
    }

    return moves_found;
}

int Board::GetKnightMoves(const int piece_index, std::vector<Move>& moves) {
    int moves_found = 0;
    Piece p = squares[piece_index];

    constexpr int mailbox[120] = {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
        -1,  8,  9, 10, 11, 12, 13, 14, 15, -1,
        -1, 16, 17, 18, 19, 20, 21, 22, 23, -1,
        -1, 24, 25, 26, 27, 28, 29, 30, 31, -1,
        -1, 32, 33, 34, 35, 36, 37, 38, 39, -1,
        -1, 40, 41, 42, 43, 44, 45, 46, 47, -1,
        -1, 48, 49, 50, 51, 52, 53, 54, 55, -1,
        -1, 56, 57, 58, 59, 60, 61, 62, 63, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
    };

    constexpr int mailbox64[64] = {
        21, 22, 23, 24, 25, 26, 27, 28,
        31, 32, 33, 34, 35, 36, 37, 38,
        41, 42, 43, 44, 45, 46, 47, 48,
        51, 52, 53, 54, 55, 56, 57, 58,
        61, 62, 63, 64, 65, 66, 67, 68,
        71, 72, 73, 74, 75, 76, 77, 78,
        81, 82, 83, 84, 85, 86, 87, 88,
        91, 92, 93, 94, 95, 96, 97, 98
    };

    constexpr int offsets[8] = {-17, 17, -15, 15, -6, 6, -10, 10};
    constexpr int mailbox_offsets[8] = { -21, 21, -19, 19, -8, 8, -12, 12 };

    bool isValid;

    for (int i = 0; i < 8; i++) {
        int offset = offsets[i];
        int mailbox_offset = mailbox_offsets[i];

        int new_index = piece_index + offset;
        isValid = (0 <= new_index);
        isValid &= (new_index < 64);
        isValid &= mailbox[mailbox64[piece_index] + mailbox_offset] != -1;

        if (squares[new_index].code != 'E') {
            if (p.isWhite) isValid &= squares[new_index].isWhite == false;
            else isValid &= squares[new_index].isWhite == true;
        }

        if (isValid) {
            moves.push_back(Move(piece_index, new_index));
            moves_found++;
        }

    }

    return moves_found;
}

int Board::GetKingMoves(const int piece_index, std::vector<Move>& moves) {
    Piece p = squares[piece_index];
    int moves_found = 0;

    constexpr int offsets[8] = {-9, -8, -7, -1, 1, 7, 8, 9};
    bool isValid = false;

    for (int offset : offsets) {
        int new_index = piece_index + offset;

        isValid = (0 <= new_index);
        isValid &= (new_index < 64);

        isValid &= abs(new_index % 8 - piece_index % 8) < 2;
        isValid &= abs(new_index / 8 - piece_index / 8) < 2;

        isValid &= squares[new_index].code != 'E' ? squares[new_index].isWhite != p.isWhite : true;

        if (isValid) {
            moves.push_back(Move(piece_index, new_index));
            moves_found++;
        }

    }

    return moves_found;
}

std::ostream& operator<<(std::ostream& os, const Board& b)
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

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            char code = b.squares[i*8+j].code;

            if (code != 'E') {
                if (b.squares[i*8+j].isWhite) {
                    os << whitePieceIcons[code] << ' ';
                } else {
                    os << blackPieceIcons[code] << ' ';
                }
            }
            else {
                os << "　 ";
            }
        }
        os << "    ";

        for (int j = 0; j < 8; j++) {
            if (i*8+j < 10) {
                os << i*8+j << " ";
            }
            else {
                os << i*8+j;
            }

            os << ' ';
        }

        if (i == 0) {
            os << "    " << (b.whiteToMove ? "White" : "Black") << " to move.";
        }

        if (i == 1) {
            os << "    ";
            for (Move m: b.moveHistory) {
                os << m << " ";
            }
        }

        os << '\n';
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const Move& m) {
    if (m.promotionPiece) {
        os << "Move(" << m.from << "->" << m.to << ", " << m.promotionPiece << ")";
        return os;
    }
    os << "Move(" << m.from << "->" << m.to << ")";
    return os;
}