//
// Created by ben on 16/05/25.
//

#ifndef CHESS_SEARCH_H
#define CHESS_SEARCH_H

#include "Board.h"
#include "TreeDebug.h"
#include <limits>
#include "Profiler.h"

constexpr float INF = std::numeric_limits<float>::infinity();

class Searcher {
    int maxDepth_;

public:
    Searcher();

    Searcher(int maxDepth);

    float FindBestMove(Board &root, std::vector<MoveEval> &principalVariation);

    float AlphaBeta(Board &b, int depth, float alpha, float beta, std::vector<MoveEval> &line);

    float Mini(Board b, int depth, float alpha, float beta, Move *bestMove, TreeDebug *tree);

    float Maxi(Board b, int depth, float alpha, float beta, Move *bestMove, TreeDebug *tree);

    float Mini(Board b, int depth, float alpha, float beta, Move *bestMove, TreeDebug *tree, int parentId);

    float Maxi(Board b, int depth, float alpha, float beta, Move *bestMove, TreeDebug *tree, int parentId);
};

#endif //CHESS_SEARCH_H
