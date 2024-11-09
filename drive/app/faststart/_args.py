from argparse import ArgumentParser
from dataclasses import dataclass
from pathlib import Path, PurePath


@dataclass(frozen=True, kw_only=True)
class KeywordArguments:
    data_path: Path
    root_path_list: list[PurePath]
    remux_only: bool
    transcode_only: bool
    cache_only: bool
    jobs: int
    tmp_path: Path | None


def parse_args(args: list[str]) -> KeywordArguments:
    parser = ArgumentParser("app")

    parser.add_argument("--data-path", required=True, type=str)
    parser.add_argument("--tmp-path", type=str)
    parser.add_argument("--jobs", "-j", default=1)

    mutex_group = parser.add_mutually_exclusive_group()
    mutex_group.add_argument("--remux-only", action="store_true", default=False)
    mutex_group.add_argument("--transcode-only", action="store_true", default=False)
    mutex_group.add_argument("--cache-only", action="store_true", default=False)

    parser.add_argument("root_path", type=str, nargs="+")

    kwargs = parser.parse_args(args)

    return KeywordArguments(
        data_path=Path(kwargs.data_path).expanduser().resolve(),
        root_path_list=[PurePath(_) for _ in kwargs.root_path],
        remux_only=kwargs.remux_only,
        transcode_only=kwargs.transcode_only,
        cache_only=kwargs.cache_only,
        jobs=kwargs.jobs,
        tmp_path=None if not kwargs.tmp_path else Path(kwargs.tmp_path),
    )
