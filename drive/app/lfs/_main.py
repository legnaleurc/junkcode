import sys
from argparse import ArgumentParser
from collections.abc import Awaitable, Callable
from pathlib import Path
from typing import Literal

from ._analyzer import analyze
from ._generator import generate


async def main(args: list[str]) -> int:
    action = _parse_args(args)
    try:
        await action()
    except Exception as e:
        print(e, file=sys.stderr)
        return 1
    return 0


def _parse_args(args: list[str]) -> Callable[[], Awaitable[None]]:
    parser = ArgumentParser()
    subparsers = parser.add_subparsers(dest="command", help="Available commands")

    analyze_parser = subparsers.add_parser("analyze", help="Analyze a given path")
    analyze_parser.add_argument("path", help="Path to analyze")

    _generate_parser = subparsers.add_parser(
        "generate", help="Generate script to stdout from stdin"
    )

    kwargs = parser.parse_args(args)
    command: Literal["analyze", "generate"] = kwargs.command
    match command:
        case "analyze":
            path: str = kwargs.path
            return lambda: analyze(Path(path))
        case "generate":
            return lambda: generate()
