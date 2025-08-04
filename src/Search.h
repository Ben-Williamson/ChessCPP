//
// Created by ben on 16/05/25.
//

#ifndef CHESS_SEARCH_H
#define CHESS_SEARCH_H

#include "Board.h"
#include "TreeDebug.h"
#include <limits>

#include "PerformantBoard.h"
#include "Profiler.h"
#include "TranspositionTable.h"

constexpr float INF = std::numeric_limits<float>::infinity();

class Searcher {
    int maxDepth_;
    TranspositionTable transpositionTable;

#ifdef DEBUG
    int transpositionHits;
    int nodesVisited;
#endif

public:
    Searcher();

    Searcher(int maxDepth);

    float FindBestMove(PerformantBoard& root, std::vector<Move>& PV);
    float FixMateScore(float score, int depthFromRoot);
    float NegaMax(PerformantBoard& board, int depth, float alpha, float beta, std::vector<Move>& PV, int colour, int maxDepth);

};

#endif //CHESS_SEARCH_H
