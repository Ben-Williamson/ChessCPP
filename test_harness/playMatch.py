import asyncio
import datetime
import os
import traceback
from pathlib import Path
from typing import Tuple

import chess
import chess.engine
import chess.pgn
from tqdm import tqdm

# --------------------------------------------------------------
# Per‑game coroutine --------------------------------------------------------------
# --------------------------------------------------------------
async def _play_single_game(
    game_idx: int,
    save_dir: Path,
    stockfish_path: str,
    botjamin_path: str,
) -> Tuple[str, bool]:
    """Play **one** Botjamin vs Stockfish game and persist the PGN.

    Returns
    -------
    result_str : str
        The game result in PGN notation ("1-0", "0-1", "1/2-1/2", or "*").
    failed : bool
        *True* if the game raised an exception that aborted play.
    """

    # ----------------------------------------------------------
    # Engine startup (each game gets its *own* engine pair)
    # ----------------------------------------------------------
    _, stockfish = await chess.engine.popen_uci(stockfish_path)
    _, botjamin = await chess.engine.popen_uci(botjamin_path)

    await stockfish.configure({"UCI_LimitStrength": True, "UCI_Elo": 1700})

    board = chess.Board(
        fen="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - - 0 1",
    )
    move_count = 0
    error: Exception | None = None

    try:
        # ------------------------------------------------------
        # Main game loop
        # ------------------------------------------------------
        while not board.is_game_over():
            if board.turn == chess.WHITE:
                result = await botjamin.play(board, chess.engine.Limit(time=1))
            else:
                result = await stockfish.play(board, chess.engine.Limit(time=0.1))

            if not board.is_legal(result.move):
                raise ValueError(f"Illegal move attempted: {result.move}")

            board.push(result.move)
            move_count += 1
    except Exception as exc:  # noqa: BLE001
        error = exc

    # ----------------------------------------------------------
    # Accounting + PGN export (always saved)
    # ----------------------------------------------------------
    result_str = board.result(claim_draw=True) if board.is_game_over() else "*"

    game = chess.pgn.Game.from_board(board)
    game.headers.update(
        {
            "Event": "Botjamin vs Stockfish",
            "Site": "Local",
            "Date": datetime.datetime.now().strftime("%Y.%m.%d"),
            "White": "Botjamin",
            "Black": "Stockfish",
            "Result": result_str,
        }
    )

    if error is not None:
        # Add error information to the PGN
        game.headers["Error"] = repr(error)
        game.comment = (
            "Traceback (most recent call last):\n" + "".join(traceback.format_exception(error))
        )

    suffix = "_failed" if error else ""
    pgn_path = save_dir / f"game_{game_idx + 1}{suffix}.pgn"
    with pgn_path.open("w", encoding="utf-8") as pgn_file:
        print(game, file=pgn_file, end="\n\n")

    # ----------------------------------------------------------
    # Cleanup
    # ----------------------------------------------------------
    await stockfish.quit()
    await botjamin.quit()

    return result_str, error is not None


# --------------------------------------------------------------
# Orchestrator --------------------------------------------------
# --------------------------------------------------------------
async def run_match(num_games: int = 5, max_concurrent: int | None = None) -> None:
    """Run *num_games* games **in parallel** and print a summary.

    Parameters
    ----------
    num_games : int, default=10
        Number of games to play.
    max_concurrent : int | None, optional
        Limit the number of games that run simultaneously.  If *None*, this
        defaults to ``min(os.cpu_count(), num_games)``.
    """

    # ------------------------------------------------------------------
    # Concurrency control
    # ------------------------------------------------------------------
    if max_concurrent is None:
        max_concurrent = min(os.cpu_count() or 4, num_games)

    semaphore = asyncio.Semaphore(max_concurrent)

    # ------------------------------------------------------------------
    # Timestamped output directory
    # ------------------------------------------------------------------
    ts = datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
    save_dir = Path("pgn_logs") / ts
    save_dir.mkdir(parents=True, exist_ok=True)

    stockfish_path = r"executables/stockfish/stockfish-windows-x86-64-avx2.exe"
    botjamin_path = r"../src/out/build/x64-Release/Chess.exe"

    # ------------------------------------------------------------------
    # Kick off all game tasks
    # ------------------------------------------------------------------
    async def _wrapper(idx: int):
        """Semaphore‑guarded single‑game wrapper."""
        async with semaphore:
            return await _play_single_game(idx, save_dir, stockfish_path, botjamin_path)

    tasks = [asyncio.create_task(_wrapper(i)) for i in range(num_games)]

    # ------------------------------------------------------------------
    # Aggregate results as games finish
    # ------------------------------------------------------------------
    white_wins = black_wins = failed_games = 0
    bar = tqdm(total=num_games, ncols=100, desc="Running games")

    for coro in asyncio.as_completed(tasks):
        result_str, failed = await coro

        if result_str == "1-0":
            white_wins += 1
        elif result_str == "0-1":
            black_wins += 1
        elif result_str == "1/2-1/2":
            white_wins += 0.5
            black_wins += 0.5

        if failed:
            failed_games += 1

        bar.set_description(
            f"White:{white_wins} Black:{black_wins} Failed:{failed_games}"
        )
        bar.update(1)

    bar.close()

    print(f"White: {white_wins}  Black: {black_wins}  Failed: {failed_games}")


# ----------------------------------------------------------------------
# CLI entry‑point ------------------------------------------------------
# ----------------------------------------------------------------------
if __name__ == "__main__":
    asyncio.run(run_match())


# import asyncio
# import datetime
# import traceback
# from pathlib import Path

# import chess
# import chess.engine
# import chess.pgn
# from tqdm import tqdm


# async def run_match(num_games: int = 10) -> None:
#     """Run *num_games* Botjamin vs Stockfish games and log every PGN — even for failed runs.

#     Games that raise an exception are **saved with a `_failed` suffix** and contain
#     an `Error` header as well as the full traceback in a root comment.  This makes
#     it easy to reproduce and debug engine problems later.
#     """

#     # ------------------------------------------------------------------
#     # Engine setup
#     # ------------------------------------------------------------------
#     _, stockfish = await chess.engine.popen_uci(
#         r"executables/stockfish/stockfish-windows-x86-64-avx2.exe"
#     )
#     _, botjamin = await chess.engine.popen_uci(
#         r"../src/out/build/x64-Release/Chess.exe"
#     )

#     await stockfish.configure({
#         "UCI_LimitStrength": True,
#         "UCI_Elo": 1320,
#     })

#     # ------------------------------------------------------------------
#     # Output directory – timestamped so logs never collide
#     # ------------------------------------------------------------------
#     ts = datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
#     save_dir = Path("pgn_logs") / ts
#     save_dir.mkdir(parents=True, exist_ok=True)

#     # ------------------------------------------------------------------
#     # Match loop
#     # ------------------------------------------------------------------
#     bar = tqdm(range(num_games), ncols=100)
#     white_wins = black_wins = failed_games = 0

#     for game_idx in bar:
#         board = chess.Board(fen="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - - 0 1")
#         move_count = 0
#         error: Exception | None = None

#         try:
#             while not board.is_game_over():
#                 if board.turn == chess.WHITE:
#                     result = await botjamin.play(board, chess.engine.Limit(time=1))
#                 else:
#                     result = await stockfish.play(board, chess.engine.Limit(time=0.1))

#                 # Sanity‑check engine output
#                 if not board.is_legal(result.move):
#                     raise ValueError(f"Illegal move attempted: {result.move}")

#                 board.push(result.move)
#                 move_count += 1

#                 # Live evaluation for tqdm bar
#                 info = await stockfish.analyse(board, chess.engine.Limit(time=0.1))
#                 eval_cp = info["score"].white().score(mate_score=10000)
#                 bar.set_description(
#                     f"White:{white_wins} Black:{black_wins} "
#                     f"Failed:{failed_games} Moves:{move_count} "
#                     f"Eval:{'N/A' if eval_cp is None else eval_cp/100:.2f}"
#                 )

#         except Exception as exc:  # noqa: BLE001
#             error = exc
#             failed_games += 1

#         # --------------------------------------------------------------
#         # Result accounting – only counts finished games toward the score
#         # --------------------------------------------------------------
#         result_str = board.result(claim_draw=True) if board.is_game_over() else "*"
#         if result_str == "1-0":
#             white_wins += 1
#         elif result_str == "0-1":
#             black_wins += 1
#         elif result_str == "1/2-1/2":
#             white_wins += 0.5
#             black_wins += 0.5

#         # --------------------------------------------------------------
#         # Persist PGN no matter what happened
#         # --------------------------------------------------------------
#         game = chess.pgn.Game.from_board(board)
#         game.headers.update(
#             {
#                 "Event": "Botjamin vs Stockfish",
#                 "Site": "Local",
#                 "Date": datetime.datetime.now().strftime("%Y.%m.%d"),
#                 "White": "Botjamin",
#                 "Black": "Stockfish",
#                 "Result": result_str,
#             }
#         )

#         if error:
#             # First line in header, complete traceback in root comment
#             game.headers["Error"] = repr(error)
#             game.comment = (
#                 "Traceback (most recent call last):\n" + "".join(traceback.format_exception(error))
#             )

#         suffix = "_failed" if error else ""
#         pgn_path = save_dir / f"game_{game_idx + 1}{suffix}.pgn"
#         with pgn_path.open("w", encoding="utf-8") as pgn_file:
#             print(game, file=pgn_file, end="\n\n")

#     # ------------------------------------------------------------------
#     # Tidy up
#     # ------------------------------------------------------------------
#     await stockfish.quit()
#     await botjamin.quit()

#     print(f"White: {white_wins} Black: {black_wins} Failed: {failed_games}")


# if __name__ == "__main__":
#     asyncio.run(run_match())

# # import asyncio
# # import chess
# # import chess.engine

# # from tqdm import tqdm

# # from time import sleep

# # # import logging
# # # logging.basicConfig(level=logging.DEBUG)

# # async def main() -> None:
# #     _, stockfish = await chess.engine.popen_uci(r"executables/stockfish/stockfish-windows-x86-64-avx2.exe")
# #     _, botjamin = await chess.engine.popen_uci(r"../src/out/build/x64-Debug/Chess.exe")

# #     await stockfish.configure({
# #         "UCI_LimitStrength": True,
# #         "UCI_Elo": 1320
# #     })

# #     bar = tqdm(range(10), ncols=100)
# #     whiteWins = 0
# #     blackWins = 0
# #     failedGames = 0

# #     for i in bar:
# #         try:
# #             moveCount = 0
# #             board = chess.Board(fen="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - - 0 1")
# #             while not board.is_game_over():
# #                 if board.turn == chess.WHITE:
# #                     result = await botjamin.play(board, chess.engine.Limit(time=1))
# #                 else:
# #                     result = await stockfish.play(board, chess.engine.Limit(time=0.1))

# #                 if board.is_legal(result.move):
# #                     board.push(result.move)
# #                     # print(board)
# #                 else:
# #                     print(f"Illegal move attempted: {result.move}")
# #                     break
# #                 moveCount += 1
# #                 info = await stockfish.analyse(board, chess.engine.Limit(time=0.1))
# #                 bar.set_description(f"White: {whiteWins} Black: {blackWins} Failed: {failedGames} Moves: {moveCount} Eval: {float(info['score'].white().score())/100.0}")

# #             if board.result() == '1-0':
# #                 whiteWins += 1
# #             elif board.result() == '0-1':
# #                 blackWins += 1
# #             else:
# #                 whiteWins += 0.5
# #                 blackWins += 0.5

# #         except Exception as e:
# #             # print("\nNOTE: Failed game detected.")
# #             # raise e
# #             failedGames += 1
# #             pass

# #     await stockfish.quit()
# #     await botjamin.quit()

# #     print(f"White: {whiteWins} Black: {blackWins} Failed: {failedGames}")


# # asyncio.run(main())