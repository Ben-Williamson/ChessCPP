
#include "UCI.h"

UCI_Wrapper::UCI_Wrapper() : isready(false), regenerateMagics(false) {
	isready = getReady(); 
}

bool UCI_Wrapper::getReady() {
	MagicSearch magic_search;

	magicBitboards.rookMagicBitboard = magic_search.GenerateRookBitboardSet(regenerateMagics);
	magicBitboards.bishopMagicBitboard = magic_search.GenerateBishopBitboardSet(regenerateMagics);

	lookupTables = magic_search.GenerateLookupTables();

	board.SetMagicBitboards(&magicBitboards);
	board.SetLookupTables(&lookupTables);

	return true;
}

int main() {
	UCI_Wrapper UCI;

	_sleep(10000);

	return 0;
}