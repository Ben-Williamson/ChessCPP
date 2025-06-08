//
// Created by ben on 06/06/25.
//

#ifndef MAGICSEARCH_H
#define MAGICSEARCH_H
#include <bitset>
#include <iostream>
#include <bits/stdint-uintn.h>

typedef uint64_t Bitboard;

struct MagicBitboardSet {
    uint64_t magics[64];
    Bitboard masks[64];
    Bitboard* attacks[64];
    int shifts[64];
};

struct AllMagicBitboards {
    MagicBitboardSet rookMagicBitboard;
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

    void BuildRookDictionary(uint64_t search_square, uint64_t magic, int num_bitboards, const uint64_t *bitboards);

    MagicBitboardSet GenerateRookBitboardSet();

    int doit();
};



#endif //MAGICSEARCH_H
