
#include "Profiler.h"
#include "TranspositionTable.h"
#include "PerformantBoard.h"

#include <cstdlib>
#include "CrossPlatform.h"

TranspositionTable::TranspositionTable() : table(nullptr) {
	uint64_t maxBytes = tableSizeMB * 1024ULL * 1024ULL;
	uint64_t capacity = maxBytes / sizeof(TranspositionEntry);

	// This could be achieved with some bit-twiddly stuff
	// but this is only a constructor.
	tableCapacity = std::pow(2, std::floor(std::log2(capacity)));

	#ifdef DEBUG
	std::cout << "TranspositionTable::TranspositionTable()" << std::endl;
	std::cout << "Table size limit (Bytes): " << maxBytes << std::endl;
	std::cout << "Maximum possible capacity: " << capacity << std::endl;
	std::cout << "Using table size of: " << tableCapacity << std::endl;
	#endif

	table = static_cast<TranspositionEntry*>(
		portable::aligned_alloc(128, tableCapacity * sizeof(TranspositionEntry))
		);
	std::memset(table, 0, tableCapacity * sizeof(TranspositionEntry));
	static_assert(alignof(TranspositionEntry) <= 128);
}

TranspositionTable::~TranspositionTable() {
	portable::aligned_free(table);
}

bool TranspositionTable::Probe(PerformantBoard& board, TranspositionEntry& entry) {
	entry = table[board.getZobrist() & (tableCapacity - 1)];
	return entry.zobristHash == board.getZobrist();
}

void TranspositionTable::Insert(TranspositionEntry entry) {
	uint64_t location = entry.zobristHash & (tableCapacity - 1);

	if (table[location].zobristHash) {
		if (table[location].depth > entry.depth) {
			table[location] = entry;
			//std::cout << "inserted: " << entry.zobristHash << std::endl;
		}
	}
	else {
		table[location] = entry;
		//std::cout << "inserted: " << entry.zobristHash << std::endl;
	}
	
}