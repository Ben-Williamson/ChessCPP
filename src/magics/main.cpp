//
// Created by ben on 06/06/25.
//
#include <iostream>

#include "MagicSearch.h"


int main() {
    MagicSearch searcher;

    searcher.doit();

    AllMagicBitboards magicBitboards;

    magicBitboards.rookMagicBitboard = searcher.GenerateRookBitboardSet(true);
    magicBitboards.bishopMagicBitboard = searcher.GenerateBishopBitboardSet(true);

    for (int i = 0; i < 64; i++) {
        std::cout << magicBitboards.rookMagicBitboard.magics[i] << ", ";
        if (i % 4 == 3) std::cout << std::endl;
    }

    for (int i = 0; i < 64; i++) {
        std::cout << magicBitboards.rookMagicBitboard.shifts[i] << ", ";
        if (i % 8 == 7) std::cout << std::endl;
    }

    return 0;
}
