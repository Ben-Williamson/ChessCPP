//
// Created by ben on 10/06/25.
//

#include <chrono>

#include "../Board.h"
#include "../PerformantBoard.h"
#include "../magics/MagicSearch.h"

int main() {
    MagicSearch magic_search;
    AllMagicBitboards magicBitboards;
    LookupTables lookupTables;

    magicBitboards.rookMagicBitboard = magic_search.GenerateRookBitboardSet(true);
    magicBitboards.bishopMagicBitboard = magic_search.GenerateBishopBitboardSet(true);

    magic_search.GenerateLookupTables();

    lookupTables = magic_search.GenerateLookupTables();

    Board B;
    PerformantBoard PB;

    B.SetMagicBitboards(&magicBitboards);
    PB.SetMagicBitboards(&magicBitboards);
    PB.SetLookupTables(&lookupTables);


    // B.SetFEN("1k1r1b1q/2p1p3/Q1p5/2Np1bp1/3P2p1/4P1B1/PPP2PP1/R4RK1 w - - 1 19");
    PB.SetFEN("r7/1bpQ2Rp/1p6/4P2P/p7/Pn1p1k2/KPq2r2/4R3 w - - 6 38");

    // std::cout << "BOARD:" <<std::endl;
    // std::cout << B << std::endl;

    std::cout << "PERFORMANT BOARD:" <<std::endl;
    std::cout << PB << std::endl;


    MoveList moves;
    // auto t1 = std::chrono::high_resolution_clock::now();
    // std::cout << "B found: " << B.GetAllMoves(moves) << std::endl;
    // auto t2 = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
    // std::cout << "in: " << duration << std::endl;
    //
    // moves.clear();
    // t1 = std::chrono::high_resolution_clock::now();
    // std::cout << "PB found: " << PB.GetAllMoves(moves) << std::endl;
    // t2 = std::chrono::high_resolution_clock::now();
    // duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
    // std::cout << "in: " << duration << std::endl;
    // moves.clear();
    //
    // std::cout << B.KingInCheck() << std::endl;
    // std::cout << PB.KingInCheck() << std::endl;

    // for (int i = 0; i < PB.GetAllMoves(moves); i++) {
    //     std::cout << i << ": " << moves.list[i] << std::endl;
    // }


    // PB.MakeMove(Move{22, 50});

    std::cout << PB << std::endl;

    std::cout << PB.KingInCheck() << std::endl;

    std::cout << PB.GetAllMoves(moves) << std::endl;

    for (int i = 0; i < moves.size; i++) {
        std::cout << moves.list[i] << std::endl;
    }


    return 0;

}
