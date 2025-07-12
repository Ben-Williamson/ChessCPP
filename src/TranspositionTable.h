#include "PerformantBoard.h"

#ifndef CHESS_TRANSPOSITION_H
#define CHESS_TRANSPOSITION_H

struct TranspositionEntry {
	uint64_t zobristHash;
};


class TranspositionTable {
	TranspositionEntry* table;
public:
	TranspositionTable();

	bool Probe(PerformantBoard* board, TranspositionEntry* entry);
};


#endif