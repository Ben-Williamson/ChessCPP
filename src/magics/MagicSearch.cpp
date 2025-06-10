//
// Created by ben on 06/06/25.
//

#include "MagicSearch.h"

#include <bitset>
#include <immintrin.h>
#include <cmath>
#include <stdint.h>
#include <iostream>
#include <unordered_set>


uint64_t random_uint64() {
    uint64_t u1, u2, u3, u4;
    u1 = (uint64_t)(random()) & 0xFFFF; u2 = (uint64_t)(random()) & 0xFFFF;
    u3 = (uint64_t)(random()) & 0xFFFF; u4 = (uint64_t)(random()) & 0xFFFF;
    return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}

MagicSearch::MagicSearch() {

}

uint64_t MagicSearch::FindMagicForBitboards(int num_bitboards, const Bitboard* bitboards, int* shift) {
    bool searching;
    uint64_t magic;

    int bits_in_key = (int)log2(num_bitboards);

    do {
        searching = false;
        bool* seen = (bool*)calloc(num_bitboards, sizeof(bool));

        magic = random_uint64() & random_uint64() & random_uint64();

        // int i = 0;
        // do {
        //     magic = random_uint64() & random_uint64() & random_uint64();
        //
        //     Bitboard top_bits = magic & 0xFF00000000000000ULL;
        //
        //     for (; top_bits != 0; i += (int)(top_bits & 1ULL))
        //         top_bits >>= 1;
        // } while (i < 6);

        for (int i = 0; i < num_bitboards; i++) {
            uint64_t key = (bitboards[i] * magic) >> (64 - bits_in_key);

            if (seen[key]) {
                searching = true;
                break;
            }
            seen[key] = true;
        }
        free(seen);
    } while (searching);

    *shift = 64 - bits_in_key;

    return magic;
}

int MagicSearch::GenerateAllRookBlockers(Bitboard square, Bitboard *bitboards) {
    const Bitboard outer_files = 0x8181818181818181;
    const Bitboard outer_ranks = 0xff000000000000ff;

    int myColumn = (int)std::log2(square) % 8;
    int myRow = (int)std::log2(square) / 8;

    Bitboard vertical_mask = 0;
    Bitboard horizontal_mask = 0;

    int num_vertical_blockers = 0;
    int num_horizontal_blockers = 0;

    // Fill in the whole column with 1s
    for (int i = 0; i < 8; i++) {
        Bitboard blocker = (1ULL << myColumn) << (i * 8);
        blocker &= ~square;
        blocker &= ~outer_ranks;

        vertical_mask |= blocker;
        num_vertical_blockers += blocker > 0;
    }

    // Fill in the whole row with 1s
    for (int i = 0; i < 8; i++) {
        Bitboard blocker = (1ULL << myRow * 8) << (i);
        blocker &= ~square;
        blocker &= ~outer_files;


        horizontal_mask |= blocker;
        num_horizontal_blockers += blocker > 0;
    }

    vertical_mask = vertical_mask & ~square & ~outer_ranks;
    horizontal_mask = horizontal_mask & ~square & ~outer_files;

    int count = 0;
    for (int fileBlockers = 0; fileBlockers < 1ULL << num_vertical_blockers; fileBlockers++) {
        const Bitboard justFileBlockers = _pdep_u64(fileBlockers, vertical_mask);


        for (int rankBlockers = 0; rankBlockers < 1ULL << num_horizontal_blockers; rankBlockers++) {
            const Bitboard rankAndFileBlockers = _pdep_u64(rankBlockers, horizontal_mask) | justFileBlockers;

            bitboards[count] = rankAndFileBlockers;
            count++;
        }
    }
    return count;
}

Bitboard MagicSearch::CalculateRookMoves(Bitboard square, Bitboard blockers) {
    const int square_col = (int)log2(square) % 8;
    const int square_row = (int)log2(square) / 8;

    Bitboard move_destinations = 0ULL;

    // Move back along row
    for (int i = square_col; i >= 0; i--) {
        Bitboard destination = (1ULL << i) << (square_row * 8);
        move_destinations |= destination;
        if (destination & blockers) break;
    }
    //Move forward along row
    for (int i = square_col; i < 8; i++) {
        Bitboard destination = (1ULL << i) << (square_row * 8);
        move_destinations |= destination;
        if (destination & blockers) break;
    }
    // Move down along col
    for (int i = square_row; i >= 0; i--) {
        Bitboard destination = (1ULL << square_col) << (i * 8);
        move_destinations |= destination;
        if (destination & blockers) break;
    }
    // Move up along col
    for (int i = square_row; i < 8; i++) {
        Bitboard destination = (1ULL << square_col) << (i * 8);
        move_destinations |= destination;
        if (destination & blockers) break;
    }

    return move_destinations & ~square;
}

Bitboard MagicSearch::CalculateRookMask(Bitboard square) {
    const int square_col = (int)log2(square) % 8;
    const int square_row = (int)log2(square) / 8;

    Bitboard move_destinations = 0ULL;

    // Fill the row, excluding a and h files.
    for (int i = 1; i < 7; i++) {
        Bitboard destination = (1ULL << i) << (square_row * 8);
        move_destinations |= destination;
    }
    // Fill the rank, excluding ranks 1 and 8.
    for (int i = 1; i < 7; i++) {
        Bitboard destination = (1ULL << square_col) << (i * 8);
        move_destinations |= destination;
    }

    return move_destinations & ~square;
}

int MagicSearch::GenerateAllBishopBlockers(Bitboard square, Bitboard *bitboards) {
    Bitboard mask = CalculateBishopMask(square);

    for (int i = 0; i < 1ULL << std::popcount(mask); i++) {
        bitboards[i] = _pdep_u64(i, mask);
    }

    return 1ULL << std::popcount(mask);
}

Bitboard MagicSearch::CalculateBishopMoves(Bitboard square, Bitboard blockers) {
    const Bitboard edge = 0xff818181818181ff;

    const int square_col = (int)log2(square) % 8;
    const int square_row = (int)log2(square) / 8;

    Bitboard move_destinations = 0ULL;

    for (int x_delta = -1; x_delta <= 1; x_delta+=2) {
        for (int y_delta = -1; y_delta <= 1; y_delta+=2) {
            for (int i = 1; i < 8; i++) {
                if (square_col + y_delta * i < 0) break;
                if (square_row + x_delta * i < 0) break;
                if (square_col + y_delta * i > 7) break;
                if (square_row + x_delta * i > 7) break;
                Bitboard destination = (1ULL << (square_col + y_delta * i)) << ((square_row + x_delta * i) * 8);
                move_destinations |= destination;

                if (destination & blockers || destination & edge) break;
            }
        }
    }
    return move_destinations & ~square;
}

Bitboard MagicSearch::CalculateBishopMask(Bitboard square) {
    Bitboard edge_of_board = 0xff818181818181ff;

    const int square_col = (int)log2(square) % 8;
    const int square_row = (int)log2(square) / 8;

    Bitboard move_destinations = 0ULL;

    for (int x_delta = -1; x_delta <= 1; x_delta+=2) {
        for (int y_delta = -1; y_delta <= 1; y_delta+=2) {
            for (int i = 1; i < 8; i++) {
                Bitboard destination = (1ULL << (square_col + y_delta * i)) << ((square_row + x_delta * i) * 8);
                move_destinations |= destination;
                if (destination & edge_of_board) break;
            }
        }
    }

    return move_destinations & ~square & ~edge_of_board;
}

MagicBitboardSet MagicSearch::GenerateRookBitboardSet() {
    MagicBitboardSet rookDictionary{};

    Bitboard* bitboards = new Bitboard[4096];
    int shift;

    for (int i = 0; i < 64; i++) {
        Bitboard search_square = 1ULL << i;
        int num_bitboards = GenerateAllRookBlockers(search_square, bitboards);
        uint64_t magic = FindMagicForBitboards(num_bitboards, bitboards, &shift);

        rookDictionary.masks[i]   = CalculateRookMask(search_square);
        rookDictionary.magics[i]  = magic;
        rookDictionary.shifts[i]  = shift;
        rookDictionary.attacks[i] = new Bitboard[num_bitboards];

        for (int j = 0; j < num_bitboards; j++) {
            int key = (bitboards[j] * magic) >> shift;

            rookDictionary.attacks[i][key] = CalculateRookMoves(search_square, bitboards[j]);
        }
    }
    delete[] bitboards;
    return rookDictionary;
}

MagicBitboardSet MagicSearch::GenerateBishopBitboardSet() {
    MagicBitboardSet bishopDictionary{};

    Bitboard* bitboards = new Bitboard[4096];
    int shift;

    for (int i = 0; i < 64; i++) {
        Bitboard search_square = 1ULL << i;
        int num_bitboards = GenerateAllBishopBlockers(search_square, bitboards);
        uint64_t magic = FindMagicForBitboards(num_bitboards, bitboards, &shift);

        bishopDictionary.masks[i]   = CalculateBishopMask(search_square);
        bishopDictionary.magics[i]  = magic;
        bishopDictionary.shifts[i]  = shift;
        bishopDictionary.attacks[i] = new Bitboard[num_bitboards];

        for (int j = 0; j < num_bitboards; j++) {
            int key = (bitboards[j] * magic) >> shift;

            bishopDictionary.attacks[i][key] = CalculateBishopMoves(search_square, bitboards[j]);
        }
    }
    delete[] bitboards;
    return bishopDictionary;
}

void MagicSearch::PopulateKingMoves(LookupTables& lookupTables) {
    int offsets[4] = {-1, 1, -8, 8};
    for (int i = 4; i < 64; i++) {
        Bitboard destinations = 0ULL;

        for (int offset : offsets) {
            if (mailbox[mailbox64[i] + offset] != -1) {
                destinations |= 1ULL << mailbox[mailbox64[i] + offset];
            }
        }

        printBitboard(destinations);
        exit(0);
    }
}

void MagicSearch::PopulateKnightMoves(LookupTables& lookupTables) {

}

LookupTables MagicSearch::GenerateLookupTables() {
    LookupTables lookupTables{};

    PopulateKingMoves(lookupTables);
    PopulateKnightMoves(lookupTables);

    return lookupTables;
}



int MagicSearch::doit() {
    GenerateRookBitboardSet();
    GenerateBishopBitboardSet();

    return 0;
}