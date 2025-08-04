#ifndef CHESS_TRANSPOSITION_H
#define CHESS_TRANSPOSITION_H

#include "PerformantBoard.h"

inline int tableSizeMB = 256*2;

enum boundType { EXACT, LOWER, UPPER };

struct alignas(64) TranspositionEntry {
	uint64_t zobristHash;
	int depth;
	int score;
	boundType type;
	Move bestMove;
};


class TranspositionTable {
	uint64_t tableCapacity;
	TranspositionEntry* table;
public:
	TranspositionTable();
	~TranspositionTable();

	bool Probe(PerformantBoard& board, TranspositionEntry& entry);

	void Insert(TranspositionEntry entry);

	//inline bool Probe(PerformantBoard& board, TranspositionEntry& entry) noexcept;
};


#endif