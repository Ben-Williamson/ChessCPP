#include "PerformantBoard.h"
#include "Search.h"

#include <queue>
#include <string>
#include <mutex>

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

    std::mutex commandQueueMux;
    std::queue<std::string> commandQueue;
    std::condition_variable queueCondition;

    void PushToCommandQueue(std::string);

    void CommandListener();
    void CommandProcessor();
    bool ProcessCommand(std::string command);

public:
    UCI_Wrapper();
    bool getReady();
    
    void ListenForCommands();

    void ProcessCommands();
};

#endif