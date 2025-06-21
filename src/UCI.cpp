
#include <iostream>
#include <string>
#include "UCI.h"
#include <map>

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

	while (true) {
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
	if (command == "readyok") {
		std::cout << "isready" << std::endl;
		return true;
	}
	if (command == "uci") {
		std::cout << "id name Botjamin" << std::endl;
		std::cout << "id author Ben Williamson" << std::endl;
		std::cout << "uciok" << std::endl;
		return true;
	}
	if (command == "ucinewgame") {
		board.SetFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
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