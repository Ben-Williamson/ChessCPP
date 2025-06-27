import asyncio
import chess
import chess.engine

from tqdm import tqdm

# import logging
# # logging.basicConfig(level=logging.DEBUG)

async def main() -> None:
    _, stockfish = await chess.engine.popen_uci(r"executables/stockfish/stockfish-windows-x86-64-avx2.exe")
    _, botjamin = await chess.engine.popen_uci(r"../src/out/build/x64-Release/Chess.exe")

    await stockfish.configure({
        "UCI_LimitStrength": True,
        "UCI_Elo": 1320
    })

    bar = tqdm(range(10), ncols=150)
    whiteWins = 0
    blackWins = 0

    for i in bar:
        try:
            moveCount = 0
            board = chess.Board(fen="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - - 0 1")
            while not board.is_game_over():
                if board.turn == chess.WHITE:
                    result = await botjamin.play(board, chess.engine.Limit(time=1))
                else:
                    result = await stockfish.play(board, chess.engine.Limit(time=0.1))

                if board.is_legal(result.move):
                    board.push(result.move)
                    # print(board)
                else:
                    print(f"Illegal move attempted: {result.move}")
                    break
                moveCount += 1
                bar.set_description(f"White: {whiteWins} Black: {blackWins} Moves: {moveCount}")

            # print("FINAL BOARD:")
            # print(board)
            # print(board.result())

            # print(board.result())
            if board.result() == '1-0':
                whiteWins += 1
            elif board.result() == '0-1':
                blackWins += 1
            else:
                whiteWins += 0.5
                blackWins += 0.5
        except:
            print("\nNOTE: Failed game detected.")
            pass

    await stockfish.quit()
    await botjamin.quit()

    bar.set_description(f"White: {whiteWins} Black: {blackWins}")


asyncio.run(main())