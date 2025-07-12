
#include "Zobrist.h"
#include "Random.h"
#include <stdint.h>
#include <iostream>

Zobrist::Zobrist() {
	GenerateBitstrings();
}

bool Zobrist::GenerateBitstrings() {
	seedRandom();
	for (int sq = 0; sq < 64; sq++) {
		for (int piece = 0; piece < 12; piece++) {
			randomBitstrings[sq * 12 + piece] = random_uint64();
		}
	}
	blackToMove = random_uint64();
	return true;
}