#!/usr/bin/env python3
"""
debug_game.py – Run a single Botjamin vs Stockfish game and dump the full
UCI dialog for both engines to the terminal.

Requirements
------------
python-chess ≥ 1.999
tqdm  (optional – just for the progress bar)

Example
-------
$ python debug_game.py --stockfish executables/stockfish/stockfish-windows-x86-64-avx2.exe \
                       --botjamin ../src/out/build/x64-Release/Chess.exe \
                       --moves 200
"""

from __future__ import annotations

import argparse
import datetime as _dt
import sys
from pathlib import Path
from typing import Final

import chess
import chess.pgn
import chess.engine
from tqdm import tqdm

# import logging

# logging.basicConfig(level=logging.DEBUG)

# ---------------------------------------------------------------------------
# Helper: make a debug stream that tags each line with the engine’s name
# ---------------------------------------------------------------------------
class _PrefixedWriter:
    """File‑like wrapper that prefixes every write with an identifier."""

    def __init__(self, prefix: str, wrapped_stream):
        self._prefix = prefix
        self._stream = wrapped_stream

    def write(self, text: str) -> int:  # noqa: D401
        return self._stream.write(f"{self._prefix}{text}")

    def flush(self):  # noqa: D401
        self._stream.flush()


def _open_engine(path: str, tag: str) -> chess.engine.SimpleEngine:
    """Open a UCI engine with full I/O logging."""
    return chess.engine.SimpleEngine.popen_uci(
        path,
        debug=_PrefixedWriter(f"[{tag}] ", sys.stdout),
    )


# ---------------------------------------------------------------------------
# Core game logic (synchronous)
# ---------------------------------------------------------------------------
def play_game(
    stockfish_path: str,
    botjamin_path: str,
    max_moves: int | None = None,
    save_dir: Path | None = None,
) -> None:
    """Run one game, print full UCI traffic, save PGN afterwards."""

    if save_dir is None:
        ts = _dt.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
        save_dir = Path("pgn_logs") / ts
    save_dir.mkdir(parents=True, exist_ok=True)

    # One engine instance each
    stockfish = _open_engine(stockfish_path, "SF ")
    botjamin = _open_engine(botjamin_path, "BJ ")

    try:
        # Tell Stockfish to play at ~1700 Elo so it’s beatable
        stockfish.configure({"UCI_LimitStrength": True, "UCI_Elo": 1700})

        board = chess.Board(
            fen="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - - 0 1"
        )
        bar = tqdm(total=max_moves or 9999, ncols=80, desc="Moves played")

        while not board.is_game_over(claim_draw=True):
            if max_moves is not None and board.fullmove_number > max_moves:
                print("\nMaximum move count reached – terminating.\n")
                break

            if board.turn == chess.WHITE:
                result = botjamin.play(board, chess.engine.Limit(time=1.0))
            else:
                result = stockfish.play(board, chess.engine.Limit(time=0.1))

            if not board.is_legal(result.move):
                raise ValueError(f"Illegal move attempted: {result.move}")

            board.push(result.move)
            bar.update(1)

        bar.close()

    finally:
        # Always quit engines or they will linger as zombie processes
        stockfish.quit()
        botjamin.quit()

    # -----------------------------------------------------------------------
    # PGN export
    # -----------------------------------------------------------------------
    game = chess.pgn.Game.from_board(board)
    game.headers.update(
        {
            "Event": "Botjamin vs Stockfish (debug)",
            "Site": "Local",
            "Date": _dt.datetime.now().strftime("%Y.%m.%d"),
            "White": "Botjamin",
            "Black": "Stockfish",
            "Result": board.result(claim_draw=True),
        }
    )

    pgn_path: Final = save_dir / "debug_game.pgn"
    with pgn_path.open("w", encoding="utf-8") as fh:
        print(game, file=fh, end="\n\n")

    print(f"\nPGN written to: {pgn_path.resolve()}")
    print(f"Final result  : {board.result(claim_draw=True)}")


# ---------------------------------------------------------------------------
# CLI entry point
# ---------------------------------------------------------------------------
def _parse_args() -> argparse.Namespace:
    ap = argparse.ArgumentParser(description="Play a single debug game.")
    ap.add_argument("--stockfish", required=True, help="Path to Stockfish binary")
    ap.add_argument("--botjamin", required=True, help="Path to Botjamin binary")
    ap.add_argument(
        "--moves",
        type=int,
        default=None,
        help="Optional hard cap on number of half‑moves to play",
    )
    ap.add_argument(
        "--out",
        type=Path,
        default=None,
        help="Directory to save the PGN (defaults to ./pgn_logs/<timestamp>/)",
    )
    return ap.parse_args()


if __name__ == "__main__":
    args = _parse_args()
    play_game(
        stockfish_path=args.stockfish,
        botjamin_path=args.botjamin,
        max_moves=args.moves,
        save_dir=args.out,
    )
