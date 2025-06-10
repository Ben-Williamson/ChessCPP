//
// Created by ben on 10/06/25.
//

#include "../Board.h"
#include "../PerformantBoard.h"
#include "../magics/MagicSearch.h"

int main() {
    MagicSearch magic_search;
    AllMagicBitboards magicBitboards;

    magicBitboards.rookMagicBitboard = magic_search.GenerateRookBitboardSet();
    magicBitboards.bishopMagicBitboard = magic_search.GenerateBishopBitboardSet();

    magic_search.GenerateLookupTables();

    Board B;
    PerformantBoard PB;

    B.SetMagicBitboards(&magicBitboards);
    PB.SetMagicBitboards(&magicBitboards);


    B.SetFEN("3k1b1r/pQ3ppp/1pR1pn2/3p1P2/5P2/8/4B2P/4K1Nq w - - 4 29");
    PB.SetFEN("3k1b1r/pQ3ppp/1pR1pn2/3p1P2/5P2/8/4B2P/4K1Nq w - - 4 29");

    std::cout << "BOARD:" <<std::endl;
    std::cout << B << std::endl;

    std::cout << "PERFORMANT BOARD:" <<std::endl;
    std::cout << PB << std::endl;

    MoveList PB_moves;

    for (int i = 0; i < PB.GetAllMoves(PB_moves); i++) {
        std::cout <<PB_moves.list[i] << std::endl;
    }
}
