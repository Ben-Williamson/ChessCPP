#include <iostream>
#include <unistd.h>

#include "Board.h"
#include "Search.h"
#include "TreeDebug.h"

class cli_wrapper {
private:
    Board board;
    Searcher searcher;
    bool showBoard = false;
    bool showMoves = false;
    bool mateIn = false;

public:
    cli_wrapper();
    int parse_args(int argc, char **argv);
    int process_arg(std::string, std::string);

    int doit();
};

cli_wrapper::cli_wrapper() {
}


int cli_wrapper::process_arg(std::string arg, std::string value) {
    if (arg == "--fen") {
        board.SetFEN(value);
    }
    else if (arg == "--mateIn") {
        mateIn = true;
    }
    else if (arg == "--board") {
        showBoard = true;
    }
    else if (arg == "--moves") {
        showMoves = true;
    }
    return 1;
}

int cli_wrapper::parse_args(int argc, char* argv[]) {
    std::string current_arg = "";
    std::string current_arg_value = "";

    for (int i = 0; i < argc; i++) {
        if (argv[i][0] == '-' && argv[i][1] == '-') {
            if (current_arg != "") {
                if (!process_arg(current_arg, current_arg_value)) {
                    return 1;
                }
            }
            current_arg = argv[i];
            current_arg_value = "";
            continue;
        }
        current_arg_value += argv[i];
        current_arg_value += ' ';
    }
    if (!process_arg(current_arg, current_arg_value)) {
        return 1;
    }

    return 0;
}

int cli_wrapper::doit() {
    std::vector<MoveEval> PV;
    searcher.FindBestMove(board, PV);

    if (mateIn) {
        int mate_in = 0;
        bool thisPlayer = true;
        for (MoveEval ME : PV) {
            mate_in += thisPlayer;
            thisPlayer = !thisPlayer;
        }
        std::cout << mate_in << std::endl;
    }

    if (showBoard) {
        std::cout << board << std::endl;
    }

    if (showMoves) {
        for (MoveEval& ME : PV) {
            std::cout << ME.move << std::endl;
        }
    }

    return 0;
}

int main(int argc, char* argv[]) {
    cli_wrapper cli;
    if (cli.parse_args(argc, argv)) {
        return 1;
    }

    return cli.doit();
}


// int main() {
//     Board board;
//     Searcher searcher(5);
//
//     board.SetFEN("r4b1r/1bNk1ppp/1Bqpp3/8/4nP2/PP6/2PnB1PP/R2Q1RK1 w - - 5 19");
//     // board.SetFEN("5Q2/8/8/8/k1K5/8/8/8 b - - 0 1");
//
//     std::cout << board << std::endl;
//
//     // std::vector<Move> moves;
//     // std::cout << board.GetAllMoves(moves) << std::endl;
// //    for (Move m: moves) std::cout << m << std::endl;
//
//     // std::cout << searcher.FindBestMove(TODO, bestMove, TODO) << std::endl;
//     // std::cout << bestMove << std::endl;
//
//     std::vector<MoveEval> PV;
//     searcher.FindBestMove(board, PV);
//
//     for (const MoveEval m : PV) std::cout << m.move << " " << m.eval << std::endl;
//
//     return 0;
// }
