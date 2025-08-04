#include "../TranspositionTable.h"

int main() {
	TranspositionTable tt;

	MagicSearch magic_search;
	AllMagicBitboards magicBitboards;
	Zobrist zobrist_tables;


	magicBitboards.rookMagicBitboard = magic_search.GenerateRookBitboardSet(false);
	magicBitboards.bishopMagicBitboard = magic_search.GenerateBishopBitboardSet(false);

	LookupTables lookupTables = magic_search.GenerateLookupTables();

	PerformantBoard testBoard1;
	testBoard1.SetMagicBitboards(&magicBitboards);
	testBoard1.SetLookupTables(&lookupTables);

	testBoard1.SetZobristTables(&zobrist_tables);
	testBoard1.SetFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

	
	std::cout << testBoard1 << std::endl;
	testBoard1.MakeMove(Move{ 11, 27 });
	testBoard1.MakeMove(Move{ 57, 42 });
	testBoard1.MakeMove(Move{ 12, 20 });
	testBoard1.MakeMove(Move{ 51, 35 });
	std::cout << testBoard1 << std::endl;
	TranspositionEntry entry1;
	entry1.zobristHash = testBoard1.getZobrist();
	tt.Insert(entry1);

	// -----------------------------------------------------------

	PerformantBoard testBoard2;
	testBoard2.SetMagicBitboards(&magicBitboards);
	testBoard2.SetLookupTables(&lookupTables);

	testBoard2.SetZobristTables(&zobrist_tables);
	testBoard2.SetFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");


	std::cout << testBoard2 << std::endl;
	testBoard2.MakeMove(Move{ 12, 20 });
	testBoard2.MakeMove(Move{ 51, 35 });
	testBoard2.MakeMove(Move{ 11, 27 });
	testBoard2.MakeMove(Move{ 57, 42 });
	std::cout << testBoard2 << std::endl;

	TranspositionEntry returnedEntry;
	std::cout << tt.Probe(testBoard2, returnedEntry) << std::endl;


	PerformantBoard testBoard3;
	testBoard3.SetMagicBitboards(&magicBitboards);
	testBoard3.SetLookupTables(&lookupTables);

	testBoard3.SetZobristTables(&zobrist_tables);
	testBoard3.SetFEN("r1bqkbnr/ppp1pppp/2n5/3p4/3P4/4P3/PPP2PPP/RNBQKBNR w KQkq - 0 1");
	std::cout << testBoard3 << std::endl;
}