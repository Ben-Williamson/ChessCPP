#include "Search.h"
#include <limits>
#include <vector>

#include "MoveList.h"
#include "PerformantBoard.h"

constexpr float MATE_SCORE = 900'000.0f;
constexpr int   DEFAULT_DEPTH = 6;


#define USE_TRANSPOSITION

// -----------------------------------------------------------------------------
// Searcher
// -----------------------------------------------------------------------------

Searcher::Searcher()
    : maxDepth_(DEFAULT_DEPTH) {}

Searcher::Searcher(int maxDepth)
    : maxDepth_(maxDepth <= 0 ? DEFAULT_DEPTH : maxDepth) {}

float Searcher::FindBestMove(PerformantBoard& root, std::vector<Move>& PV) {
    for (int d = 1; d < maxDepth_; d++) {
        NegaMax(root, 0, -INF, INF, PV, root.GetWhiteToMove() ? 1 : -1, d);
#ifdef DEBUG
        std::cout << d << " complete." << std::endl;
#endif
    }

#ifdef DEBUG
    transpositionHits = 0;
    nodesVisited = 0;
#endif

    float score = NegaMax(root, 0, -INF, INF, PV, root.GetWhiteToMove() ? 1 : -1, maxDepth_);

#ifdef DEBUG
    std::cout << "Visited nodes:      " << nodesVisited << std::endl;
    std::cout << "Transposition hits: " << transpositionHits << std::endl;
#endif

    return score;
}

inline float ToTT(float score, int ply) {        // store
    if (score > MATE_SCORE - 1000) return score + ply;
    if (score < -MATE_SCORE + 1000) return score - ply;
    return score;
}

inline float FromTT(float score, int ply) {        // probe
    if (score > MATE_SCORE - 1000) return score - ply;
    if (score < -MATE_SCORE + 1000) return score + ply;
    return score;
}

float Searcher::NegaMax(PerformantBoard& board, int depth, float alpha, float beta, std::vector<Move>& PV, int colour, int maxDepth) {
#ifdef DEBUG
    nodesVisited++;
#endif

#ifdef USE_TRANSPOSITION
    TranspositionEntry foundEntry;
    bool probeSuccess = transpositionTable.Probe(board, foundEntry);
    if (probeSuccess && foundEntry.depth >= (maxDepth - depth)) {

#ifdef DEBUG
        transpositionHits++;
#endif

        float adjustedScore = FromTT(foundEntry.score, depth);
        switch (foundEntry.type) {
        case EXACT:
            return adjustedScore;
        case LOWER:
            alpha = std::max(alpha, adjustedScore);
            break;
        case UPPER:
            beta = std::min(beta, adjustedScore);
            break;
        }
        if (alpha >= beta) {
            return adjustedScore;
        }
    }
#endif

    MoveList moves;
    const int moveCount = board.GetAllMoves(moves);

    if (depth == maxDepth || moveCount == 0) {
        float score;
        if (moveCount == 0) {
            bool inCheck = (colour == 1) ? board.WhiteKingInCheck() : board.BlackKingInCheck();
            if (inCheck) {
                score = -(MATE_SCORE - depth);
            }
            else {
                score = 0; // Stalemate
            }
        }
        else {
            score = colour * board.PieceValues();
        }
#ifdef USE_TRANSPOSITION
        // Store in transposition table
        TranspositionEntry newEntry;
        newEntry.zobristHash = board.getZobrist();
        newEntry.depth = maxDepth - depth;
        newEntry.score = ToTT(score, depth);
        newEntry.type = EXACT;
        newEntry.bestMove = Move{ -1, -1 };
        transpositionTable.Insert(newEntry);
#endif
        return score;
    }

    float originalAlpha = alpha;
    float bestScore = -INF;
    std::vector<Move> bestChildPV;
    boundType entryType = UPPER;

    for (int i = -1; i < moveCount; i++) {
        std::vector<Move> childPV;
        Move move;

        if (i == -1) {
            if (probeSuccess && foundEntry.bestMove.from != -1) {
                move = foundEntry.bestMove;
            }
            else {
                continue;
            }
        }
        else {
            move = moves.list[i];
        }
    
        board.MakeMove(move);
        float score = -NegaMax(board, depth + 1, -beta, -alpha, childPV, -colour, maxDepth);
        board.UndoMove();

        if (score > bestScore) {
            bestScore = score;
            bestChildPV.clear();
            bestChildPV.push_back(move);
            bestChildPV.insert(bestChildPV.end(), childPV.begin(), childPV.end());
        }

        //if (depth == 0) {
        //    std::cout << move << " score: " << score << std::endl;
        //}

        alpha = std::max(alpha, bestScore);
        if (alpha >= beta) {
            entryType = LOWER;
            break;
        }
    }

    if (bestScore <= originalAlpha)      entryType = UPPER;
    else if (bestScore >= beta)      entryType = LOWER;
    else                              entryType = EXACT;

    PV = bestChildPV;

#ifdef USE_TRANSPOSITION
    // Store in transposition table
    TranspositionEntry newEntry;
    newEntry.zobristHash = board.getZobrist();
    newEntry.depth = maxDepth - depth;
    newEntry.score = bestScore;
    newEntry.type = entryType;
    newEntry.bestMove = PV[0];
    transpositionTable.Insert(newEntry);
#endif

    return bestScore;
}
