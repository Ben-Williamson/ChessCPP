#include "Search.h"
#include <limits>
#include <vector>

#include "MoveList.h"
#include "PerformantBoard.h"

constexpr float MATE_SCORE = 1'000'000.0f;
constexpr int   DEFAULT_DEPTH = 6;


// -----------------------------------------------------------------------------
// Searcher
// -----------------------------------------------------------------------------

Searcher::Searcher()
    : maxDepth_(DEFAULT_DEPTH) {}

Searcher::Searcher(int maxDepth)
    : maxDepth_(maxDepth <= 0 ? DEFAULT_DEPTH : maxDepth) {}

float Searcher::FindBestMove(PerformantBoard& root,
                             std::vector<MoveEval>& principalVariation)
{
    ZoneScoped;
    principalVariation.clear();
    const float score = AlphaBeta(root, maxDepth_, -INF, INF, principalVariation);

    return score;
}

float Searcher::AlphaBeta(PerformantBoard&            b,
                          int               depth,
                          float             alpha,
                          float             beta,
                          std::vector<MoveEval>& line)
{

    #ifdef DEBUG
    node_counter++;
    #endif

    // ZoneScoped;

    // std::vector<Move> moves;

    MoveList moves;

    const int moveCount = b.GetAllMoves(moves);

    if (depth == 0 || moveCount == 0)
    {
        if (moveCount == 0 && b.KingInCheck())
        {
            // Checkmate: bad for the side to move. Use depth to prefer quicker mates.
            return -MATE_SCORE + (maxDepth_ - depth);
        }
        // Stalemate or depth limit: static evaluation.
        return b.PieceValues();
    }

    float bestScore = -INF;
    std::vector<MoveEval> bestLine;

    for (int i = 0; i < moves.size; i++)
    {
        Move mv = moves.list[i];
        b.MakeMove(mv);

        std::vector<MoveEval> childLine;
        const float score = -AlphaBeta(b, depth - 1, -beta, -alpha, childLine);

        b.UndoMove();

        if (score > bestScore)
        {
            bestScore = score;
            bestLine  = childLine;
            bestLine.insert(bestLine.begin(), MoveEval{mv, false, score}); // prepend current move
        }

        alpha = std::max(alpha, bestScore);
        if (alpha >= beta)
            break;
    }

    line = std::move(bestLine);
    return bestScore;
}
















float Searcher::FindBestMove(Board& root,
                             std::vector<MoveEval>& principalVariation)
{
    ZoneScoped;
    principalVariation.clear();
    const float score = AlphaBeta(root, maxDepth_, -INF, INF, principalVariation);

    return score;
}

float Searcher::AlphaBeta(Board&            b,
                          int               depth,
                          float             alpha,
                          float             beta,
                          std::vector<MoveEval>& line)
{
    // ZoneScoped;

    // std::vector<Move> moves;

    MoveList moves;

    const int moveCount = b.GetAllMoves(moves);

    if (depth == 0 || moveCount == 0)
    {
        if (moveCount == 0 && b.KingInCheck())
        {
            // Checkmate: bad for the side to move. Use depth to prefer quicker mates.
            return -MATE_SCORE + (maxDepth_ - depth);
        }
        // Stalemate or depth limit: static evaluation.
        return b.PieceValues();
    }

    float bestScore = -INF;
    std::vector<MoveEval> bestLine;

    for (int i = 0; i < moves.size; i++)
    {
        Move mv = moves.list[i];
        Board next = b.MakeMove(mv);

        std::vector<MoveEval> childLine;
        const float score = -AlphaBeta(next, depth - 1, -beta, -alpha, childLine);

        if (score > bestScore)
        {
            bestScore = score;
            bestLine  = childLine;
            bestLine.insert(bestLine.begin(), MoveEval{mv, false, score}); // prepend current move
        }

        alpha = std::max(alpha, bestScore);
        if (alpha >= beta)
            break;
    }

    line = std::move(bestLine);
    return bestScore;
}
