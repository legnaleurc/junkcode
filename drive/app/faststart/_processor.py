import asyncio
import json
import shutil
import subprocess
from concurrent.futures import Executor
from contextlib import asynccontextmanager, contextmanager
from logging import getLogger
from os.path import splitext
from pathlib import Path

from wcpan.drive.core.lib import upload_file_from_local, download_file_to_local
from wcpan.drive.core.types import Node, MediaInfo, Drive
from wcpan.drive.cli.lib import get_file_hash, get_video_info

from app.lib import get_daily_usage

from ._cache import is_migrated, set_cache, need_transcode, has_cache, unset_cache


VIDEO_CODEC_SET = {"h264", "hevc"}
H264_PRESET = "veryslow"
H264_CRF = "18"
MP4_FLAGS = "+faststart"

DAILY_UPLOAD_QUOTA = 500 * 1024 * 1024 * 1024


_L = getLogger(__name__)


class VideoProcessor(object):
    def __init__(
        self, *, work_folder: Path, dsn: Path, pool: Executor, drive: Drive, node: Node
    ):
        self.work_folder = work_folder
        self.dsn = dsn
        self.pool = pool
        self.drive = drive
        self.node = node
        # implies mp4
        self.is_faststart = False
        self.is_h264 = False
        self.is_aac = False
        self.is_movtext = False

    async def prepare_codec_info(self) -> None:
        raise NotImplementedError()

    @property
    def transcoded_file_name(self) -> str:
        raise NotImplementedError()

    @property
    def codec_command(self) -> list[str]:
        # codec options
        if self.is_h264:
            vc = ["-c:v", "copy"]
        else:
            vc = ["-c:v", "libx264", "-crf", H264_CRF, "-preset", H264_PRESET]
        if self.is_aac:
            ac = ["-c:a", "copy"]
        else:
            ac = []
        if self.is_movtext:
            sc = ["-c:s", "copy"]
        else:
            sc = ["-c:s", "mov_text"]
        dc = ["-c:d", "copy"]
        # muxer options
        fast_start = ["-movflags", MP4_FLAGS]
        # keeps subtitles if possible
        all_streams = ["-map", "0"]
        # increase frame queue to fix corrupted frames
        frame_queue = ["-max_muxing_queue_size", "1024"]
        return fast_start + ac + vc + sc + dc + all_streams + frame_queue

    @property
    def output_folder(self) -> Path:
        folder = self.work_folder / self.node.id
        return folder

    @property
    def raw_file_path(self) -> Path:
        return self.output_folder / f"__{self.node.name}"

    @property
    def transcoded_file_path(self) -> Path:
        return self.output_folder / self.transcoded_file_name

    async def update_codec_from_ffmpeg(self):
        cmd = [
            "ffprobe",
            "-v",
            "trace",
            "-show_streams",
            "-print_format",
            "json",
            "-i",
            str(self.raw_file_path),
        ]
        out, err = await shell_pipe(cmd)
        data = json.loads(out)
        data = data["streams"]
        audio_codec_list: list[str] = []
        video_codec_list: list[str] = []
        subtitle_codec_list: list[str] = []
        for stream in data:
            codec_type = stream["codec_type"]
            if codec_type == "data":
                continue
            codec_name = stream["codec_name"]
            if codec_type == "audio":
                audio_codec_list.append(codec_name)
            elif codec_type == "video":
                attached_pic: int = stream["disposition"]["attached_pic"]
                if attached_pic == 0:
                    video_codec_list.append(codec_name)
            elif codec_type == "subtitle":
                subtitle_codec_list.append(codec_name)
        self.is_aac = all(c == "aac" for c in audio_codec_list)
        self.is_h264 = all(c in VIDEO_CODEC_SET for c in video_codec_list)
        self.is_movtext = all(c == "mov_text" for c in subtitle_codec_list)
        self.is_faststart = err.find("bytes read, 0 seeks") >= 0

    @property
    def is_skippable(self) -> bool:
        return self.is_faststart and self.is_native_codec

    @property
    def is_native_codec(self) -> bool:
        return self.is_h264 and self.is_aac

    async def __call__(
        self,
        *,
        remux_only: bool,
        transcode_only: bool,
        cache_only: bool,
    ):
        if is_migrated(self.dsn, self.node):
            _L.info("(cache) already migrated, skip")
            return False

        if (
            transcode_only
            and has_cache(self.dsn, self.node)
            and not need_transcode(self.dsn, self.node)
        ):
            _L.info("no need transcode, skip")
            return False

        if (
            remux_only
            and has_cache(self.dsn, self.node)
            and need_transcode(self.dsn, self.node)
        ):
            _L.info("need transcode, skip")
            return False

        if cache_only and has_cache(self.dsn, self.node):
            _L.info("already cached, skip")
            return False

        if not cache_only and not has_enough_quota(self.drive, self.node.size):
            _L.info("not enough quota, skip")
            return False

        with self._local_context():
            try:
                await self._download()
            except Exception:
                _L.exception("download failed")
                return True

            try:
                await self.prepare_codec_info()
            except Exception:
                _L.exception("ffmpeg failed")
                return True
            if self.is_skippable:
                _L.info("nothing to do, skip")
                set_cache(self.dsn, self.node, True, True)
                return True

            set_cache(self.dsn, self.node, self.is_faststart, self.is_native_codec)

            if remux_only and not self.is_native_codec:
                _L.info("need transcode, skip")
                return True

            if transcode_only and self.is_native_codec:
                _L.info("no need transcode, skip")
                return True

            if cache_only:
                _L.info("cached, skip")
                return True

            self._dump_info()
            transcode_command = self._get_transcode_command()
            _L.info(" ".join(transcode_command))

            exit_code = await shell_call(transcode_command, self.output_folder)
            if exit_code != 0:
                _L.error("ffmpeg failed")
                return True
            media_info = await get_video_info(self.transcoded_file_path)
            _L.info(media_info)

            async with self._remote_context():
                node = await self._upload(media_info)
                await self._verify(node)

            set_cache(self.dsn, node, True, True)
            return True

    def _get_transcode_command(self):
        main_cmd = ["ffmpeg", "-nostdin", "-y"]
        input_cmd = ["-i", str(self.raw_file_path)]
        codec_cmd = self.codec_command
        output_path = self.transcoded_file_path
        cmd = main_cmd + input_cmd + codec_cmd + [str(output_path)]
        return cmd

    async def _delete_remote(self):
        _L.info(f"removing {self.node.name}")
        await self.drive.move(self.node, trashed=True)
        await wait_for_sync(self.drive)
        _L.info(f"removed {self.node.name}")

    async def _download(self):
        _L.info(f"downloading {self.node.name}")
        output_folder = self.output_folder
        downloaded_path = await download_file_to_local(
            self.drive, self.node, output_folder
        )
        output_path = self.raw_file_path
        downloaded_path.rename(output_path)
        _L.info(f"downloaded {self.node.name}")

    async def _upload(self, media_info: MediaInfo):
        from mimetypes import guess_type

        output_path = self.transcoded_file_path
        _L.info(f"uploading {output_path}")
        assert self.node.parent_id
        parent_node = await self.drive.get_node_by_id(self.node.parent_id)
        type_, _ext = guess_type(output_path)
        node = await upload_file_from_local(
            self.drive, output_path, parent_node, mime_type=type_, media_info=media_info
        )
        _L.info(f"uploaded {node.id}")
        return node

    async def _verify(self, uploaded_node: Node):
        output_path = self.transcoded_file_path
        _L.info(f"verifying {output_path}")
        local_hash = await get_file_hash(output_path, pool=self.pool, drive=self.drive)
        if local_hash != uploaded_node.hash:
            _L.info(f"removing {uploaded_node.name}")
            await self.drive.move(uploaded_node, trashed=True)
            _L.info(f"removed {uploaded_node.name}")
            raise Exception("hash mismatch")
        _L.info(f"verified {uploaded_node.hash}")

    @contextmanager
    def _local_context(self):
        output_folder = self.output_folder
        output_folder.mkdir(exist_ok=True)
        try:
            yield
        finally:
            shutil.rmtree(output_folder)
            _L.info(f"deleted {output_folder}")

    @asynccontextmanager
    async def _remote_context(self):
        await self._rename_remote()
        try:
            yield
            await self._delete_remote()
            unset_cache(self.dsn, self.node)
        except Exception:
            _L.exception("upload error")
            await self._restore_remote()
            raise

    async def _rename_remote(self):
        await self.drive.move(self.node, new_name=f"__{self.node.name}")
        _L.debug("confirming rename")
        while True:
            await wait_for_sync(self.drive)
            new_node = await self.drive.get_node_by_id(self.node.id)
            if new_node.name != self.node.name:
                break
            await asyncio.sleep(1)
        _L.debug("rename confirmed")

    async def _restore_remote(self):
        await self.drive.move(self.node, new_name=self.node.name)
        _L.debug("confirming restore")
        while True:
            await wait_for_sync(self.drive)
            new_node = await self.drive.get_node_by_id(self.node.id)
            if new_node.name == self.node.name:
                break
            await asyncio.sleep(1)
        _L.debug("restore confirmed")

    def _dump_info(self):
        _L.info(f"node id: {self.node.id}")
        _L.info(f"node name: {self.node.name}")
        _L.info(f"is faststart: {self.is_faststart}")
        _L.info(f"is h264: {self.is_h264}")
        _L.info(f"is aac: {self.is_aac}")


class MP4Processor(VideoProcessor):
    def __init__(
        self, *, work_folder: Path, dsn: Path, pool: Executor, drive: Drive, node: Node
    ):
        super().__init__(
            work_folder=work_folder, dsn=dsn, pool=pool, drive=drive, node=node
        )

    async def prepare_codec_info(self):
        await self.update_codec_from_ffmpeg()

    @property
    def transcoded_file_name(self):
        return self.node.name


class MaybeH264Processor(VideoProcessor):
    def __init__(
        self, *, work_folder: Path, dsn: Path, pool: Executor, drive: Drive, node: Node
    ):
        super().__init__(
            work_folder=work_folder, dsn=dsn, pool=pool, drive=drive, node=node
        )

    async def prepare_codec_info(self):
        await self.update_codec_from_ffmpeg()
        self.is_faststart = False

    @property
    def transcoded_file_name(self):
        name, _ext = splitext(self.node.name)
        return name + ".mp4"


class MKVProcessor(VideoProcessor):
    def __init__(
        self, *, work_folder: Path, dsn: Path, pool: Executor, drive: Drive, node: Node
    ):
        super().__init__(
            work_folder=work_folder, dsn=dsn, pool=pool, drive=drive, node=node
        )

    async def prepare_codec_info(self):
        await self.update_codec_from_ffmpeg()
        self.is_faststart = False

    @property
    def transcoded_file_name(self):
        name, _ext = splitext(self.node.name)
        return name + ".mp4"


class NeverH264Processor(VideoProcessor):
    def __init__(
        self, *, work_folder: Path, dsn: Path, pool: Executor, drive: Drive, node: Node
    ):
        super().__init__(
            work_folder=work_folder, dsn=dsn, pool=pool, drive=drive, node=node
        )

    async def prepare_codec_info(self):
        await self.update_codec_from_ffmpeg()
        self.is_faststart = False

    @property
    def transcoded_file_name(self):
        name, _ext = splitext(self.node.name)
        return name + ".mp4"


async def shell_pipe(cmd_list: list[str]):
    p = await asyncio.create_subprocess_exec(
        *cmd_list, stdout=subprocess.PIPE, stderr=subprocess.PIPE
    )
    out, err = await p.communicate()
    return out.decode("utf-8", "ignore"), err.decode("utf-8", "ignore")


async def shell_call(cmd_list: list[str], folder: Path):
    with open(folder / "shell.log", "ab") as out:
        p = await asyncio.create_subprocess_exec(
            *cmd_list, stdout=out, stderr=subprocess.STDOUT
        )
        return await p.wait()


async def wait_for_sync(drive: Drive):
    async for change in drive.sync():
        _L.debug(change)


def has_enough_quota(drive: Drive, size: int) -> bool:
    total = get_daily_usage(drive)
    return (total + size) < DAILY_UPLOAD_QUOTA


PROCESSOR_TABLE: dict[str, type[VideoProcessor]] = {
    "video/mp4": MP4Processor,
    "video/x-matroska": MKVProcessor,
    "video/x-msvideo": MaybeH264Processor,
    "video/x-ms-wmv": NeverH264Processor,
    "video/quicktime": MaybeH264Processor,
    "video/mpeg": MaybeH264Processor,
}


def create_processor(
    *,
    work_folder: Path,
    dsn: Path,
    pool: Executor,
    drive: Drive,
    node: Node,
) -> VideoProcessor | None:
    if node.mime_type in PROCESSOR_TABLE:
        constructor = PROCESSOR_TABLE[node.mime_type]
        return constructor(
            work_folder=work_folder, dsn=dsn, pool=pool, drive=drive, node=node
        )
    return None
