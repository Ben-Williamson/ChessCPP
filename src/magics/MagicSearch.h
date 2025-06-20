//
// Created by ben on 06/06/25.
//

#ifndef MAGICSEARCH_H
#define MAGICSEARCH_H
#include <bitset>
#include <iostream>
#include <cstdint>

typedef std::uint64_t Bitboard;

struct MagicBitboardSet {
    uint64_t magics[64];
    Bitboard masks[64];
    Bitboard* attacks[64];
    int shifts[64];
};

struct LookupTables {
    Bitboard knightDestinations[64];
    Bitboard kingDestinations[64];
};

struct AllMagicBitboards {
    MagicBitboardSet rookMagicBitboard;
    MagicBitboardSet bishopMagicBitboard;
};

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

inline void printBitboard(const Bitboard& b) {
    for (int i = 56; i >= 0; i-=8) {
        const std::string bits = std::bitset<8>(b >> i).to_string();
        for (int j = 7; j >= 0; j--) {
            std::cout << bits[j];
        }
        std::cout << std::endl;
    }
}

class MagicSearch {
public:
    MagicSearch();

    int GenerateAllRookBlockers(uint64_t square, uint64_t *bitboards);

    int GenerateAllBlockers(uint64_t square, uint64_t *bitboards);

    uint64_t FindMagicForBitboards(int num_bitboards, const uint64_t *bitboards, int *shift);

    uint64_t CalculateRookMoves(uint64_t square, uint64_t blockers);

    uint64_t CalculateRookMask(uint64_t square);

    int GenerateAllBishopBlockers(Bitboard square, Bitboard *bitboards);

    Bitboard CalculateBishopMoves(Bitboard square, Bitboard blockers);

    Bitboard CalculateBishopMask(Bitboard square);

    void BuildRookDictionary(uint64_t search_square, uint64_t magic, int num_bitboards, const uint64_t *bitboards);

    MagicBitboardSet GenerateRookBitboardSet(bool regenerateMagics);

    MagicBitboardSet GenerateBishopBitboardSet(bool regenerateMagics);

    void PopulateKingMoves(LookupTables &lookupTables);

    void PopulateKnightMoves(LookupTables &lookupTables);

    LookupTables GenerateLookupTables();

    MagicBitboardSet GenerateBishopSet();

    int doit();
};



#endif //MAGICSEARCH_H
