import asyncio
import contextlib
import json
import os.path
import pathlib
import shutil
import subprocess
from typing import Union

from wcpan.drive.core.drive import Drive, Node, MediaInfo
from wcpan.drive.core.util import upload_from_local, download_to_local
from wcpan.logger import INFO, DEBUG, ERROR, EXCEPTION

from .cache import is_migrated, set_migrated


H264_PRESET = 'veryfast'
H264_CRF = '18'


class VideoProcessor(object):

    def __init__(self, work_folder: str, drive: Drive, node: Node):
        self.work_folder = pathlib.Path(work_folder)
        self.drive = drive
        self.node = node
        # implies mp4
        self.is_faststart = False
        self.is_h264 = False
        self.is_aac = False

    async def prepare_codec_info(self):
        raise NotImplementedError()

    @property
    def transcoded_file_name(self) -> str:
        raise NotImplementedError()

    @property
    def codec_command(self):
        if self.is_h264:
            vc = ['-c:v', 'copy']
        else:
            vc = ['-c:v', 'libx264', '-crf', H264_CRF, '-preset', H264_PRESET]
        if self.is_aac:
            ac = ['-c:a', 'copy']
        else:
            ac = []
        return ac + vc + ['-movflags', '+faststart']

    @property
    def output_folder(self) -> pathlib.Path:
        folder = self.work_folder / self.node.id_
        return folder

    @property
    def raw_file_path(self) -> pathlib.Path:
        return self.output_folder / f'__{self.node.name}'

    @property
    def transcoded_file_path(self) -> pathlib.Path:
        return self.output_folder / self.transcoded_file_name

    async def update_codec_from_ffmpeg(self):
        cmd = [
            'ffprobe',
            '-v', 'trace',
            '-show_streams',
            '-print_format', 'json',
            '-i', self.raw_file_path,
        ]
        out, err = await shell_pipe(cmd)
        data = json.loads(out)
        data = data['streams']
        for stream in data:
            if stream['codec_type'] == 'audio':
                if not self.is_aac:
                    self.is_aac = stream['codec_name'] == 'aac'
            elif stream['codec_type'] == 'video':
                if not self.is_h264:
                    self.is_h264 = stream['codec_name'] == 'h264'
        self.is_faststart = err.find('bytes read, 0 seeks') >= 0

    @property
    def is_skippable(self):
        return self.is_faststart and not self.need_transcode

    @property
    def need_transcode(self):
        return not (self.is_h264 and self.is_aac)

    async def __call__(self):
        if is_migrated(self.node):
            return

        with self._local_context():
            try:
                await self._download()
            except Exception as e:
                EXCEPTION('faststart', e) << 'download failed'
                return

            await self.prepare_codec_info()
            if self.is_skippable:
                set_migrated(self.node)
                return

            if self.need_transcode:
                INFO('faststart') << 'need transcode, skip'
                return

            self._dump_info()
            transcode_command = self._get_transcode_command()
            INFO('faststart') << transcode_command

            exit_code = await shell_call(transcode_command)
            if exit_code != 0:
                ERROR('faststart') << 'ffmpeg failed'
                return

            async with self._upload_context():
                node = await self._upload()

            set_migrated(node)

    def _get_transcode_command(self):
        main_cmd = ['ffmpeg', '-nostdin', '-y']
        input_cmd = ['-i', str(self.raw_file_path)]
        codec_cmd = self.codec_command
        output_path = self.transcoded_file_path
        cmd = main_cmd + input_cmd + codec_cmd + [str(output_path)]
        return cmd

    async def _delete_remote(self):
        INFO('faststart') << 'removing' << self.node.name
        await self.drive.trash_node(self.node)
        await wait_for_sync(self.drive)
        INFO('faststart') << 'removed' << self.node.name

    async def _download(self):
        INFO('faststart') << 'downloading' << self.node.name
        output_folder = self.output_folder
        downloaded_path = await download_to_local(self.drive, self.node, output_folder)
        output_path = self.raw_file_path
        downloaded_path.rename(output_path)
        INFO('faststart') << 'downloaded' << self.raw_file_path

    async def _upload(self):
        output_path = self.transcoded_file_path
        INFO('faststart') << 'uploading' << output_path
        parent_node = await self.drive.get_node_by_id(self.node.parent_id)
        media_info = MediaInfo.video(self.node.video_width, self.node.video_height, self.node.video_ms_duration)
        node = await upload_from_local(self.drive, parent_node, output_path, media_info)
        INFO('faststart') << 'uploaded' << node.id_
        return node

    @contextlib.contextmanager
    def _local_context(self):
        output_folder = self.output_folder
        output_folder.mkdir(exist_ok=True)
        try:
            yield
        finally:
            shutil.rmtree(output_folder)
            INFO('faststart') << 'deleted' << output_folder

    @contextlib.asynccontextmanager
    async def _upload_context(self):
        await self._rename_remote()
        try:
            yield
            await self._delete_remote()
        except Exception as e:
            EXCEPTION('faststart', e) << 'upload error'
            await self._restore_remote()
            raise

    async def _rename_remote(self):
        await self.drive.rename_node(self.node, new_name=f'__{self.node.name}')
        DEBUG('faststart') << 'confirming rename'
        while True:
            await wait_for_sync(self.drive)
            new_node = await self.drive.get_node_by_id(self.node.id_)
            if new_node.name != self.node.name:
                break
            await asyncio.sleep(1)
        DEBUG('faststart') << 'rename confirmed'

    async def _restore_remote(self):
        await self.drive.rename_node(self.node, new_name=self.node.name)
        DEBUG('faststart') << 'confirming restore'
        while True:
            await wait_for_sync(self.drive)
            new_node = await self.drive.get_node_by_id(self.node.id_)
            if new_node.name == self.node.name:
                break
            await asyncio.sleep(1)
        DEBUG('faststart') << 'restore confirmed'

    def _dump_info(self):
        INFO('faststart') << f'node id: {self.node.id_}'
        INFO('faststart') << f'node name: {self.node.name}'
        INFO('faststart') << f'is faststart: {self.is_faststart}'
        INFO('faststart') << f'is h264: {self.is_h264}'
        INFO('faststart') << f'is aac: {self.is_aac}'


class MP4Processer(VideoProcessor):

    def __init__(self, work_folder: str, drive: Drive, node: Node):
        super().__init__(work_folder, drive, node)

    async def prepare_codec_info(self):
        await self.update_codec_from_ffmpeg()

    @property
    def transcoded_file_name(self):
        return self.node.name


class MaybeH264Processer(VideoProcessor):

    def __init__(self, work_folder: str, drive: Drive, node: Node):
        super().__init__(work_folder, drive, node)

    async def prepare_codec_info(self):
        await self.update_codec_from_ffmpeg()
        self.is_faststart = False

    @property
    def transcoded_file_name(self):
        name, ext = os.path.splitext(self.node.name)
        return name + '.mp4'


class MKVProcesser(VideoProcessor):

    def __init__(self, work_folder: str, drive: Drive, node: Node):
        super().__init__(work_folder, drive, node)

    async def prepare_codec_info(self):
        await self.update_codec_from_ffmpeg()
        self.is_faststart = False

    @property
    def transcoded_file_name(self):
        name, ext = os.path.splitext(self.node.name)
        return name + '.mp4'


class NeverH264Processer(VideoProcessor):

    def __init__(self, work_folder: str, drive: Drive, node: Node):
        super().__init__(work_folder, drive, node)

    async def prepare_codec_info(self):
        await self.update_codec_from_ffmpeg()
        self.is_faststart = False

    @property
    def transcoded_file_name(self):
        name, ext = os.path.splitext(self.node.name)
        return name + '.mp4'


async def shell_pipe(cmd_list: list[str]):
    p = await asyncio.create_subprocess_exec(*cmd_list, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = await p.communicate()
    return out.decode('utf-8'), err.decode('utf-8')


async def shell_call(cmd_list: list[str]):
    with open('./data/shell.log', 'ab') as out:
        p = await asyncio.create_subprocess_exec(*cmd_list, stdout=out, stderr=subprocess.STDOUT)
        return await p.wait()


async def wait_for_sync(drive: Drive):
    async for change in drive.sync():
        DEBUG('faststart') << change


def create_processor(
    work_folder: str,
    drive: Drive,
    node: Node,
) -> Union[VideoProcessor, None]:
    table = {
        'video/mp4': MP4Processer,
        'video/x-matroska': MKVProcesser,
        'video/x-msvideo': MaybeH264Processer,
        'video/x-ms-wmv': NeverH264Processer,
        # '.mp4': MP4Processer,
        # '.mkv': MKVProcesser,
        # '.flv': MaybeH264Processer,
        # '.avi': MaybeH264Processer,
        # '.asf': MaybeH264Processer,
        # '.wmv': NeverH264Processer,
        # '.rm': NeverH264Processer,
        # '.rmvb': NeverH264Processer,
        # '.ogm': NeverH264Processer,
    }
    if node.mime_type not in table:
        return None
    constructor = table[node.mime_type]
    return constructor(work_folder, drive, node)
