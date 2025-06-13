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

uint64_t knownBishopMagics[64] = {
    9585384058465157188ULL, 2535755167694856ULL, 4630300751376679426ULL, 9242569529203165218ULL,
    36328139059648512ULL, 9512174167777362688ULL, 4630264479702057088ULL, 36103581006824000ULL,
    2596329720798980352ULL, 1585280709786140706ULL, 180188000997638208ULL, 10385736439403906564ULL,
    145157546573826ULL, 666533587203391498ULL, 1387830312818458626ULL, 35324093009984ULL,
    364791587265480704ULL, 1162069587412521472ULL, 2454461831318743305ULL, 1171076743717601280ULL,
    113153095334232448ULL, 18578469466214657ULL, 4758123521124598787ULL, 36101408341361664ULL,
    578717022105371168ULL, 295271651032172547ULL, 9223416017589452864ULL, 6764204126117920ULL,
    18441009063010336ULL, 436884897999653000ULL, 1126174801596420ULL, 10412394910559832340ULL,
    4541018055641088ULL, 184855409721856ULL, 1735013989827543104ULL, 9385506038703718928ULL,
    1191501185786347552ULL, 9011601600422016ULL, 2319356559030817808ULL, 18300273713807440ULL,
    1128238784972292ULL, 4612957058299879428ULL, 681698484307968ULL, 1153010033011729408ULL,
    10275142923979904ULL, 4908924212317524240ULL, 117243184297410689ULL, 2459356840140473408ULL,
    142943493030401ULL, 285877335491668ULL, 38691549478912ULL, 8757839874ULL,
    54080166644944896ULL, 2305935387117617216ULL, 9270695051682580480ULL, 288379914030154240ULL,
    2990671825387784194ULL, 9241387028091765250ULL, 2568460269834240ULL, 292734113722450946ULL,
    2342576318847796224ULL, 337958813812916800ULL, 1459887645063925824ULL, 1172062971824800000ULL
};

uint64_t knownRookMagics[64] = {
    2341872700126921088ULL, 36063999388811394ULL, 36046404774332544ULL, 9295434166529298432ULL,
    3602884108533893280ULL, 2017759984801030184ULL, 9295442885161878784ULL, 2377900746143170690ULL,
    4614078557879025664ULL, 45176871473643648ULL, 2306265290400334209ULL, 1765551825785786368ULL,
    2314287277306024448ULL, 140892115567104ULL, 10522942008623235204ULL, 3531385097024569476ULL,
    2630173651193856002ULL, 1729664832472359040ULL, 4612251167674877056ULL, 72567903752704ULL,
    4645986450278400ULL, 4901184681308848640ULL, 1161968286306405896ULL, 164383585432961100ULL,
    1226175399508083237ULL, 9077572296016144ULL, 281754151682068ULL, 9227946151256263170ULL,
    5779037411607808ULL, 12385193180659840ULL, 327130049571606664ULL, 2392539449540864ULL,
    70371168485408ULL, 4638710090633842689ULL, 2308270733043724288ULL, 18023196791934976ULL,
    83599237606017024ULL, 3096241940480520ULL, 9565799437150716432ULL, 9225624387531571460ULL,
    324435369935405056ULL, 9241474397905633285ULL, 9448833630777442320ULL, 17451586132508736ULL,
    54051991688609920ULL, 1153486138255212560ULL, 4613656918790045708ULL, 4574260740096001ULL,
    4620974968283333376ULL, 4665887695027897088ULL, 576602383147075712ULL, 288375546047365760ULL,
    576601506989998208ULL, 9259719694393540736ULL, 4611703645275227136ULL, 9223512916077596800ULL,
    108122125201576641ULL, 1697787688288706ULL, 2319388993542263041ULL, 2598612465718136833ULL,
    2306405994072395778ULL, 2533291974592517ULL, 216454308640980993ULL, 292060430594ULL
};

int knownBishopShifts[64] = {
    58, 59, 59, 59, 59, 59, 59, 58,
    59, 59, 59, 59, 59, 59, 59, 59,
    59, 59, 57, 57, 57, 57, 59, 59,
    59, 59, 57, 55, 55, 57, 59, 59,
    59, 59, 57, 55, 55, 57, 59, 59,
    59, 59, 57, 57, 57, 57, 59, 59,
    59, 59, 59, 59, 59, 59, 59, 53,
    58, 59, 59, 59, 59, 59, 59, 58
};

int knownRookShifts[64] = {
    52, 53, 53, 53, 53, 53, 53, 52,
    53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53,
    52, 53, 53, 53, 53, 53, 53, 52,
};

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

MagicBitboardSet MagicSearch::GenerateRookBitboardSet(bool regenerateMagics) {
    MagicBitboardSet rookDictionary{};

    Bitboard* bitboards = new Bitboard[4096];
    uint64_t magic;
    int shift;

    for (int i = 0; i < 64; i++) {
        Bitboard search_square = 1ULL << i;
        int num_bitboards = GenerateAllRookBlockers(search_square, bitboards);
        if (regenerateMagics) {
            magic = FindMagicForBitboards(num_bitboards, bitboards, &shift);
        } else {
            magic = knownRookMagics[i];
            shift = knownRookShifts[i];
        }

        rookDictionary.masks[i]   = CalculateRookMask(search_square);
        rookDictionary.magics[i]  = magic;
        rookDictionary.shifts[i]  = shift;
        rookDictionary.attacks[i] = new Bitboard[num_bitboards];

        for (int j = 0; j < num_bitboards; j++) {
            int key = (bitboards[j] * magic) >> shift;

            // if (bitboards[j] == 16852480) {
            //     std::cout << "FOUND IT" << std::endl;
            //     std::cout << i << ", " << key << std::endl;
            //     printBitboard(CalculateRookMoves(search_square, bitboards[j]));
            // }
            //
            // if (i == 8 && key == 678) {
            //     std::cout << "KEY USED" << std::endl;
            // }

            rookDictionary.attacks[i][key] = CalculateRookMoves(search_square, bitboards[j]);
        }
    }
    delete[] bitboards;
    return rookDictionary;
}

MagicBitboardSet MagicSearch::GenerateBishopBitboardSet(bool regenerateMagics) {
    MagicBitboardSet bishopDictionary{};

    Bitboard* bitboards = new Bitboard[4096];
    uint64_t  magic;
    int shift;

    for (int i = 0; i < 64; i++) {
        Bitboard search_square = 1ULL << i;
        int num_bitboards = GenerateAllBishopBlockers(search_square, bitboards);
        if (regenerateMagics) {
            magic = FindMagicForBitboards(num_bitboards, bitboards, &shift);
        } else {
            magic = knownBishopMagics[i];
            shift = knownBishopShifts[i];
        }

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
    const int kingOffsets[8] = { -1, 1, -10, 10, -11, 11, -9, 9 };

    for (int i = 0; i < 64; i++) {
        Bitboard dest = 0ULL;

        for (int offset : kingOffsets) {
            int to64  = mailbox[mailbox64[i] + offset];
            if (to64 != -1)
                dest |= 1ULL << to64;
        }
        lookupTables.kingDestinations[i] = dest;
    }
}


void MagicSearch::PopulateKnightMoves(LookupTables& lookupTables) {
    const int kingOffsets[8] = { -12, 12, -8, 8, -19, 19, -21, 21 };

    for (int i = 0; i < 64; i++) {
        Bitboard dest = 0ULL;

        for (int offset : kingOffsets) {
            int to64  = mailbox[mailbox64[i] + offset];
            if (to64 != -1)
                dest |= 1ULL << to64;
        }
        lookupTables.knightDestinations[i] = dest;
    }
}

LookupTables MagicSearch::GenerateLookupTables() {
    LookupTables lookupTables{};

    PopulateKingMoves(lookupTables);
    PopulateKnightMoves(lookupTables);

    return lookupTables;
}



int MagicSearch::doit() {
    GenerateRookBitboardSet(true);
    GenerateBishopBitboardSet(true);

    return 0;
}