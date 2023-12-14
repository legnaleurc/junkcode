from argparse import ArgumentParser
from dataclasses import dataclass
from pathlib import Path, PurePath


@dataclass(frozen=True, kw_only=True)
class KeywordArgument:
    config_path: Path
    path_list: list[PurePath]


def parse_args(args: list[str]) -> KeywordArgument:
    parser = ArgumentParser("fix-hah")

    parser.add_argument("--config", "-c", required=True, type=str)
    parser.add_argument("path", nargs="+", type=str)

    kwargs = parser.parse_args(args)

    return KeywordArgument(
        config_path=Path(kwargs.config).expanduser().resolve(),
        path_list=[Path(_) for _ in kwargs.path],
    )
