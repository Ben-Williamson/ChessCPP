
#ifndef CHESS_ZOBRIST_H
#define CHESS_ZOBRIST_H

#include <cstdint>

class Zobrist {
public:
	uint64_t randomBitstrings[64 * 12];
	uint64_t blackToMove;

	Zobrist();
	bool GenerateBitstrings();
};

#endif