#include "PerformantBoard.h"
#include "Search.h"

#ifndef CHESS_UCI_H
#define CHESS_UCI_H

class UCI_Wrapper {
	bool isready;

    PerformantBoard board;
    Searcher searcher;
    bool showBoard = false;
    bool showMoves = false;
    bool mateIn = false;
    bool regenerateMagics = false;
    bool showBestMove = false;

    AllMagicBitboards magicBitboards;
    LookupTables lookupTables;

public:
    UCI_Wrapper();
    bool getReady();

};

#endif