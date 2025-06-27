
#include <iostream>
#include <string>
#include "UCI.h"
#include <map>

UCI_Wrapper::UCI_Wrapper() : isready(false), regenerateMagics(false), running(true) {
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

void UCI_Wrapper::PushToCommandQueue(std::string command) {
	std::lock_guard<std::mutex> lock(commandQueueMux);

	commandQueue.push(command);
	queueCondition.notify_one();
}

void UCI_Wrapper::CommandListener() {
	for (std::string command; std::getline(std::cin, command);) {
		PushToCommandQueue(command);
	}
}

void UCI_Wrapper::CommandProcessor() {
	std::unique_lock<std::mutex> lock(commandQueueMux);

	while (running) {
		while (commandQueue.empty()) {
			queueCondition.wait(lock);
		}
		// If we get where then there is work to be done in the queue.
		while (!commandQueue.empty()) {
			std::string command = commandQueue.front();
			commandQueue.pop();
			lock.unlock();

			UCI_Wrapper::ProcessCommand(command);

			lock.lock();
		}
	}
}

void UCI_Wrapper::ListenForCommands() {
	std::thread listenerThread(&UCI_Wrapper::CommandListener, this);
	listenerThread.detach();
}

void UCI_Wrapper::ProcessCommands() {
	std::thread processorThread(&UCI_Wrapper::CommandProcessor, this);
	processorThread.join();
}

bool UCI_Wrapper::ProcessCommand(std::string command) {
	std::vector<std::string> args;

	int startPointer = 0;
	for (int i = 0; i < command.size(); i++) {
		if (command[i] == ' ') {
			args.push_back(command.substr(startPointer, i-startPointer));
			startPointer = i+1;
			i++;
		}
	}
	args.push_back(command.substr(startPointer, command.size() - startPointer));


	if (args[0] == "readyok") {
		std::cout << "isready" << std::endl;
		return true;
	}
	if (args[0] == "uci") {
		std::cout << "id name Botjamin" << std::endl;
		std::cout << "id author Ben Williamson" << std::endl;
		std::cout << "uciok" << std::endl;
		return true;
	}
	if (args[0] == "ucinewgame") {
		board.SetFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
		return true;
	}
	if (args[0] == "isready") {
		std::cout << "readyok" << std::endl;
		return true;
	}
	if (args[0] == "position") {
		if (args.size() > 0 && args[1] == "startpos") {
			board.SetFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
		}
		if (args.size() > 1 && args[2] == "moves") {
			for (int i = 3; i < args.size(); i++) {
				board.MakeMove(Move::fromUci(args[i]));
				std::cout << "info string played:" << args[i] << std::endl;
				std::cout << "info string promotion found:" << Move::fromUci(args[i]).promotionPiece << std::endl;
				if (Move::fromUci(args[i]).promotionPiece) {
					std::cout << "info string promotion happened" << std::endl;
				}
			}
		}
		if (args.size() > 0 && args[1] == "fen") {
			std::string fen = "";
			for (int i = 2; i < 8; i++) {
				fen += args[i];
				fen += " ";
			}
			std::cout << "info string set this fen: " << fen << std::endl;
			board.SetFEN(fen);
			if (args.size() > 8 && args[8] == "moves") {
				for (int i = 9; i < args.size(); i++) {
					board.MakeMove(Move::fromUci(args[i]));
					std::cout << "info string played:" << args[i] << std::endl;
					if (Move::fromUci(args[i]).promotionPiece) {
						std::cout << "info string promotion happened" << std::endl;
					}
				}
			}
		}
		return true;
	}
	if (args[0] == "go") {
		if (args.size() > 1 && args[1] == "movetime") {
			// this needs to be implemented
		}
		// Can currently just return the best move,
		// and not think very hard.

		std::vector<MoveEval> PV;
		MoveList moves;
		searcher.FindBestMove(board, PV);

		std::cout << "bestmove " << PV[0].move.CoordsToBoardSquares() << std::endl;

		std::cout << "info string total moves:" << board.GetAllMoves(moves)  << " best:" << PV[0].move << std::endl;
		std::cout << board << std::endl;

		return true;
	}

	if (args[0] == "quit") {
		running = false;
		return true;
	}

	return false;
}

int main() {
	UCI_Wrapper UCI;
	
	UCI.ListenForCommands();
	UCI.ProcessCommands();

	return 0;
}