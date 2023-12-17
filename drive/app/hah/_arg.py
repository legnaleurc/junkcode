from argparse import ArgumentParser
from dataclasses import dataclass
from pathlib import Path, PurePath


@dataclass(frozen=True, kw_only=True)
class KeywordArgument:
    drive_path: Path
    path_list: list[PurePath]


def parse_args(args: list[str]) -> KeywordArgument:
    parser = ArgumentParser("hah")

    parser.add_argument("--drive-path", required=True, type=str)
    parser.add_argument("path", nargs="+", type=str)

    kwargs = parser.parse_args(args)

    return KeywordArgument(
        drive_path=Path(kwargs.drive_path).expanduser().resolve(),
        path_list=[Path(_) for _ in kwargs.path],
    )
