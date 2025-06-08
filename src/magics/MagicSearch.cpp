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

// How does this work then?
// Generate every possible position for a rook
// Generate every possible combination of blockers for that rook
// Then do a * magic >> (64-n) and store the result.
// Increment magic if the result had been found before.

uint64_t random_uint64() {
    uint64_t u1, u2, u3, u4;
    u1 = (uint64_t)(random()) & 0xFFFF; u2 = (uint64_t)(random()) & 0xFFFF;
    u3 = (uint64_t)(random()) & 0xFFFF; u4 = (uint64_t)(random()) & 0xFFFF;
    return u1 | (u2 << 16) | (u3 << 32) | (u4 << 48);
}

MagicSearch::MagicSearch() {

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

    return move_destinations;
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

    return move_destinations;
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

int MagicSearch::doit() {
    GenerateRookBitboardSet();
}


// int MagicSearch::doit() {
//     Bitboard rookA1 = 1;
//
//     int bits_used = 12;
//
//     bool seen[1ULL << bits_used];
//     for (int i = 0; i < 1ULL << bits_used; i++) {
//         seen[i] = false;
//     }
//
//     int shift_amount = 64 - bits_used;
//
//     search_start:
//     uint64_t count = 0;
//     uint64_t magic = random_uint64() & random_uint64() & random_uint64();
//
//     int best = 0;
//
//
//     bool found = false;
//     while (not found) {
//         found = true;
//
//         for (int fileBlockers = 1; fileBlockers < 1ULL << 6; fileBlockers++) {
//             const Bitboard justFileBlockers = _pdep_u64(fileBlockers, 0x1010101010100);
//             for (int rankBlockers = 1; rankBlockers < 1ULL << 6; rankBlockers++) {
//                 const Bitboard rankAndFileBlockers = (rankBlockers << 1) | justFileBlockers;
//
//                 const uint64_t key = (rankAndFileBlockers * magic) >> shift_amount;
//
//                 count++;
//
//                 if (seen[key]) {
//                     if (count > best) {
//                         std::cout << std::hex << magic << " failed " << std::dec << count << std::endl;
//                         best = count;
//                     }
//                     found = false;
//                     magic = random_uint64() & random_uint64() & random_uint64();
//                     count = 0;
//                     for (int i = 0; i < 1ULL << 12; i++) {
//                         seen[i] = false;
//                     }
//                     goto endloop;
//                 }
//                 seen[key] = true;
//             }
//         }
//         endloop:
//     }
//
//     std::cout << "Found perfect magic: " << magic << " for shift " << shift_amount << std::endl;
//
//     return 0;
// }

// int MagicSearch::doit() {
//     Bitboard rookA1 = 1;
//
//     const int shift_amount = 52;
//
//     std::unordered_set<uint64_t> seen;
//     seen.reserve((1ULL << 6) * (1ULL << 6));
//
//     uint64_t count = 0;
//     uint64_t magic = 4620710844240757761;
//
//     for (int fileBlockers = 1; fileBlockers < 1ULL << 6; fileBlockers++) {
//         const Bitboard justFileBlockers = _pdep_u64(fileBlockers, 0x1010101010100);
//         for (int rankBlockers = 1; rankBlockers < 1ULL << 6; rankBlockers++) {
//             const Bitboard rankAndFileBlockers = (rankBlockers << 1) | justFileBlockers;
//
//             const uint64_t key = (rankAndFileBlockers * magic) >> shift_amount;
//             printBitboard(rankAndFileBlockers);
//             // std::cout << " key: " << key << std::endl;
//             count++;
//
//             if (seen.contains(key)) {
//                 std::cout << "INVALID MAGIC!" << std::endl;
//                 return 1;
//             }
//             seen.insert(key);
//         }
//     }
//     return 0;
// }


// Found perfect magic: 1 for shift 0
// Found perfect magic: 1 for shift 1
// Found perfect magic: 2 for shift 2
// Found perfect magic: 4 for shift 3
// Found perfect magic: 8 for shift 4
// Found perfect magic: 16 for shift 5
// Found perfect magic: 32 for shift 6
// Found perfect magic: 63 for shift 7
// Found perfect magic: 126 for shift 8
// Found perfect magic: 252 for shift 9
// Found perfect magic: 504 for shift 10
// Found perfect magic: 1008 for shift 11
// Found perfect magic: 2016 for shift 12
// Found perfect magic: 4032 for shift 13
// Found perfect magic: 8064 for shift 14
// Found perfect magic: 16127 for shift 15
// Found perfect magic: 32254 for shift 16
// Found perfect magic: 64508 for shift 17
// Found perfect magic: 129016 for shift 18
// Found perfect magic: 258032 for shift 19
// Found perfect magic: 516064 for shift 20
// Found perfect magic: 1032128 for shift 21
// Found perfect magic: 2064256 for shift 22
// Found perfect magic: 4128512 for shift 23
// Found perfect magic: 8257023 for shift 24
// Found perfect magic: 16514046 for shift 25
// Found perfect magic: 33028092 for shift 26
// Found perfect magic: 66056184 for shift 27
// Found perfect magic: 132112368 for shift 28
// Found perfect magic: 264224736 for shift 29
// Found perfect magic: 528449472 for shift 30
// Found perfect magic: 1056898944 for shift 31
// Found perfect magic: 2113797888 for shift 32
// Found perfect magic: 4227595776 for shift 33
// Found perfect magic: 8455191552 for shift 34
// Found perfect magic: 16910383104 for shift 35
// Found perfect magic: 33820766208 for shift 36
// Found perfect magic: 67641532416 for shift 37